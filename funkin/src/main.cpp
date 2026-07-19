/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include <QGuiApplication>
#include "render/graphics/graphics_device.hpp"
#include "utils/log.hpp"

int main(int argc, char* argv[]) {
    auto& device = Funkin::Render::GFX::graphics_device::instance();
    bool ok = device.init(false);
    LOG_PRINT("device result: {}", ok);

    if (ok) {
        device.shutdown();
    }

    return ok ? 0 : -1;

    // QGuiApplication app(argc, argv);
    // return app.exec();
}
