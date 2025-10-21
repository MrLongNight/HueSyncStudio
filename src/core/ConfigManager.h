#pragma once

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

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
