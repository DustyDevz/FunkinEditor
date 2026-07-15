/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "render/qt/vk_surface.hpp"

namespace Funkin::Render::QT {
    vk_surface::vk_surface(QWindow *parent) : QWindow(parent) {}

    void *vk_surface::native_handle() const {
        return reinterpret_cast<void*>(winId());
    }
}
