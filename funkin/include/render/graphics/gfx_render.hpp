/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#pragma once
#include <bgfx/bgfx.h>

namespace Funkin::Render::GFX {
    class gfx_render {
    public:
        explicit gfx_render(bgfx::ViewId id);

        void set_color(uint32_t rgba);
        void set_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
        void touch();

        bgfx::ViewId id() const { return m_id_; }

    private:
        bgfx::ViewId m_id_;
    };
}
