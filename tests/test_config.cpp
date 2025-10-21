#include <catch2/catch_test_macros.hpp>
#include "core/ConfigManager.h"
#include "core/Logger.h"
#include <string>

// This macro is defined in CMakeLists.txt
#ifndef PROJECT_SOURCE_DIR
#define PROJECT_SOURCE_DIR ""
#endif

TEST_CASE("ConfigManager loads a valid JSON file", "[ConfigManager]") {
    Logger::init();
    std::string configPath = std::string(PROJECT_SOURCE_DIR) + "/resources/default_config.json";
    ConfigManager config(configPath);
    REQUIRE(config.load() == true);
    REQUIRE(config.get()["settings"]["appName"] == "HueSyncStudio");
}
