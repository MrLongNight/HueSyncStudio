#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "core/AudioAnalyzer.h"
#include "ui/AudioDataModel.h"
#include "ui/LampModel.h" // Include the new model

int main(int argc, char *argv[]) {
    Logger::init();
    Logger::get()->info("Starting HueSyncStudio GUI...");

    qRegisterMetaType<Lamp>(); // Register Lamp struct for QVariant

    std::string configPath = "resources/default_config.json";
    ConfigManager config(configPath);
    if (!config.load()) {
        Logger::get()->error("Failed to load config file: {}", configPath);
    }

    QGuiApplication app(argc, argv);

    // --- C++ / QML Integration ---
    AudioAnalyzer analyzer(config, &app);
    AudioDataModel audioDataModel(&app);
    LampModel lampModel(&app); // Create the lamp model

    // Add some mock data for TG12
    lampModel.addLamp({1, "Lightstrip", 50.0, 50.0, 0.0});
    lampModel.addLamp({2, "Play Bar 1", 150.0, 100.0, 0.0});
    lampModel.addLamp({3, "Play Bar 2", 250.0, 150.0, 0.0});

    QObject::connect(&analyzer, &AudioAnalyzer::bandsUpdated,
                     &audioDataModel, [&](const AudioBand& bands) {
        audioDataModel.setLowBandEnergy(bands.low);
        audioDataModel.setMidBandEnergy(bands.mid);
        audioDataModel.setHighBandEnergy(bands.high);
    });

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("audioDataModel", &audioDataModel);
    engine.rootContext()->setContextProperty("lampModel", &lampModel); // Expose lamp model

    const QUrl url(QStringLiteral("qrc:/HueSyncStudio.ui/AppWindow.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            Logger::get()->critical("Failed to load QML. Exiting.");
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    if (!analyzer.startStream()) {
        Logger::get()->error("Failed to start audio stream.");
    }

    int execResult = app.exec();
    analyzer.stopStream();
    return execResult;
}
