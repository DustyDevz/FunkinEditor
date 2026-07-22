/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include <QGuiApplication>
#include <QWindow>
#include <chrono>

#include "render/graphics/graphics_context.hpp"
#include "render/graphics/graphics_device.hpp"
#include "utils/log.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    auto& graphics_device = Funkin::Render::Graphics::graphics_device::instance();
    if (!graphics_device.init(false)) {
        LOG_CRIT("device init FAILED");
        return -1;
    }

    Funkin::Render::Graphics::graphics_context graphics_context;
    if (!graphics_context.init(1280, 720)) {
        LOG_CRIT("context init FAILED");
        graphics_device.shutdown();
        return -1;
    }

    QWindow window;
    window.setTitle("test, hi");
    window.resize(1280, 720);
    window.show();

    HWND hwnd = reinterpret_cast<HWND>(window.winId());
    if (!graphics_device.create_swap_chain(hwnd, 1280, 720)) {
        LOG_CRIT("swap_chain create FAILED");
        return -1;
    }

    int frame = 0;
    auto last = std::chrono::steady_clock::now();

    bool running = true;
    QObject::connect(&window, &QObject::destroyed, &app, [&running]() { running = false; });

    while (running && !window.isVisible() == false) {
        app.processEvents(QEventLoop::AllEvents);
        if (!window.isVisible()) break;

        graphics_context.begin_frame(0.2f, 0.4f, 0.8f, 1.0f);
        graphics_context.end_frame();
        graphics_context.present();

        frame++;
        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - last).count();
        if (elapsed >= 1.0) {
            QString title = QString("hi | FPS: %1").arg(frame / elapsed, 0, 'f', 1);
            window.setTitle(title);
            frame = 0;
            last = now;
        }
    }

    graphics_context.shutdown();
    graphics_device.shutdown();
    return 0;
}