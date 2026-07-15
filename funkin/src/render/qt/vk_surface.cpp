/*
 * FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#include "render/qt/vk_surface.hpp"

namespace Funkin::Render::QT {
    vk_surface::vk_surface(QWindow *parent) : QWindow(parent) {}

    void *vk_surface::native_handle() const {
        return reinterpret_cast<void*>(winId());
    }
}
