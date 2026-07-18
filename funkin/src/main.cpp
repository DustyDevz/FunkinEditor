/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include <QGuiApplication>

#include "render/graphics/gfx_device.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    bool ok = Funkin::Render::GFX::gfx_device::instance().init(
        #ifdef FUNKIN_DEBUG
            true
        #else
            false
        #endif
    );
    LOG_PRINT("device init result: {}", ok);


    return app.exec();
}