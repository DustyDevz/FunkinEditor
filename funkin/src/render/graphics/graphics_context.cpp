/*
FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include <NativeWindow.h>
#include <EngineFactoryVk.h>

#include "render/graphics/graphics_context.hpp"
#include "render/graphics/graphics_device.hpp"
#include "utils/log.hpp"

namespace Funkin::Render::Graphics {
    graphics_context::~graphics_context() {
        shutdown();
    }

    bool graphics_context::init(uint32_t width, uint32_t height) {
        if (m_is_initialized) return true;

        if (!create_targets(width, height)) {
            LOG_CRIT("Failed to create offscreen render targets");
            return false;
        }

        m_is_initialized = true;
        LOG_PRINT("graphics_context offscreen init OK ({}x{})", width, height);
        return true;
    }

    void graphics_context::shutdown() {
        if (!m_is_initialized) return;

        if (auto* context = graphics_device::instance().context()) {
            context->Flush();
            context->WaitForIdle();
        }

        m_render_target.Release();
        m_depth_buffer.Release();
        m_is_initialized = false;
        LOG_PRINT("graphics_context shutdown OK");
    }

    void graphics_context::resize(uint32_t width, uint32_t height) {
        if (!m_is_initialized || (width == m_width && height == m_height)) return;
        create_targets(width, height);
    }

    void graphics_context::present() {
        auto* context = graphics_device::instance().context();
        auto* swap_chain = graphics_device::instance().swap_chain();
        if (!context || !swap_chain) return;

        swap_chain->Present(0);
    }

    void graphics_context::begin_frame(float r, float g, float b, float a) {
        if (!m_is_initialized) return;

        // TEMP DEBUGGING //
        auto now = std::chrono::high_resolution_clock::now();
        if (m_last_time_.time_since_epoch().count() > 0) {
            std::chrono::duration<double, std::milli> delta = now - m_last_time_;
            m_accumulated_time_ += delta.count();
            m_frame_count_++;

            if (m_accumulated_time_ >= 200.) {
                m_frame_time_ms_ = m_accumulated_time_ / m_frame_count_;
                m_fps_ = 1000.0 / m_frame_time_ms_;
                m_accumulated_time_ = 0.0;
                m_frame_count_ = 0;
            }
        }
        m_last_time_ = now;
        // TEMP DEBUGGING //

        auto* context = graphics_device::instance().context();
        if (!context) return;

        auto* swap_chain = graphics_device::instance().swap_chain();
        Diligent::ITextureView* rtv;
        Diligent::ITextureView* dsv;

        if (swap_chain) {
            rtv = swap_chain->GetCurrentBackBufferRTV();
            dsv = swap_chain->GetDepthBufferDSV();
        } else {
            rtv = m_render_target->GetDefaultView(Diligent::TEXTURE_VIEW_RENDER_TARGET);
            dsv = m_depth_buffer ->GetDefaultView(Diligent::TEXTURE_VIEW_DEPTH_STENCIL);
        }
        context->SetRenderTargets(1, &rtv, dsv, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float clear_color[4] = { r, g, b, a };
        context->ClearRenderTarget(rtv, clear_color, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        context->ClearDepthStencil(dsv, Diligent::CLEAR_DEPTH_FLAG, 1.0f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY);
    }

    VkImage graphics_context::get_vk_image() const {
        if (!m_render_target) return VK_NULL_HANDLE;

        Diligent::RefCntAutoPtr<Diligent::ITextureVk> tex_vk{m_render_target, Diligent::IID_TextureVk};
        return tex_vk ? tex_vk->GetVkImage() : VK_NULL_HANDLE;
    }

    void graphics_context::end_frame() {
        if (!m_is_initialized) return;
        // cats
    }

    bool graphics_context::create_targets(uint32_t width, uint32_t height) {
        auto* device = graphics_device::instance().device();
        if (!device) return false;

        m_render_target.Release();
        m_depth_buffer.Release();

        Diligent::TextureDesc color_desc{};
        color_desc.Name      = "Offscreen Color Target";
        color_desc.Type      = Diligent::RESOURCE_DIM_TEX_2D;
        color_desc.Width     = width;
        color_desc.Height    = height;
        color_desc.Format    = Diligent::TEX_FORMAT_RGBA8_UNORM;
        color_desc.BindFlags = Diligent::BIND_RENDER_TARGET | Diligent::BIND_SHADER_RESOURCE;

        device->CreateTexture(color_desc, nullptr, &m_render_target);

        Diligent::TextureDesc depth_desc{};
        depth_desc.Name      = "Offscreen Depth Buffer";
        depth_desc.Type      = Diligent::RESOURCE_DIM_TEX_2D;
        depth_desc.Width     = width;
        depth_desc.Height    = height;
        depth_desc.Format    = Diligent::TEX_FORMAT_D32_FLOAT;
        depth_desc.BindFlags = Diligent::BIND_DEPTH_STENCIL;

        device->CreateTexture(depth_desc, nullptr, &m_depth_buffer);

        m_width  = width;
        m_height = height;
        return (m_render_target && m_depth_buffer);
    }
}