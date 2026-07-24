/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#pragma once
#include <QGuiApplication>
#include <QWindow>
#include <chrono>

#include "render/graphics/graphics_context.hpp"
#include "render/graphics/graphics_device.hpp"

namespace Funkin {
    class MainWindow : public QWindow {
    public:
        MainWindow() = default;

    protected:
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;
    };

    class App {
    public:
        App(int argc, char* argv[]);
        ~App();

        bool init();
        int run();

    private:
        void update(double dt);
        void render();

        QGuiApplication m_qtApp;
        MainWindow m_window;
        Render::Graphics::graphics_context m_graphicsContext;

        bool m_running = false;
        int m_frameCount = 0;
        std::chrono::steady_clock::time_point m_lastFpsTime;
    };
}