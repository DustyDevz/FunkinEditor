/*
 * FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#pragma once
#include <bgfx/bgfx.h>

namespace Funkin::Render::GFX {
    class vk_render {
    public:
        explicit vk_render(bgfx::ViewId id);

        void set_color(uint32_t rgba);
        void set_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        void touch();

        bgfx::ViewId id() const { return m_id_; }

    private:
        bgfx::ViewId m_id_;
    };
}
