/*
 * FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#include "render/gfx/vk_context.hpp"

namespace Funkin::Render::GFX {
    vk_context& vk_context::instance() {
        static vk_context ctx;
        return ctx;
    }

    bool vk_context::init(void* native_window_handle, uint16_t w, uint16_t h) {
        if (m_initialized_) return true;

        if (!native_window_handle) {
            LOG_CRIT("native_window_handle is null");
            return false;
        }

        bgfx::PlatformData pd{};
        pd.nwh = native_window_handle;
        bgfx::setPlatformData(pd);

        bgfx::Init init;
        init.type              = bgfx::RendererType::Count;
        init.resolution.width  = w;
        init.resolution.height = h;
        init.resolution.reset  = BGFX_RESET_VSYNC;

        if (!bgfx::init(init)) {
            LOG_CRIT("init failed");
            return false;
        }

        LOG_PRINT("init OK, renderer={}",
                   bgfx::getRendererName(bgfx::getRendererType()));

        m_initialized_ = true;
        return true;
    }

    void vk_context::shutdown() {
        if (!m_initialized_) return;
        LOG_PRINT("shutdown");
        bgfx::shutdown();
        m_initialized_ = false;
    }

    void vk_context::frame() {
        if (m_initialized_) bgfx::frame();
    }

    void vk_context::reset(uint16_t w, uint16_t h) {
        if (!m_initialized_) return;
        LOG_PRINT("reset: {}x{}", w, h);
        bgfx::reset(w, h, BGFX_RESET_VSYNC);
    }
}