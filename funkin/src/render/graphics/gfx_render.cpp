/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "render/graphics/gfx_render.hpp"

namespace Funkin::Render::GFX {
    gfx_render::gfx_render(bgfx::ViewId id) : m_id_(id) {}

    void gfx_render::set_color(uint32_t rgba) {
        bgfx::setViewClear(m_id_, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, rgba, 1.f, 0);
    }

    void gfx_render::set_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
        bgfx::setViewRect(m_id_, x, y, w, h);
    }

    void gfx_render::touch() {
        bgfx::touch(m_id_);
    }
}
