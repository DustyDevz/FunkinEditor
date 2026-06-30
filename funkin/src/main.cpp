/*
 * FunkinC++ Engine
 * Copyright (©) 2026 Dusty. All rights reserved.
 *
 * This project is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
*/

#include <iostream>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    QString qmlPath = QCoreApplication::applicationDirPath() + "/test.qml";
    engine.load(QUrl::fromLocalFile(qmlPath));

    std::cout << "Test" << std::endl;
    return app.exec();
}