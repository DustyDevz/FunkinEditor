/*
FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "render/graphics/graphics_device.hpp"
#include <EngineFactoryVk.h>

#include "CommandQueueVk.h"
#include "RenderDeviceVk.h"
#include "utils/log.hpp"

namespace Funkin::Render::Graphics {
    graphics_device &graphics_device::instance() {
        static graphics_device instance;
        return instance;
    }

    bool graphics_device::init(bool enable_validation) {
        if (is_initialized()) return true;

        auto* get_factory_vk = Diligent::LoadGraphicsEngineVk();
        if (!get_factory_vk) {
            LOG_CRIT("get_factory_vk failed");
            return false;
        }

        m_factory_ = get_factory_vk();
        Diligent::EngineVkCreateInfo engine_vk_create_info{};
        engine_vk_create_info.EnableValidation = enable_validation;

        Diligent::IRenderDevice*  raw_device = nullptr;
        Diligent::IDeviceContext* raw_context = nullptr;

        m_factory_->CreateDeviceAndContextsVk(
            engine_vk_create_info,
            &raw_device,
            &raw_context
        );

        if (!raw_device || !raw_context) {
            LOG_CRIT("create_device_and_contexts_vk failed");
            return false;
        }

        m_device_.Attach(raw_device);
        m_context_.Attach(raw_context);

        LOG_PRINT("init OK");
        return true;
    }

    graphics_device::native_vk_handles graphics_device::get_native_vk_handles() const {
        native_vk_handles vk_handles{};
        if (!m_device_ || !m_context_) {
            LOG_CRIT("get_native_vk_handles FAILED");
            return vk_handles;
        }

        Diligent::RefCntAutoPtr<Diligent::IRenderDeviceVk> device_vk{m_device_, Diligent::IID_RenderDeviceVk};
        if (!device_vk) {
            LOG_CRIT("get_native_vk_handles FAILED");
            return vk_handles;
        }

        vk_handles.instance        = device_vk->GetVkInstance();
        vk_handles.physical_device = device_vk->GetVkPhysicalDevice();
        vk_handles.device          = device_vk->GetVkDevice();

        Diligent::ICommandQueue* raw_queue = m_context_->LockCommandQueue();
        if (raw_queue) {
            Diligent::RefCntAutoPtr<Diligent::ICommandQueueVk> cmd_queue_vk{raw_queue, Diligent::IID_CommandQueueVk};
            if (cmd_queue_vk) {
                vk_handles.queue              = cmd_queue_vk->GetVkQueue();
                vk_handles.queue_family_index = cmd_queue_vk->GetQueueFamilyIndex();
                vk_handles.queue_index        = 0;
            } else {
                LOG_CRIT("cmd_queue_vk query interface FAILED");
            }
            m_context_->UnlockCommandQueue();
        } else {
            LOG_CRIT("lock_command_queue FAILED");
        }

        return vk_handles;
    }

    bool graphics_device::create_swap_chain(void *hwnd, uint32_t width, uint32_t height) {
        if (!m_device_ || !m_context_ || !m_factory_) {
            LOG_CRIT("create_swap_chain FAILED, device not initialized");
            return false;
        }

        Diligent::SwapChainDesc swap_chain_desc{};
        swap_chain_desc.Width  = width;
        swap_chain_desc.Height = height;
        swap_chain_desc.ColorBufferFormat = Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB;
        swap_chain_desc.DepthBufferFormat = Diligent::TEX_FORMAT_D32_FLOAT;
        swap_chain_desc.BufferCount = 3;

        Diligent::Win32NativeWindow native_window{hwnd};
        m_factory_->CreateSwapChainVk(m_device_, m_context_, swap_chain_desc, native_window, &m_swap_chain_);

        if (!m_swap_chain_) {
            LOG_CRIT("create_swap_chain FAILED");
            return false;
        }

        LOG_PRINT("create_swap_chain OK ({}, {})", width, height);
        return true;
    }

    void graphics_device::shutdown() {
        if (m_context_) {
            m_context_->Flush();
            m_context_.Release();
        }

        if (m_device_) {
            m_device_.Release();
        }

        m_factory_ = nullptr;
        LOG_PRINT("shutdown OK");
    }
}
