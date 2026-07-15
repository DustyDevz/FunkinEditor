/*
 * FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#pragma once
#include <cstdint>

namespace Funkin::Render::GFX {
    class vk_context {
    public:
        static vk_context& instance();
        bool init(void* native_window_handle, uint16_t w, uint16_t h);
        void shutdown();
        void frame();
        void reset(uint16_t w, uint16_t h);

        bool is_initialized() const { return m_initialized_; }

    private:
        vk_context() = default;
        bool m_initialized_ = false;
    };
}