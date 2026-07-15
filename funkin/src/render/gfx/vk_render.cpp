/*
* FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#include "render/gfx/vk_render.hpp"

namespace Funkin::Render::GFX {
    vk_render::vk_render(bgfx::ViewId id) : m_id_(id) {}

    void vk_render::set_color(uint32_t rgba) {
        bgfx::setViewClear(m_id_, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, rgba, 1.f, 0);
    }

    void vk_render::set_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
        bgfx::setViewRect(m_id_, x, y, w, h);
    }

    void vk_render::touch() {
        bgfx::touch(m_id_);
    }
}
