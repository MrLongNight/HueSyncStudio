#include <catch2/catch_test_macros.hpp>
#include "core/effects/PulseEffect.h"
#include "core/effects/SpectrumEffect.h"
#include "core/Lamp.h"
#include "core/AudioAnalyzer.h" // For AudioBand struct
#include <QMap>
#include <QColor>

TEST_CASE("PulseEffect functionality", "[Effects]") {
    PulseEffect effect(QColor(Qt::white));

    QMap<int, Lamp> lamps;
    lamps[1] = {1, "Lamp 1", 0, 0, 0, "b1"};
    lamps[2] = {2, "Lamp 2", 0, 0, 0, "b1"};

    SECTION("No audio input results in black") {
        AudioBand band = {0.0, 0.0, 0.0};
        LightStateMap state = effect.update(0.034, lamps, band);
        REQUIRE(state[1].value() == 0);
    }

    SECTION("High audio input results in bright color") {
        AudioBand band = {0.8, 0.2, 0.1};
        LightStateMap state = effect.update(0.034, lamps, band);
        REQUIRE(state[1].valueF() > 0.9); // Should be almost fully bright
        REQUIRE(state[1].saturationF() == QColor(Qt::white).saturationF());
    }
}

TEST_CASE("SpectrumEffect functionality", "[Effects]") {
    SpectrumEffect effect;

    QMap<int, Lamp> lamps;
    lamps[1] = {1, "Lamp Left", -0.8, 0, 0, "b1"}; // Far left
    lamps[2] = {2, "Lamp Mid", 0.0, 0, 0, "b1"};   // Middle
    lamps[3] = {3, "Lamp Right", 0.8, 0, 0, "b1"};  // Far right

    AudioBand band = {0.8, 0.7, 0.9}; // Strong signal in all bands
    LightStateMap state = effect.update(0.034, lamps, band);

    SECTION("Lamps get colors based on X position") {
        // Far left lamp should be red (bass)
        REQUIRE(state[1].hueF() < 0.1); // Hue for red is ~0.0
        REQUIRE(state[1].valueF() > 0.9);

        // Middle lamp should be green (mid)
        REQUIRE(state[2].hueF() > 0.3 && state[2].hueF() < 0.35); // Hue for green is ~0.33
        REQUIRE(state[2].valueF() > 0.9);

        // Far right lamp should be blue (high)
        REQUIRE(state[3].hueF() > 0.6 && state[3].hueF() < 0.7); // Hue for blue is ~0.66
        REQUIRE(state[3].valueF() > 0.9);
    }
}
