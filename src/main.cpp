#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "core/AudioAnalyzer.h"

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
    // The path must match the URI and filename from qt_add_qml_module
    const QUrl url(QStringLiteral("qrc:/HueSyncStudio.ui/AppWindow.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            Logger::get()->critical("Failed to load QML. Exiting.");
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    // Initialize and start the audio analyzer after the event loop has started
    AudioAnalyzer analyzer(config);
    if (!analyzer.startStream()) {
        Logger::get()->error("Failed to start audio stream.");
        // We don't exit here, the UI should still be usable.
    }

    // The application's main event loop is started here.
    int execResult = app.exec();

    // Stop the stream when the application is about to quit.
    analyzer.stopStream();

    return execResult;
}
