#include <catch2/catch_test_macros.hpp>
#include "core/LampGroupManager.h"
#include "core/Lamp.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

TEST_CASE("LampGroupManager group management", "[LampManager]") {
    LampGroupManager manager;
    Lamp lamp1 = {1, "Lamp 1", 0.5, 0.5, 0, "bridge1"};
    Lamp lamp2 = {2, "Lamp 2", -0.5, -0.5, 0, "bridge1"};

    manager.addLamp(lamp1);
    manager.addLamp(lamp2);

    SECTION("Default 'All' group contains all lamps") {
        const auto& allGroup = manager.getGroup("All");
        REQUIRE(allGroup.size() == 2);
        REQUIRE(allGroup.contains(1));
        REQUIRE(allGroup.contains(2));
    }

    SECTION("Creating a custom group and adding lamps") {
        manager.createGroup("Custom");
        manager.addLampToGroup("Custom", 1);

        const auto& customGroup = manager.getGroup("Custom");
        REQUIRE(customGroup.size() == 1);
        REQUIRE(customGroup.contains(1));
        REQUIRE(!customGroup.contains(2));
    }
}

TEST_CASE("Parsing Entertainment Group JSON with correct structure", "[LampManager]") {
    // This test simulates the JSON parsing part of HueBridge::onFetchGroupsReply
    // with the corrected, realistic JSON structure.
    QString mockResponse = R"({
        "1": {
            "name": "My Entertainment",
            "lights": [ "1", "2" ],
            "type": "Entertainment",
            "locations": {
                "1": [ 0.5, 0.2, -0.8 ],
                "2": [ -0.5, -0.2, 0.8 ]
            }
        },
        "2": { "name": "Living Room", "type": "Room" }
    })";

    QList<Lamp> parsedLamps;

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(mockResponse.toUtf8());
    const QJsonObject groupsObj = jsonDoc.object();

    for (auto it = groupsObj.begin(); it != groupsObj.end(); ++it) {
        const QJsonObject group = it.value().toObject();
        if (group["type"].toString() == "Entertainment") {
            const QJsonObject locations = group["locations"].toObject();
            const QJsonArray lightIds = group["lights"].toArray();

            for (const auto& lightIdValue : lightIds) {
                QString lightIdStr = lightIdValue.toString();
                Lamp lamp;
                lamp.id = lightIdStr.toInt();

                // This logic should now correctly parse the object
                if (locations.contains(lightIdStr)) {
                    QJsonArray pos = locations[lightIdStr].toArray();
                    lamp.x = pos[0].toDouble();
                    lamp.y = pos[1].toDouble();
                    lamp.z = pos[2].toDouble();
                }
                parsedLamps.append(lamp);
            }
        }
    }

    REQUIRE(parsedLamps.size() == 2);
    REQUIRE(parsedLamps[0].id == 1);
    REQUIRE(parsedLamps[0].x == 0.5);
    REQUIRE(parsedLamps[0].y == 0.2);
    REQUIRE(parsedLamps[0].z == -0.8);
    REQUIRE(parsedLamps[1].id == 2);
    REQUIRE(parsedLamps[1].x == -0.5);
}
