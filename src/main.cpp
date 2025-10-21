#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "core/AudioAnalyzer.h"
#include "ui/AudioDataModel.h"

int main(int argc, char *argv[]) {
    Logger::init();
    Logger::get()->info("Starting HueSyncStudio GUI...");

    std::string configPath = "resources/default_config.json";
    ConfigManager config(configPath);
    if (!config.load()) {
        Logger::get()->error("Failed to load config file: {}", configPath);
    }

    QGuiApplication app(argc, argv);

    // --- C++ / QML Integration ---
    AudioAnalyzer analyzer(config, &app);
    AudioDataModel audioDataModel(&app);

    // Connect the analyzer's signal to the model's setters
    QObject::connect(&analyzer, &AudioAnalyzer::bandsUpdated,
                     &audioDataModel, [&](const AudioBand& bands) {
        audioDataModel.setLowBandEnergy(bands.low);
        audioDataModel.setMidBandEnergy(bands.mid);
        audioDataModel.setHighBandEnergy(bands.high);
    });

    QQmlApplicationEngine engine;

    // Expose the C++ model to QML
    engine.rootContext()->setContextProperty("audioDataModel", &audioDataModel);

    const QUrl url(QStringLiteral("qrc:/HueSyncStudio.ui/AppWindow.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            Logger::get()->critical("Failed to load QML. Exiting.");
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    // --- Start Application Logic ---
    if (!analyzer.startStream()) {
        Logger::get()->error("Failed to start audio stream.");
    }

    int execResult = app.exec();

    // Cleanly stop the audio stream when the application quits
    analyzer.stopStream();

    return execResult;
}
