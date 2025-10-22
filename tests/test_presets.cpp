#include <catch2/catch_test_macros.hpp>
#include "core/PresetManager.h"
#include "core/MultiBridgeManager.h"
#include "core/LampGroupManager.h"
#include "core/ColorGroupEditor.h"
#include "core/EffectEngine.h"
#include "core/AudioAnalyzer.h"
#include "core/ConfigManager.h"
#include <QCoreApplication>
#include <fstream>

// Dummy classes to satisfy dependencies for testing PresetManager
class DummyEffectEngine : public EffectEngine { public: DummyEffectEngine(AudioAnalyzer& a, LampGroupManager& l, ColorGroupEditor& c) : EffectEngine(a, l, c) {} };

TEST_CASE("PresetManager Save and Load", "[Presets]") {
    // We need a QCoreApplication for the managers that use Qt's event system
    int argc = 0;
    char* argv[] = { nullptr };
    QCoreApplication app(argc, argv);

    // Create a dummy config for the audio analyzer
    ConfigManager dummyConfig("dummy_config.json");

    LampGroupManager lampManager;
    ColorGroupEditor colorEditor;
    AudioAnalyzer audioAnalyzer(dummyConfig);
    DummyEffectEngine effectEngine(audioAnalyzer, lampManager, colorEditor);
    MultiBridgeManager bridgeManager(lampManager);

    // Create a dummy bridge and authenticate it
    bridgeManager.addManualBridge("127.0.0.1", "test-api-key", "test-client-key");

    // Create some data to save
    colorEditor.createPalette("TestPalette");
    colorEditor.addColorToPalette("TestPalette", Qt::red);
    lampManager.createGroup("TestGroup");
    lampManager.addLamp({1, "Lamp1", 0,0,0, "127.0.0.1"});
    lampManager.addLampToGroup("TestGroup", 1);

    PresetManager presetManager("test_presets.json", bridgeManager, lampManager, colorEditor, effectEngine);

    SECTION("Save Preset creates correct JSON structure") {
        REQUIRE(presetManager.savePreset("MyTestPreset") == true);

        // Re-read the file to check its contents
        std::ifstream f("test_presets.json");
        json data = json::parse(f);

        REQUIRE(data.contains("MyTestPreset"));
        const auto& preset = data["MyTestPreset"];
        REQUIRE(preset["bridges"][0]["ip"] == "127.0.0.1");
        REQUIRE(preset["color_palettes"].contains("TestPalette"));
        REQUIRE(preset["logical_groups"]["TestGroup"][0] == 1);
        REQUIRE(data["last_preset_name"] == "MyTestPreset");
    }

    SECTION("Load Preset correctly configures managers") {
        REQUIRE(presetManager.savePreset("LoadTest") == true);

        // Clear the state of the managers
        colorEditor.clearPalettes();
        lampManager.clearGroups();

        REQUIRE(presetManager.loadPreset("LoadTest") == true);

        REQUIRE(!colorEditor.getAllPalettes().isEmpty());
        REQUIRE(!lampManager.getAllLogicalGroups().isEmpty());
    }
}
