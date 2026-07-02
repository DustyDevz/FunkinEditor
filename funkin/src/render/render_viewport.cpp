/*
 * FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#include "render/render_viewport.hpp"
#include "utils/log.hpp"

#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <cstddef>
#include <iterator>
#include <vulkan/vulkan.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define ENGINE_DLL 1
#include <RenderDevice.h>
#include <DeviceContext.h>
#include <Texture.h>
#include <EngineFactoryVk.h>
#include <RenderDeviceVk.h>

#include "RefCntAutoPtr.hpp"

namespace Funkin::Render {
    RenderViewport::RenderViewport()
        : m_device_(nullptr),
          m_context_(nullptr),
          m_colorTarget_(nullptr),
          m_depthTarget_(nullptr),
          m_stagingTexture_(nullptr),
        #ifdef _WIN32
          m_sharedImage_(VK_NULL_HANDLE),
          m_sharedMemory_(VK_NULL_HANDLE),
          m_diligentVkDevice_(VK_NULL_HANDLE),
        #endif
          m_targetWidth_(0),
          m_targetHeight_(0),
          m_initialized_(false),
          m_pendingResize_(false),
          m_frameDirty_(false)
    {
        setFlag(ItemHasContents, true);

        m_renderTimer_.setInterval(16);
        connect(&m_renderTimer_, &QTimer::timeout, this, &RenderViewport::onFrameTick);
        m_renderTimer_.start();

        LOG_PRINT("RenderViewport constructed");
    }

    RenderViewport::~RenderViewport() {
        LOG_PRINT("RenderViewport destructing");
        m_renderTimer_.stop();

        if (m_context_) m_context_->Flush();

        destroySharedColorImage();

        if (m_stagingTexture_) m_stagingTexture_->Release();
        if (m_depthTarget_)    m_depthTarget_->Release();
        if (m_context_)        m_context_->Release();
        if (m_device_)         m_device_->Release();

        m_stagingTexture_ = nullptr;
        m_depthTarget_    = nullptr;
        m_context_        = nullptr;
        m_device_         = nullptr;
    }

    void RenderViewport::init() {
        if (m_initialized_) return;

        LOG_PRINT("init: creating Vulkan device+context (with external memory extensions)");
        auto* pFactoryVk = Diligent::GetEngineFactoryVk();

        Diligent::SetDebugMessageCallback([](Diligent::DEBUG_MESSAGE_SEVERITY Severity, const char* Message, const char* Function, const char* File, int Line) {
            switch (Severity) {
                case Diligent::DEBUG_MESSAGE_SEVERITY_INFO:
                    break;
                case Diligent::DEBUG_MESSAGE_SEVERITY_WARNING:
                    ::Funkin::Utils::LogMessage(::Funkin::Utils::LogLevel::Warning, File ? File : "", Line, "[Diligent] {}", Message);
                    break;
                case Diligent::DEBUG_MESSAGE_SEVERITY_ERROR:
                    ::Funkin::Utils::LogMessage(::Funkin::Utils::LogLevel::Error, File ? File : "", Line, "[Diligent] {}", Message);
                    break;
                case Diligent::DEBUG_MESSAGE_SEVERITY_FATAL_ERROR:
                    ::Funkin::Utils::LogMessage(::Funkin::Utils::LogLevel::Critical, File ? File : "", Line, "[Diligent Fatal] {}", Message);
                    break;
            }
        });

        static const char* instanceExtensions[] = {
            "VK_KHR_get_physical_device_properties2",
            "VK_KHR_external_memory_capabilities",
            "VK_KHR_external_semaphore_capabilities",
        };

        static const char* deviceExtensions[] = {
            "VK_KHR_external_memory",
            "VK_KHR_external_memory_win32",
            "VK_KHR_external_semaphore",
            "VK_KHR_external_semaphore_win32",
        };

        Diligent::EngineVkCreateInfo EngineCI;
        EngineCI.InstanceLayerCount         = 0;
        EngineCI.InstanceExtensionCount     = static_cast<Diligent::Uint32>(std::size(instanceExtensions));
        EngineCI.ppInstanceExtensionNames   = instanceExtensions;
        EngineCI.DeviceExtensionCount       = static_cast<Diligent::Uint32>(std::size(deviceExtensions));
        EngineCI.ppDeviceExtensionNames     = deviceExtensions;

        pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &m_device_, &m_context_);
        if (!m_device_ || !m_context_) {
            LOG_CRIT("init: FAILED to create Vulkan device/context");
            return;
        }

        #ifdef _WIN32
            Diligent::RefCntAutoPtr<Diligent::IRenderDeviceVk> pDeviceVk;
            m_device_->QueryInterface(Diligent::IID_RenderDeviceVk, reinterpret_cast<Diligent::IObject**>(static_cast<Diligent::IRenderDeviceVk**>(&pDeviceVk)));
            if (!pDeviceVk) {
                LOG_CRIT("init: QueryInterface for IRenderDeviceVk FAILED");
                return;
            }
            m_diligentVkDevice_ = pDeviceVk->GetVkDevice();
            LOG_PRINT("init: cached Diligent's raw VkDevice handle for external memory ops");
        #endif

        LOG_PRINT("init: device+context created OK");
        m_initialized_ = true;
    }

#ifdef _WIN32
    bool RenderViewport::createSharedColorImage(int width, int height) {
        if (m_diligentVkDevice_ == VK_NULL_HANDLE) {
            LOG_ERR("createSharedColorImage: Diligent VkDevice not cached yet");
            return false;
        }

        HMODULE vulkanLib = GetModuleHandleA("vulkan-1.dll");
        if (!vulkanLib) {
            vulkanLib = LoadLibraryA("vulkan-1.dll");
        }
        if (!vulkanLib) {
            LOG_ERR("createSharedColorImage: Failed to load vulkan-1.dll");
            return false;
        }

        auto pfnVkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(vulkanLib, "vkGetInstanceProcAddr"));
        if (!pfnVkGetInstanceProcAddr) return false;

        Diligent::RefCntAutoPtr<Diligent::IRenderDeviceVk> pDeviceVk;
        m_device_->QueryInterface(Diligent::IID_RenderDeviceVk, reinterpret_cast<Diligent::IObject**>(static_cast<Diligent::IRenderDeviceVk**>(&pDeviceVk)));
        if (!pDeviceVk) return false;

        VkInstance instance = pDeviceVk->GetVkInstance();
        auto pfnVkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(pfnVkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr"));
        if (!pfnVkGetDeviceProcAddr) return false;

        auto pfnVkCreateImage = reinterpret_cast<PFN_vkCreateImage>(pfnVkGetDeviceProcAddr(m_diligentVkDevice_, "vkCreateImage"));
        auto pfnVkGetImageMemoryRequirements = reinterpret_cast<PFN_vkGetImageMemoryRequirements>(pfnVkGetDeviceProcAddr(m_diligentVkDevice_, "vkGetImageMemoryRequirements"));
        auto pfnVkAllocateMemory = reinterpret_cast<PFN_vkAllocateMemory>(pfnVkGetDeviceProcAddr(m_diligentVkDevice_, "vkAllocateMemory"));
        auto pfnVkBindImageMemory = reinterpret_cast<PFN_vkBindImageMemory>(pfnVkGetDeviceProcAddr(m_diligentVkDevice_, "vkBindImageMemory"));
        auto pfnVkDestroyImage = reinterpret_cast<PFN_vkDestroyImage>(pfnVkGetDeviceProcAddr(m_diligentVkDevice_, "vkDestroyImage"));
        auto pfnVkGetPhysicalDeviceMemoryProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(pfnVkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties"));

        if (!pfnVkCreateImage || !pfnVkGetImageMemoryRequirements || !pfnVkAllocateMemory ||
            !pfnVkBindImageMemory || !pfnVkDestroyImage || !pfnVkGetPhysicalDeviceMemoryProperties) {
            LOG_ERR("createSharedColorImage: Failed to resolve one or more Vulkan symbols via proc address");
            return false;
        }

        VkExternalMemoryImageCreateInfo externalImageInfo{};
        externalImageInfo.sType       = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO;
        externalImageInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

        VkImageCreateInfo imageInfo{};
        imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.pNext         = &externalImageInfo;
        imageInfo.imageType     = VK_IMAGE_TYPE_2D;
        imageInfo.format        = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.extent        = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
        imageInfo.mipLevels     = 1;
        imageInfo.arrayLayers   = 1;
        imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkResult res = pfnVkCreateImage(m_diligentVkDevice_, &imageInfo, nullptr, &m_sharedImage_);
        if (res != VK_SUCCESS) {
            LOG_ERR("createSharedColorImage: vkCreateImage failed, VkResult={}", static_cast<int>(res));
            return false;
        }

        VkMemoryRequirements memReqs{};
        pfnVkGetImageMemoryRequirements(m_diligentVkDevice_, m_sharedImage_, &memReqs);

        VkPhysicalDeviceMemoryProperties memProps{};
        pfnVkGetPhysicalDeviceMemoryProperties(pDeviceVk->GetVkPhysicalDevice(), &memProps);

        uint32_t memTypeIndex = UINT32_MAX;
        for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
            const bool typeAllowed = (memReqs.memoryTypeBits & (1u << i)) != 0;
            const bool isDeviceLocal = (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0;
            if (typeAllowed && isDeviceLocal) {
                memTypeIndex = i;
                break;
            }
        }
        if (memTypeIndex == UINT32_MAX) {
            LOG_ERR("createSharedColorImage: no suitable device-local memory type found");
            pfnVkDestroyImage(m_diligentVkDevice_, m_sharedImage_, nullptr);
            m_sharedImage_ = VK_NULL_HANDLE;
            return false;
        }

        VkExportMemoryAllocateInfo exportInfo{};
        exportInfo.sType       = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO;
        exportInfo.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.pNext           = &exportInfo;
        allocInfo.allocationSize  = memReqs.size;
        allocInfo.memoryTypeIndex = memTypeIndex;

        res = pfnVkAllocateMemory(m_diligentVkDevice_, &allocInfo, nullptr, &m_sharedMemory_);
        if (res != VK_SUCCESS) {
            LOG_ERR("createSharedColorImage: vkAllocateMemory failed, VkResult={}", static_cast<int>(res));
            pfnVkDestroyImage(m_diligentVkDevice_, m_sharedImage_, nullptr);
            m_sharedImage_ = VK_NULL_HANDLE;
            return false;
        }

        res = pfnVkBindImageMemory(m_diligentVkDevice_, m_sharedImage_, m_sharedMemory_, 0);
        if (res != VK_SUCCESS) {
            LOG_ERR("createSharedColorImage: vkBindImageMemory failed, VkResult={}", static_cast<int>(res));
            return false;
        }

        LOG_PRINT("createSharedColorImage: raw VkImage+memory created OK ({}x{}, exportable)", width, height);

        Diligent::TextureDesc ColorDesc;
        ColorDesc.Name      = "RenderViewport SharedColorTarget";
        ColorDesc.Type      = Diligent::RESOURCE_DIM_TEX_2D;
        ColorDesc.Width     = static_cast<Diligent::Uint32>(width);
        ColorDesc.Height    = static_cast<Diligent::Uint32>(height);
        ColorDesc.Format    = Diligent::TEX_FORMAT_RGBA8_UNORM;
        ColorDesc.BindFlags = Diligent::BIND_RENDER_TARGET | Diligent::BIND_SHADER_RESOURCE;
        ColorDesc.Usage     = Diligent::USAGE_DEFAULT;

        pDeviceVk->CreateTextureFromVulkanImage(m_sharedImage_, ColorDesc, Diligent::RESOURCE_STATE_UNDEFINED, &m_colorTarget_);
        if (!m_colorTarget_) {
            LOG_ERR("createSharedColorImage: CreateTextureFromVulkanImage FAILED");
            return false;
        }

        LOG_PRINT("createSharedColorImage: wrapped into Diligent ITexture OK");
        return true;
    }

    void RenderViewport::destroySharedColorImage() {
        if (m_colorTarget_) {
            m_colorTarget_->Release();
            m_colorTarget_ = nullptr;
        }

        if (m_diligentVkDevice_ != VK_NULL_HANDLE) {
            HMODULE vulkanLib = GetModuleHandleA("vulkan-1.dll");
            if (vulkanLib) {
                auto pfnVkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(vulkanLib, "vkGetInstanceProcAddr"));
                Diligent::RefCntAutoPtr<Diligent::IRenderDeviceVk> pDeviceVk;
                m_device_->QueryInterface(Diligent::IID_RenderDeviceVk, reinterpret_cast<Diligent::IObject**>(static_cast<Diligent::IRenderDeviceVk**>(&pDeviceVk)));

                if (pfnVkGetInstanceProcAddr && pDeviceVk) {
                    VkInstance instance = pDeviceVk->GetVkInstance();
                    auto pfnVkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(pfnVkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr"));
                    if (pfnVkGetDeviceProcAddr) {
                        auto pfnVkDestroyImage = reinterpret_cast<PFN_vkDestroyImage>(pfnVkGetDeviceProcAddr(m_diligentVkDevice_, "vkDestroyImage"));
                        auto pfnVkFreeMemory = reinterpret_cast<PFN_vkFreeMemory>(pfnVkGetDeviceProcAddr(m_diligentVkDevice_, "vkFreeMemory"));

                        if (m_sharedImage_ != VK_NULL_HANDLE && pfnVkDestroyImage) {
                            pfnVkDestroyImage(m_diligentVkDevice_, m_sharedImage_, nullptr);
                            m_sharedImage_ = VK_NULL_HANDLE;
                        }
                        if (m_sharedMemory_ != VK_NULL_HANDLE && pfnVkFreeMemory) {
                            pfnVkFreeMemory(m_diligentVkDevice_, m_sharedMemory_, nullptr);
                            m_sharedMemory_ = VK_NULL_HANDLE;
                        }
                    }
                }
            }
        }
    }
#endif

    void RenderViewport::resizeRenderTarget(int width, int height) {
        if (width <= 0 || height <= 0) return;
        if (width == m_targetWidth_ && height == m_targetHeight_ && m_colorTarget_) return;

        LOG_PRINT("resizeRenderTarget: {}x{}", width, height);

        if (m_stagingTexture_) { m_stagingTexture_->Release(); m_stagingTexture_ = nullptr; }
        if (m_depthTarget_)    { m_depthTarget_->Release();    m_depthTarget_    = nullptr; }
#ifdef _WIN32
        destroySharedColorImage();

        if (!createSharedColorImage(width, height)) {
            LOG_CRIT("resizeRenderTarget: createSharedColorImage FAILED, viewport will not render");
            return;
        }
#else
        if (m_colorTarget_) { m_colorTarget_->Release(); m_colorTarget_ = nullptr; }
        Diligent::TextureDesc ColorDesc;
        ColorDesc.Name      = "RenderViewport ColorTarget";
        ColorDesc.Type      = Diligent::RESOURCE_DIM_TEX_2D;
        ColorDesc.Width     = static_cast<Diligent::Uint32>(width);
        ColorDesc.Height    = static_cast<Diligent::Uint32>(height);
        ColorDesc.Format    = Diligent::TEX_FORMAT_RGBA8_UNORM;
        ColorDesc.BindFlags = Diligent::BIND_RENDER_TARGET | Diligent::BIND_SHADER_RESOURCE;
        ColorDesc.Usage     = Diligent::USAGE_DEFAULT;
        m_device_->CreateTexture(ColorDesc, nullptr, &m_colorTarget_);
#endif

        Diligent::TextureDesc DepthDesc;
        DepthDesc.Name      = "RenderViewport DepthTarget";
        DepthDesc.Type      = Diligent::RESOURCE_DIM_TEX_2D;
        DepthDesc.Width     = static_cast<Diligent::Uint32>(width);
        DepthDesc.Height    = static_cast<Diligent::Uint32>(height);
        DepthDesc.Format    = Diligent::TEX_FORMAT_D32_FLOAT;
        DepthDesc.BindFlags = Diligent::BIND_DEPTH_STENCIL;
        DepthDesc.Usage     = Diligent::USAGE_DEFAULT;
        m_device_->CreateTexture(DepthDesc, nullptr, &m_depthTarget_);

        Diligent::TextureDesc StagingDesc;
        StagingDesc.Name           = "RenderViewport Staging";
        StagingDesc.Type           = Diligent::RESOURCE_DIM_TEX_2D;
        StagingDesc.Width          = static_cast<Diligent::Uint32>(width);
        StagingDesc.Height         = static_cast<Diligent::Uint32>(height);
        StagingDesc.Format         = Diligent::TEX_FORMAT_RGBA8_UNORM;
        StagingDesc.BindFlags      = Diligent::BIND_NONE;
        StagingDesc.Usage          = Diligent::USAGE_STAGING;
        StagingDesc.CPUAccessFlags = Diligent::CPU_ACCESS_READ;
        m_device_->CreateTexture(StagingDesc, nullptr, &m_stagingTexture_);

        if (!m_colorTarget_ || !m_depthTarget_ || !m_stagingTexture_) {
            LOG_ERR("resizeRenderTarget: FAILED to create one or more textures (color={} depth={} staging={})",
                     (void*)m_colorTarget_, (void*)m_depthTarget_, (void*)m_stagingTexture_);
            return;
        }

        m_targetWidth_  = width;
        m_targetHeight_ = height;
        LOG_PRINT("resizeRenderTarget: success");
    }

    void RenderViewport::renderFrame() {
        if (!m_initialized_ || !m_context_ || !m_colorTarget_ || !m_depthTarget_) return;

        auto* pRTV = m_colorTarget_->GetDefaultView(Diligent::TEXTURE_VIEW_RENDER_TARGET);
        auto* pDSV = m_depthTarget_->GetDefaultView(Diligent::TEXTURE_VIEW_DEPTH_STENCIL);

        m_context_->SetRenderTargets(1, &pRTV, pDSV, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        float clearColor[] = {.12f, .12f, .12f, 1.f};
        m_context_->ClearRenderTarget(pRTV, clearColor, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_context_->ClearDepthStencil(pDSV, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0,
                                       Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        m_context_->SetRenderTargets(0, nullptr, nullptr, Diligent::RESOURCE_STATE_TRANSITION_MODE_NONE);
    }

    bool RenderViewport::readbackFrame(QImage& outImage) {
        if (!m_context_ || !m_colorTarget_ || !m_stagingTexture_) return false;

        Diligent::CopyTextureAttribs CopyAttribs(m_colorTarget_, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
                                                  m_stagingTexture_, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_context_->CopyTexture(CopyAttribs);

        m_context_->WaitForIdle();

        Diligent::MappedTextureSubresource MappedData;
        m_context_->MapTextureSubresource(m_stagingTexture_, 0, 0, Diligent::MAP_READ,
                                           Diligent::MAP_FLAG_DO_NOT_WAIT, nullptr, MappedData);
        if (!MappedData.pData) {
            LOG_ERR("readbackFrame: MapTextureSubresource returned null data");
            return false;
        }

        outImage = QImage(m_targetWidth_, m_targetHeight_, QImage::Format_RGBA8888);
        const auto* srcBytes = static_cast<const unsigned char*>(MappedData.pData);
        for (int y = 0; y < m_targetHeight_; ++y) {
            memcpy(outImage.scanLine(y), srcBytes + static_cast<size_t>(y) * MappedData.Stride,
                   static_cast<size_t>(m_targetWidth_) * 4);
        }

        m_context_->UnmapTextureSubresource(m_stagingTexture_, 0, 0);
        return true;
    }

    void RenderViewport::onFrameTick() {
        if (!m_initialized_) {
            init();
            return;
        }

        if (m_pendingResize_) {
            resizeRenderTarget(static_cast<int>(width()), static_cast<int>(height()));
            m_pendingResize_ = false;
        }

        if (m_targetWidth_ <= 0 || m_targetHeight_ <= 0) return;

        renderFrame();

        QImage frame;
        if (readbackFrame(frame)) {
            m_latestFrame_ = std::move(frame);
            m_frameDirty_  = true;
            update();
        }
    }

    void RenderViewport::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) {
        QQuickItem::geometryChange(newGeometry, oldGeometry);
        if (newGeometry.width() > 0 && newGeometry.height() > 0) {
            m_pendingResize_ = true;
        }
    }

    QSGNode* RenderViewport::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
        auto* node = static_cast<QSGSimpleTextureNode*>(oldNode);
        if (!node) {
            node = new QSGSimpleTextureNode();
            node->setOwnsTexture(true);

            QImage fallback(1, 1, QImage::Format_RGBA8888);
            fallback.fill(Qt::transparent);
            node->setTexture(window()->createTextureFromImage(fallback));
        }

        if (m_frameDirty_ && !m_latestFrame_.isNull() && window()) {
            QSGTexture* texture = window()->createTextureFromImage(m_latestFrame_);
            node->setTexture(texture);
            m_frameDirty_ = false;
        }

        node->setRect(boundingRect());
        return node;
    }
}