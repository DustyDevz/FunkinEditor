/*
 * FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#pragma once
#include <bgfx/bgfx.h>
#include <cstdint>

namespace Funkin::Render::GFX {
    class VKContext {
    public:
        static VKContext& instance();
        bool initialize(void* nativeWindowHandle, uint16_t width, uint16_t height);
        void shutdown();
        void frame();
        void reset(uint16_t width, uint16_t height);

        bool isInitialized() const { return m_initialized_; }

    private:
        VKContext() = default;
        bool m_initialized_ = false;
    };
}