#include <catch2/catch_test_macros.hpp>
#include "core/AudioAnalyzer.h"
#include "core/ConfigManager.h"
#include <vector>
#include <cmath>

#ifndef PROJECT_SOURCE_DIR
#define PROJECT_SOURCE_DIR ""
#endif

constexpr double PI = 3.14159265358979323846;

TEST_CASE("AudioAnalyzer processes sine wave correctly", "[AudioAnalyzer]") {
    // Setup ConfigManager for the test
    std::string configPath = std::string(PROJECT_SOURCE_DIR) + "/resources/default_config.json";
    ConfigManager config(configPath);
    REQUIRE(config.load() == true);

    AudioAnalyzer analyzer(config);

    // Generate a 100 Hz sine wave
    unsigned int sampleRate = 44100;
    unsigned int bufferSize = 1024;
    double frequency = 100.0;
    std::vector<double> testBuffer(bufferSize);

    for (unsigned int i = 0; i < bufferSize; ++i) {
        testBuffer[i] = sin(2.0 * PI * frequency * static_cast<double>(i) / sampleRate);
    }

    analyzer.processFFT(testBuffer.data());

    AudioBand bands = analyzer.getFrequencyBands();

    // With a 100 Hz sine wave, the low band should have significant energy,
    // while mid and high bands should be close to zero.
    REQUIRE(bands.low > 0.1);
    REQUIRE(bands.mid < 0.01);
    REQUIRE(bands.high < 0.01);
}
