/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include "app/app.hpp"
#include "utils/log.hpp"

namespace Funkin {
    void MainWindow::keyPressEvent(QKeyEvent* event) {
        Input::Input::instance().handleQtKeyEvent(event, true);
        QWindow::keyPressEvent(event);
    }

    void MainWindow::keyReleaseEvent(QKeyEvent* event) {
        Input::Input::instance().handleQtKeyEvent(event, false);
        QWindow::keyReleaseEvent(event);
    }

    void MainWindow::mousePressEvent(QMouseEvent* event) {
        Input::Input::instance().handleQtMouseEvent(event, true);
        QWindow::mousePressEvent(event);
    }

    void MainWindow::mouseReleaseEvent(QMouseEvent* event) {
        Input::Input::instance().handleQtMouseEvent(event, false);
        QWindow::mouseReleaseEvent(event);
    }

    void MainWindow::mouseMoveEvent(QMouseEvent* event) {
        Input::Input::instance().handleQtMouseMoveEvent(event);
        QWindow::mouseMoveEvent(event);
    }

    void MainWindow::wheelEvent(QWheelEvent* event) {
        Input::Input::instance().handleQtWheelEvent(event);
        QWindow::wheelEvent(event);
    }

    App::App(int argc, char* argv[])
        : m_qtApp(argc, argv) {}

    App::~App() {
        Input::Input::instance().shutdown();

        auto* context = Render::Graphics::graphics_device::instance().context();
        if (context) {
            context->Flush();
            context->FinishFrame();
        }

        m_graphicsContext.shutdown();
        Render::Graphics::graphics_device::instance().shutdown();
    }

    bool App::init() {
        auto& device = Render::Graphics::graphics_device::instance();
        if (!device.init(false)) {
            LOG_CRIT("device init FAILED");
            return false;
        }

        if (!m_graphicsContext.init(1280, 720)) {
            LOG_CRIT("context init FAILED");
            return false;
        }

        m_window.setTitle("hi :3");
        m_window.resize(1280, 720);
        m_window.show();
        m_window.requestActivate();

        HWND hwnd = reinterpret_cast<HWND>(m_window.winId());
        if (!device.create_swap_chain(hwnd, 1280, 720)) {
            LOG_CRIT("swap_chain create FAILED");
            return false;
        }

        auto& input = Input::Input::instance();
        input.init();
        input.setWindow(&m_window);

        QObject::connect(&m_window, &QWindow::visibleChanged, &m_qtApp, [this](bool visible) {
            if (!visible) {
                m_running = false;
            }
        });

        QObject::connect(&m_window, &QObject::destroyed, &m_qtApp, [this]() {
            m_running = false;
        });

        m_lastFpsTime = std::chrono::steady_clock::now();
        m_running = true;
        return true;
    }

    int App::run() {
        auto lastFrameTime = std::chrono::steady_clock::now();

        while (m_running && m_window.isVisible()) {
            m_qtApp.processEvents(QEventLoop::AllEvents);

            if (!m_running || !m_window.isVisible()) {
                break;
            }

            auto now = std::chrono::steady_clock::now();
            double dt = std::chrono::duration<double>(now - lastFrameTime).count();
            lastFrameTime = now;

            update(dt);
            render();

            m_frameCount++;
            double elapsed = std::chrono::duration<double>(now - m_lastFpsTime).count();
            if (elapsed >= 1.0) {
                QString title = QString("hi :3 | FPS: %1").arg(m_frameCount / elapsed, 0, 'f', 1);
                m_window.setTitle(title);
                m_frameCount = 0;
                m_lastFpsTime = now;
            }
        }

        auto* context = Render::Graphics::graphics_device::instance().context();
        if (context) {
            context->Flush();
        }

        return 0;
    }

    void App::update(double dt) {
        auto& input = Input::Input::instance();
        input.update();

        if (input.justDown(Input::KeyCode::G)) {
            LOG_PRINT("G pressed! :3");
        }
    }

    void App::render() {
        if (!m_running || !m_window.isVisible()) {
            return;
        }

        m_graphicsContext.begin_frame(0.2f, 0.4f, 0.8f, 1.0f);
        m_graphicsContext.end_frame();
        m_graphicsContext.present();
    }
}