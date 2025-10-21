#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "core/AudioAnalyzer.h"
#include "core/MultiBridgeManager.h"
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
