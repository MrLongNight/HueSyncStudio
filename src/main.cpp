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
    Logger::get()->info("Welcome to HueSyncStudio!");

    ConfigManager config("resources/default_config.json");
    if (!config.load()) {
        Logger::get()->error("Failed to load default config file.");
        return -1;
    }

    AudioAnalyzer analyzer(config);
    LampGroupManager lampManager;
    ColorGroupEditor colorEditor;
    MultiBridgeManager bridgeManager(lampManager);
    EffectEngine engine(analyzer, lampManager, colorEditor);

    PresetManager presetManager("presets.json", bridgeManager, lampManager, colorEditor, engine);

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
}
