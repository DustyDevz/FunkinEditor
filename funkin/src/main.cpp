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