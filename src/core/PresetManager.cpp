#include "PresetManager.h"
#include "MultiBridgeManager.h"
#include "HueBridge.h"
#include "LampGroupManager.h"
#include "ColorGroupEditor.h"
#include "EffectEngine.h"
#include "JsonSerializers.h"
#include "Logger.h"
#include <fstream>

// Existing constructor and save/load file methods...
PresetManager::PresetManager(const QString& filePath,
                             MultiBridgeManager& bridgeManager,
                             LampGroupManager& lampManager,
                             ColorGroupEditor& colorEditor,
                             EffectEngine& effectEngine,
                             QObject* parent)
    : QObject(parent),
      m_filePath(filePath),
      m_bridgeManager(bridgeManager),
      m_lampManager(lampManager),
      m_colorEditor(colorEditor),
      m_effectEngine(effectEngine)
{
    loadFromFile();
}

bool PresetManager::loadFromFile() {
    std::ifstream f(m_filePath.toStdString());
    if (!f.is_open()) return false;
    try {
        m_presets = json::parse(f);
    } catch (json::parse_error& e) {
        return false;
    }
    return true;
}

bool PresetManager::saveToFile() {
    std::ofstream o(m_filePath.toStdString());
    if (!o.is_open()) return false;
    o << m_presets.dump(4) << std::endl;
    return true;
}

bool PresetManager::savePreset(const QString& presetName) {
    Logger::get()->info("Saving preset '{}'...", presetName.toStdString());
    json presetData;
    json bridgeArray = json::array();
    for (const auto& bridge : m_bridgeManager.getBridges()) {
        if (!bridge->getApiKey().isEmpty()) {
            json bridgeObj;
            bridgeObj["ip"] = bridge->getId().toStdString();
            bridgeObj["api_key"] = bridge->getApiKey().toStdString();
            bridgeObj["client_key"] = bridge->getClientKey().toStdString();
            bridgeArray.push_back(bridgeObj);
        }
    }
    presetData["bridges"] = bridgeArray;
    presetData["color_palettes"] = m_colorEditor.getAllPalettes();
    presetData["logical_groups"] = m_lampManager.getAllLogicalGroups();
    presetData["effect"] = { {"type", "Spectrum"} };
    m_presets[presetName.toStdString()] = presetData;
    m_presets["last_preset_name"] = presetName.toStdString();
    Logger::get()->info("Preset '{}' saved.", presetName.toStdString());
    return saveToFile();
}

bool PresetManager::loadPreset(const QString& presetName) {
    Logger::get()->info("Loading preset '{}'...", presetName.toStdString());
    if (!m_presets.contains(presetName.toStdString())) {
        Logger::get()->error("Preset '{}' not found.", presetName.toStdString());
        return false;
    }

    try {
        const json& presetData = m_presets.at(presetName.toStdString());

        // 1. Load Bridge configurations
        if (presetData.contains("bridges")) {
            for (const auto& bridgeObj : presetData["bridges"]) {
                m_bridgeManager.addManualBridge(
                    QString::fromStdString(bridgeObj.at("ip")),
                    QString::fromStdString(bridgeObj.at("api_key")),
                    QString::fromStdString(bridgeObj.at("client_key"))
                );
            }
        }

        // 2. Load Color Palettes
        m_colorEditor.clearPalettes();
        if (presetData.contains("color_palettes")) {
            for (auto it = presetData["color_palettes"].begin(); it != presetData["color_palettes"].end(); ++it) {
                m_colorEditor.createPalette(QString::fromStdString(it.key()));
                ColorPalette palette = it.value();
                for(const auto& color : palette) {
                    m_colorEditor.addColorToPalette(QString::fromStdString(it.key()), color);
                }
            }
        }

        // 3. Load Logical Groups
        m_lampManager.clearGroups();
        if (presetData.contains("logical_groups")) {
             for (auto it = presetData["logical_groups"].begin(); it != presetData["logical_groups"].end(); ++it) {
                m_lampManager.createGroup(QString::fromStdString(it.key()));
                LogicalGroup group = it.value();
                for(int lampId : group) {
                    m_lampManager.addLampToGroup(QString::fromStdString(it.key()), lampId);
                }
            }
        }

        // 4. Load Effect settings (placeholder)
        // m_effectEngine.setActiveEffect(...)

        Logger::get()->info("Preset '{}' loaded successfully.", presetName.toStdString());
        return true;

    } catch (const json::exception& e) {
        Logger::get()->error("Error parsing preset '{}': {}", presetName.toStdString(), e.what());
        return false;
    }
}

bool PresetManager::loadLastPreset() {
    if (m_presets.contains("last_preset_name")) {
        std::string lastName = m_presets["last_preset_name"];
        if (loadPreset(QString::fromStdString(lastName))) {
            m_presets["last_preset_name"] = lastName; // Ensure it stays set
            return true;
        }
    }
    return false;
}
