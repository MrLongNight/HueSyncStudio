#include <catch2/catch_test_macros.hpp>
#include "core/WebSocketServer.h"
#include "core/EffectEngine.h"
#include "core/LampGroupManager.h"
#include "core/ColorGroupEditor.h"
#include "core/AudioAnalyzer.h"
#include "core/ConfigManager.h"
#include <QCoreApplication>
#include <QJsonObject>

TEST_CASE("WebSocketServer Command Processing Logic", "[WebSocket]") {
    // Setup
    int argc = 0; char* argv[] = { nullptr };
    QCoreApplication app(argc, argv);
    ConfigManager dummyConfig("dummy_config.json");
    AudioAnalyzer analyzer(dummyConfig);
    LampGroupManager lampManager;
    ColorGroupEditor colorEditor;
    EffectEngine engine(analyzer, lampManager, colorEditor);
    WebSocketServer server(9091, engine, lampManager, colorEditor);

    SECTION("setActiveEffect command changes the active effect") {
        QJsonObject payload;
        payload["effect"] = "Pulse";
        payload["color"] = "#FF00FF";
        QJsonObject command;
        command["command"] = "setActiveEffect";
        command["payload"] = payload;

        server.processCommand(command, nullptr); // Pass nullptr for client, as it's not used in this command

        REQUIRE(engine.getActiveEffectName() == "Pulse");
    }

    SECTION("manualOverride command clears the active effect") {
        // First set an effect
        engine.setActiveEffect(new PulseEffect(Qt::red));
        REQUIRE(engine.getActiveEffectName() == "Pulse");

        QJsonObject payload;
        payload["group"] = "All";
        payload["color"] = "#00FFFF";
        QJsonObject command;
        command["command"] = "manualOverride";
        command["payload"] = payload;

        server.processCommand(command, nullptr);

        // Manual override should clear the effect
        REQUIRE(engine.getActiveEffectName() == "None");
    }
}
