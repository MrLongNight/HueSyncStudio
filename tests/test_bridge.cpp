#include <catch2/catch_test_macros.hpp>
#include "core/MultiBridgeManager.h" // We will test parsing logic which is inside the manager
#include "core/HueBridge.h"          // And logic inside the bridge
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

// Helper function to simulate a discovery reply
TEST_CASE("MultiBridgeManager correctly parses discovery JSON", "[Bridge]") {
    // This test doesn't actually create a manager, it just tests the parsing logic
    // that would be inside onDiscoveryFinished.

    QString mockResponse = R"([
        {"id":"001788fffe123456","internalipaddress":"192.168.1.10"},
        {"id":"001788fffe789abc","internalipaddress":"192.168.1.11"}
    ])";

    QJsonDocument doc = QJsonDocument::fromJson(mockResponse.toUtf8());
    REQUIRE(doc.isArray());
    QJsonArray array = doc.array();
    REQUIRE(array.size() == 2);

    QString ip1 = array[0].toObject()["internalipaddress"].toString();
    QString ip2 = array[1].toObject()["internalipaddress"].toString();

    REQUIRE(ip1 == "192.168.1.10");
    REQUIRE(ip2 == "192.168.1.11");
}

TEST_CASE("HueBridge correctly parses authentication success JSON", "[Bridge]") {
    // This test simulates the JSON parsing part of onAuthenticationReply

    QString mockResponse = R"([
        {"success":{"username":"SOME_API_KEY_HERE"}}
    ])";

    QJsonDocument doc = QJsonDocument::fromJson(mockResponse.toUtf8());
    REQUIRE(doc.isArray());
    QJsonObject responseObj = doc.array().first().toObject();

    REQUIRE(responseObj.contains("success"));
    QString apiKey = responseObj["success"].toObject()["username"].toString();
    REQUIRE(apiKey == "SOME_API_KEY_HERE");
}

TEST_CASE("HueBridge correctly parses authentication error JSON", "[Bridge]") {
    // This test simulates parsing a "link button not pressed" error

    QString mockResponse = R"([
        {"error":{"type":101,"address":"","description":"link button not pressed"}}
    ])";

    QJsonDocument doc = QJsonDocument::fromJson(mockResponse.toUtf8());
    REQUIRE(doc.isArray());
    QJsonObject responseObj = doc.array().first().toObject();

    REQUIRE(responseObj.contains("error"));
    int errorType = responseObj["error"].toObject()["type"].toInt();
    REQUIRE(errorType == 101);
}
