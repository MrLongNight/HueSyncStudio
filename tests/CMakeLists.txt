find_package(Catch2 REQUIRED)

add_executable(run_tests
    test_config.cpp
    test_audio.cpp
)

# Define the path to the project source directory for the test executable
target_compile_definitions(run_tests PRIVATE "PROJECT_SOURCE_DIR=\"${CMAKE_SOURCE_DIR}\"")

target_link_libraries(run_tests PRIVATE
    Catch2::Catch2WithMain
    # Link all necessary source files and libraries for testing
    ${CMAKE_SOURCE_DIR}/src/core/ConfigManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Logger.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AudioAnalyzer.cpp
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    kissfft
    rtaudio
)

target_include_directories(run_tests PUBLIC
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/libs" # For kiss_fft.h
)

add_test(NAME run_tests COMMAND run_tests)
