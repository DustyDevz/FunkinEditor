/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "render/gfx/vk_context.hpp"

namespace Funkin::Render::GFX {
    vk_context& vk_context::instance() {
        static vk_context ctx;
        return ctx;
    }

    struct b_callback : public bgfx::CallbackI {
        void fatal(const char* filePath, uint16_t line, bgfx::Fatal::Enum code, const char* str) override {
            LOG_CRIT("bgfx FATAL [{}:{}] code={} : {}", filePath, line, (int)code, str);
        }
        void traceVargs(const char* filePath, uint16_t line, const char* format, va_list argList) override {
            char buf[1024];
            vsnprintf(buf, sizeof(buf), format, argList);
            LOG_PRINT("bgfx trace [{}:{}] {}", filePath, line, buf);
        }
        void profilerBegin(const char*, uint32_t, const char*, uint16_t) override {}
        void profilerBeginLiteral(const char*, uint32_t, const char*, uint16_t) override {}
        void profilerEnd() override {}
        uint32_t cacheReadSize(uint64_t) override { return 0; }
        bool cacheRead(uint64_t, void*, uint32_t) override { return false; }
        void cacheWrite(uint64_t, const void*, uint32_t) override {}
        void screenShot(const char*, uint32_t, uint32_t, uint32_t, bgfx::TextureFormat::Enum, const void*, uint32_t, bool) override {}
        void captureBegin(uint32_t, uint32_t, uint32_t, bgfx::TextureFormat::Enum, bool) override {}
        void captureEnd() override {}
        void captureFrame(const void*, uint32_t) override {}
    };

    static b_callback s_callback;

    bool vk_context::init(void* native_window_handle, uint16_t w, uint16_t h) {
        if (m_initialized_) return true;

        if (!native_window_handle) {
            LOG_CRIT("native_window_handle is null");
            return false;
        }

        bgfx::Init init;
        init.type              = bgfx::RendererType::Vulkan;
        init.resolution.width  = w;
        init.resolution.height = h;
        init.resolution.reset  = BGFX_RESET_VSYNC;
        //init.callback           = &s_callback;
        init.platformData.nwh  = native_window_handle;

        if (!bgfx::init(init)) {
            LOG_CRIT("init failed");
            return false;
        }

        //bgfx::setDebug(BGFX_DEBUG_TEXT | BGFX_DEBUG_STATS);

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