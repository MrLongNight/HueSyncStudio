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
        Logger::get()->info("Exiting application.");
        app.quit();
    });

    return app.exec();
}
