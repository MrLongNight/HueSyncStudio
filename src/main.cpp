#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "core/AudioAnalyzer.h"
#include "core/MultiBridgeManager.h"
#include "core/LampGroupManager.h"
#include "core/ColorGroupEditor.h"
#include "core/EffectEngine.h"
#include "core/effects/SpectrumEffect.h"
#include "core/HueBridge.h"
#include <QCoreApplication>
#include <QTimer>
#include <memory>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Logger::init();
    Logger::get()->info("Welcome to HueSyncStudio!");

    std::string configPath = "resources/default_config.json";
    ConfigManager config(configPath);
    if (!config.load()) {
        Logger::get()->error("Failed to load config file: {}", configPath);
        return -1;
    }

    AudioAnalyzer analyzer(config);
    LampGroupManager lampManager;
    ColorGroupEditor colorEditor;
    MultiBridgeManager bridgeManager(lampManager);
    EffectEngine engine(analyzer, lampManager, colorEditor);

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
}
