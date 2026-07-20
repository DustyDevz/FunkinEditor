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

    bool graphics_context::init(void *native_window_handle, uint16_t w, uint16_t h) {
        if (m_is_initialized) return true;

        if (!native_window_handle) {
            LOG_CRIT("init FAILED, native_window_handle is null");
            return false;
        }

        auto& device = graphics_device::instance();
        if (!device.is_initialized()) {
            LOG_CRIT("init FAILED, graphics device is not initialized");
            return false;
        }

        Diligent::SwapChainDesc sc_desc{};
        sc_desc.Width = w;
        sc_desc.Height = h;
        sc_desc.ColorBufferFormat = Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB;
        sc_desc.DepthBufferFormat = Diligent::TEX_FORMAT_D32_FLOAT;

        Diligent::Win32NativeWindow window { static_cast<HWND>(native_window_handle) };
        Diligent::IEngineFactoryVk* factory_vk = device.factory();
        if (!factory_vk) {
            LOG_CRIT("init FAILED, graphics device factory is null");
            return false;
        }

        Diligent::ISwapChain* raw_swap_chain = nullptr;
        factory_vk->CreateSwapChainVk(
            device.device(),
            device.context(),
            sc_desc,
            window,
            &raw_swap_chain);

        if (!raw_swap_chain) {
            LOG_CRIT("init FAILED, failed to create swap chain");
            return false;
        }

        m_swap_chain.Attach(raw_swap_chain);
        m_is_initialized = true;

        LOG_PRINT("init OK (w={}, h={})", w, h);
        return true;
    }

    void graphics_context::shutdown() {
        if (!m_is_initialized) return;

        if (auto* graphics_context = graphics_device::instance().context()) {
            graphics_context->Flush();
        }

        m_swap_chain.Release();
        m_is_initialized = false;
        LOG_PRINT("shutdown OK");
    }

    void graphics_context::resize(uint16_t w, uint16_t h) {
        if (!m_is_initialized || !m_swap_chain) return;
        m_swap_chain->Resize(w, h);
    }

    void graphics_context::begin_frame(float r, float g, float b, float a) {
        if (!m_is_initialized) return;

        auto* graphics_context = graphics_device::instance().context();
        if (!graphics_context) return;

        auto* rtv = m_swap_chain->GetCurrentBackBufferRTV();
        auto* dsv = m_swap_chain->GetDepthBufferDSV();
        graphics_context->SetRenderTargets(1, &rtv, dsv, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float clear_color[4] = { r, g, b, a };
        graphics_context->ClearRenderTarget(rtv, clear_color, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        graphics_context->ClearDepthStencil(dsv, Diligent::CLEAR_DEPTH_FLAG, 1.f, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    }

    void graphics_context::present() {
        if (!m_is_initialized || !m_swap_chain) return;
        m_swap_chain->Present();
    }
}
