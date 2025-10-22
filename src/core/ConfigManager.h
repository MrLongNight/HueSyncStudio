#pragma once

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

/**
 * @class ConfigManager
 * @brief Manages loading and saving of the application's JSON configuration file.
 *
 * This class is responsible for reading the `default_config.json` file from disk,
 * parsing it into a `nlohmann::json` object, and providing access to the raw
 * configuration data.
 */
class ConfigManager {
public:
    ConfigManager(const std::string& filepath);
    bool load();
    bool save();
    json& get();
private:
    std::string m_filepath;
    json m_config;
};
