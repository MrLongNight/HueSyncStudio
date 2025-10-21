find_package(Catch2 REQUIRED)

add_executable(run_tests
    test_config.cpp
)

# Define the path to the project source directory for the test executable
target_compile_definitions(run_tests PRIVATE "PROJECT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")

target_link_libraries(run_tests PRIVATE
    Catch2::Catch2WithMain
    ${CMAKE_SOURCE_DIR}/src/core/ConfigManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Logger.cpp
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

target_include_directories(run_tests PUBLIC
    "${CMAKE_SOURCE_DIR}/src"
)

add_test(NAME run_tests COMMAND run_tests)
