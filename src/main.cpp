#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "core/AudioAnalyzer.h"
#include "core/MultiBridgeManager.h"
#include "core/LampGroupManager.h"
#include <QCoreApplication>
#include <QTimer>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Logger::init();
    Logger::get()->info("Welcome to HueSyncStudio!");
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
        return -1;
    }

    LampGroupManager lampManager;
    MultiBridgeManager bridgeManager(lampManager);
    bridgeManager.discoverBridges();

    // In a real app, the UI would drive the authentication and group selection.
    // For now, let's run for 15 seconds to allow for discovery and a manual
    // link button press simulation before exiting.
    QTimer::singleShot(15000, [&]() {
    // AudioAnalyzer is not used in this TG, but we leave it for now.
    // AudioAnalyzer analyzer(config);
    // analyzer.startStream();

    MultiBridgeManager bridgeManager;
    bridgeManager.discoverBridges();

    // In a real app, we would connect to signals to proceed.
    // For now, just run for a few seconds to allow discovery to happen.
    QTimer::singleShot(5000, [&]() {
        Logger::get()->info("Exiting application.");
        app.quit();
    });

    return app.exec();
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
