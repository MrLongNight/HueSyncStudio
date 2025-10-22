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
    Logger::init();

int main(int argc, char *argv[]) {
#include <QUrl>
#include "core/Logger.h"
#include "core/ConfigManager.h"
// #include "core/AudioAnalyzer.h" // Will be integrated later

int main(int argc, char *argv[]) {
    Logger::init();
    Logger::get()->info("Starting HueSyncStudio GUI...");
#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "core/AudioAnalyzer.h"
#include "core/MultiBridgeManager.h"
#include "core/LampGroupManager.h"
#include "core/ColorGroupEditor.h"
#include "core/EffectEngine.h"
#include "core/PresetManager.h"
#include "core/effects/SpectrumEffect.h"
#include "core/HueBridge.h"
#include "core/WebSocketServer.h"
#include <QCoreApplication>
#include <QTimer>
#include <memory>

// Global flag to ensure engine is started only once
bool isEngineStarted = false;

void startEngineForBridge(HueBridge* bridge, AudioAnalyzer& analyzer, EffectEngine& engine) {
    if (!bridge) return;

    QObject::connect(bridge, &HueBridge::entertainmentGroupsFound, [bridge, &analyzer, &engine]() {
        const auto& groups = bridge->getEntertainmentGroups();
        if (!groups.isEmpty()) {
            QString groupId = groups.firstKey();
            Logger::get()->info("Starting stream on group ID {} for bridge {}", groupId.toStdString(), bridge->getId().toStdString());

            if (!isEngineStarted) {
                analyzer.startStream();
                engine.start();
                isEngineStarted = true;
            }
            bridge->startStreaming(groupId);
        } else {
            Logger::get()->warn("No entertainment groups found on bridge {}.", bridge->getId().toStdString());
        }
    });

    if (bridge->getApiKey().isEmpty()) {
        bridge->authenticate();
    } else {
        QMetaObject::invokeMethod(bridge, "fetchEntertainmentGroups", Qt::QueuedConnection);
    }
}


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Logger::init();
    Logger::get()->info("Starting HueSyncStudio GUI...");

    ConfigManager config("resources/default_config.json");
    if (!config.load()) {
        Logger::get()->error("Failed to load default config file.");
        return -1;
    }

#include "core/effects/SpectrumEffect.h"
#include "core/HueBridge.h"
#include <QCoreApplication>
#include <QTimer>
#include <memory>
#include <QCoreApplication>
#include <QTimer>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

int main(int argc, char *argv[]) {
    Logger::init();
    Logger::get()->info("Starting HueSyncStudio GUI...");
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
        // Continue to show the UI even if config fails
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

    QQmlApplicationEngine engine;
    // The path must match the URI and filename from qt_add_qml_module
    const QUrl url(QStringLiteral("qrc:/HueSyncStudio.ui/AppWindow.qml"));
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

    engine.rootContext()->setContextProperty("audioDataModel", &audioDataModel);
    engine.rootContext()->setContextProperty("lampModel", &lampModel); // Expose lamp model

    const QUrl url(QStringLiteral("qrc:/HueSyncStudio.ui/AppWindow.qml"));
    // Expose the C++ model to QML
    engine.rootContext()->setContextProperty("audioDataModel", &audioDataModel);

    const QUrl url(QStringLiteral("qrc:/HueSyncStudio.ui/AppWindow.qml"));
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

    if (!analyzer.startStream()) {
        Logger::get()->error("Failed to start audio stream.");
    }

    int execResult = app.exec();
    analyzer.stopStream();
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

    // --- Start Application Logic ---
    if (!analyzer.startStream()) {
        Logger::get()->error("Failed to start audio stream.");
    }

    int execResult = app.exec();

    // Cleanly stop the audio stream when the application quits
    analyzer.stopStream();

    return app.exec();
    AudioAnalyzer analyzer(config);
    LampGroupManager lampManager;
    ColorGroupEditor colorEditor;
    MultiBridgeManager bridgeManager(lampManager);
    EffectEngine engine(analyzer, lampManager, colorEditor);

    PresetManager presetManager("presets.json", bridgeManager, lampManager, colorEditor, engine);
    WebSocketServer server(9090, engine, lampManager, colorEditor);

    std::unique_ptr<Effect> activeEffect = std::make_unique<SpectrumEffect>();
    engine.setActiveEffect(activeEffect.get());

    if (!presetManager.loadLastPreset()) {
        QObject::connect(&bridgeManager, &MultiBridgeManager::discoveryFinished, [&]() {
            if (!bridgeManager.getBridges().isEmpty()) {
                startEngineForBridge(bridgeManager.getBridges().first(), analyzer, engine);
            } else {
                 Logger::get()->warn("No bridges found on the network.");
            }
        });
        bridgeManager.discoverBridges();
    } else {
        Logger::get()->info("Setting up {} bridge(s) from preset...", bridgeManager.getBridges().size());
        for (HueBridge* bridge : bridgeManager.getBridges()) {
            startEngineForBridge(bridge, analyzer, engine);
        }
    }

    return app.exec();
    // Use a unique_ptr to manage the effect's lifetime
    std::unique_ptr<Effect> activeEffect = std::make_unique<SpectrumEffect>();
    engine.setActiveEffect(activeEffect.get());

    QObject::connect(&bridgeManager, &MultiBridgeManager::discoveryFinished, [&]() {
        if (!bridgeManager.getBridges().isEmpty()) {
            HueBridge* bridge = bridgeManager.getBridges().first();

            // Once the bridge is authenticated and has found entertainment groups...
            QObject::connect(bridge, &HueBridge::entertainmentGroupsFound, [bridge, &analyzer, &engine]() {
                const auto& groups = bridge->getEntertainmentGroups();
                if (!groups.isEmpty()) {
                    QString groupId = groups.firstKey(); // Get the ID of the first found group
                    Logger::get()->info("Found entertainment group. Starting stream on group ID {}.", groupId.toStdString());

                    analyzer.startStream();
                    engine.start();
                    bridge->startStreaming(groupId);
                } else {
                    Logger::get()->warn("No entertainment groups found on bridge {}.", bridge->getId().toStdString());
                }
            });

            // Start the authentication process
            bridge->authenticate();
        } else {
            Logger::get()->warn("No bridges found on the network.");
        }
    });

    bridgeManager.discoverBridges();

    return app.exec();
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
