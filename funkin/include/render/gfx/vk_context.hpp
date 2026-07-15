/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
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