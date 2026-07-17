/*
 FunkinC++ Engine
 Copyright (©) 2026 Dusty. All rights reserved.

 This program is licensed under the GNU Affero General Public License v3.0 and is distributed WITHOUT ANY WARRANTY.
 For a copy, see <https://www.gnu.org/licenses/agpl-3.0.html>.
*/

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QAbstractNativeEventFilter>

#include "render/qt/vk_viewport.hpp"
#include "app/debug.hpp"

class resize_filter : public QAbstractNativeEventFilter {
public:
    Funkin::Render::QT::vk_viewport* target = nullptr;

    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *) override {
        if (eventType != "windows_generic_MSG") return false;
        auto* msg = static_cast<MSG*>(message);
        if (msg->message == WM_SIZE && target) {
            const int w = LOWORD(msg->lParam);
            const int h = HIWORD(msg->lParam);
            target->force_resize(w, h);
        }
        return false;
    }
};

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    qmlRegisterType<Funkin::Render::QT::vk_viewport>("Funkin.Render", 1, 0, "Render");

    QQmlApplicationEngine engine;
    Funkin::App::Debug* debug = new Funkin::App::Debug(&app);
    engine.rootContext()->setContextProperty("Debug", debug);

    const QUrl url(QStringLiteral("qrc:/main/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    QQuickWindow::setDefaultAlphaBuffer(true);
    engine.load(url);

    auto* filter = new resize_filter();
    if (!engine.rootObjects().isEmpty()) {
        auto* root_window = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
        if (root_window) {
            auto* vp = root_window->findChild<Funkin::Render::QT::vk_viewport*>("viewport");
            filter->target = vp;

            if (vp) {
                QObject::connect(root_window, &QQuickWindow::closing, vp, [vp](QQuickCloseEvent*) {
                    vp->shutdown();
                });
            }
        }
    }
    app.installNativeEventFilter(filter);

    return app.exec();
}