/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include <QGuiApplication>
#include <QWindow>
#include <Qtimer>

#include "render/graphics/graphics_context.hpp"
#include "render/graphics/graphics_device.hpp"
#include "utils/log.hpp"

int main(int argc, char* argv[]) {
  QGuiApplication app(argc, argv);

    auto& graphics_device = Funkin::Render::Graphics::graphics_device::instance();
    if (!graphics_device.init(true)) {
        LOG_CRIT("device init FAILED");
        return -1;
    }

    // temp
    int w = 1280;
    int h = 720;
    //

    QWindow window;
    window.setTitle("Funkin");
    window.resize(w, h);
    window.show();

    Funkin::Render::Graphics::graphics_context graphics_context;
    if (!graphics_context.init(reinterpret_cast<void*>(window.winId()), w, h)) {
        LOG_CRIT("graphics context init FAILED");
        return -1;
    }

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        graphics_context.begin_frame(.1f, .6f, .1f, 1.f);
        graphics_context.present();
    });
    timer.start(16); // 60

    int result = app.exec();
    graphics_context.shutdown();
    graphics_device.shutdown();
    return result;
}
