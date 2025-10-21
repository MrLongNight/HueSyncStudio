#include <iostream>
#include <chrono>
#include <thread>
#include "core/Logger.h"
#include "core/ConfigManager.h"
#include "core/AudioAnalyzer.h"

int main() {
    Logger::init();
    Logger::get()->info("Welcome to HueSyncStudio!");

    std::string configPath = "resources/default_config.json";
    ConfigManager config(configPath);
    if (!config.load()) {
        Logger::get()->error("Failed to load config file: {}", configPath);
        return -1;
    }

    AudioAnalyzer analyzer(config);
    if (!analyzer.startStream()) {
        Logger::get()->error("Failed to start audio stream. Exiting.");
        return -1;
    }

    Logger::get()->info("Press Enter to exit...");
    std::cin.get();

    analyzer.stopStream();

    return 0;
}
