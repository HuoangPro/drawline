#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QVariant>
#include "UI.h"
#include <QDebug>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    Grid grid;
    Robot robot(grid);

    std::ifstream inputFile("commands.txt");
    if (!inputFile) {
        std::cerr << "Error: Could not open file.\n";
        return 1;
    }

    std::string line;
    try {
        while (std::getline(inputFile, line)) {
            if (line.empty()) continue;
            auto command = CommandParser::parseCommand(line);
            command->execute(robot, grid);
        }
        grid.print();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    UI ui(grid);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("UI", &ui);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);


    return app.exec();
}
