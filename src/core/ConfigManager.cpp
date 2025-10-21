#include "ConfigManager.h"
#include "Logger.h"
#include <fstream>

ConfigManager::ConfigManager(const std::string& filepath) : m_filepath(filepath) {}

bool ConfigManager::load() {
    std::ifstream file(m_filepath);
    if (!file.is_open()) {
        Logger::get()->error("Could not open config file: {}", m_filepath);
        return false;
    }

    try {
        m_config = json::parse(file);
        Logger::get()->info("Config file loaded: {}", m_filepath);
    } catch (json::parse_error& e) {
        Logger::get()->error("Failed to parse config file: {}. Error: {}", m_filepath, e.what());
        return false;
    }

    return true;
}

bool ConfigManager::save() {
    std::ofstream file(m_filepath);
    if (!file.is_open()) {
        Logger::get()->error("Could not open config file for writing: {}", m_filepath);
        return false;
    }
    file << m_config.dump(4);
    Logger::get()->info("Config file saved: {}", m_filepath);
    return true;
}

json& ConfigManager::get() {
    return m_config;
}
