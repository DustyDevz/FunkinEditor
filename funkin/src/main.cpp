/*
 * FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

#include "render/render_viewport.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);
    qmlRegisterType<Funkin::Render::RenderViewport>("Funkin.Render", 1, 0, "RenderViewport");
    QQmlApplicationEngine engine;

    const QUrl url(QStringLiteral("qrc:/main/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}