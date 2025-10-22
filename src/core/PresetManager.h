#pragma once

#include <QObject>
#include <QString>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class MultiBridgeManager;
class LampGroupManager;
class ColorGroupEditor;
class EffectEngine;

class PresetManager : public QObject {
    Q_OBJECT

public:
    explicit PresetManager(const QString& filePath,
                           MultiBridgeManager& bridgeManager,
                           LampGroupManager& lampManager,
                           ColorGroupEditor& colorEditor,
                           EffectEngine& effectEngine,
                           QObject* parent = nullptr);

    bool savePreset(const QString& presetName);
    bool loadPreset(const QString& presetName);
    bool loadLastPreset();

private:
    bool loadFromFile();
    bool saveToFile();

    QString m_filePath;
    json m_presets;

    // References to the managers to save/load data from/to
    MultiBridgeManager& m_bridgeManager;
    LampGroupManager& m_lampManager;
    ColorGroupEditor& m_colorEditor;
    EffectEngine& m_effectEngine;
};
