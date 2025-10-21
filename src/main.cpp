#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include "core/Logger.h"
#include "core/ConfigManager.h"
// #include "core/AudioAnalyzer.h" // Will be integrated later

int main(int argc, char *argv[]) {
    Logger::init();
    Logger::get()->info("Starting HueSyncStudio GUI...");

    std::string configPath = "resources/default_config.json";
    ConfigManager config(configPath);
    if (!config.load()) {
        Logger::get()->error("Failed to load config file: {}", configPath);
        // Continue to show the UI even if config fails
    }

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    // The path must be relative to the resource system root
    const QUrl url(QStringLiteral("qrc:/ui/AppWindow.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            Logger::get()->critical("Failed to load QML. Exiting.");
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
