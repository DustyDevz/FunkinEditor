/*
 * FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#include "render/gfx/vk_context.hpp"

namespace Funkin::Render::GFX {
    VKContext& VKContext::instance() {
        static VKContext ctx;
        return ctx;
    }

    bool VKContext::initialize(void* nativeWindowHandle, uint16_t width, uint16_t height) {
        if (m_initialized_) return true;

        if (!nativeWindowHandle) {
            LOG_CRIT("initialize: nativeWindowHandle is null");
            return false;
        }

        bgfx::PlatformData pd{};
        pd.nwh = nativeWindowHandle;
        bgfx::setPlatformData(pd);

        bgfx::Init init;
        init.type              = bgfx::RendererType::Count;
        init.resolution.width  = width;
        init.resolution.height = height;
        init.resolution.reset  = BGFX_RESET_VSYNC;

        if (!bgfx::init(init)) {
            LOG_CRIT("initialize: bgfx::init failed");
            return false;
        }

        LOG_PRINT("initialize: bgfx initialized OK, renderer={}",
                   bgfx::getRendererName(bgfx::getRendererType()));

        m_initialized_ = true;
        return true;
    }

    void VKContext::shutdown() {
        if (!m_initialized_) return;
        LOG_PRINT("shutdown: shutting down bgfx");
        bgfx::shutdown();
        m_initialized_ = false;
    }

    void VKContext::frame() {
        if (m_initialized_) bgfx::frame();
    }

    void VKContext::reset(uint16_t width, uint16_t height) {
        if (!m_initialized_) return;
        LOG_PRINT("reset: {}x{}", width, height);
        bgfx::reset(width, height, BGFX_RESET_VSYNC);
    }
}