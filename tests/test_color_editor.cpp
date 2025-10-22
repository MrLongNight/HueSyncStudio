#include <catch2/catch_test_macros.hpp>
#include "core/ColorGroupEditor.h"
#include <QColor>

TEST_CASE("ColorGroupEditor Palette Management", "[ColorEditor]") {
    ColorGroupEditor editor;
    editor.createPalette("TestPalette");
    editor.addColorToPalette("TestPalette", QColor(255, 0, 0));
    editor.addColorToPalette("TestPalette", QColor(0, 255, 0));

    const auto& palette = editor.getPalette("TestPalette");
    REQUIRE(palette.size() == 2);
    REQUIRE(palette[0] == QColor(255, 0, 0));
}

TEST_CASE("ColorGroupEditor Color Selection", "[ColorEditor]") {
    ColorGroupEditor editor; // Uses the default palette (red, green, blue)

    SECTION("Get color at start") {
        QColor color = editor.getColorFromPalette("Default", 0.0);
        REQUIRE(color == QColor(Qt::red));
    }

    SECTION("Get color in the middle") {
        QColor color = editor.getColorFromPalette("Default", 0.5);
        // Halfway between red and green is yellow
        REQUIRE(color == QColor(0, 255, 0)); // Halfway between red and blue is green
    }

    SECTION("Get color at the end") {
        QColor color = editor.getColorFromPalette("Default", 1.0);
        REQUIRE(color == QColor(Qt::blue));
    }
}

TEST_CASE("ColorGroupEditor Color Deviation", "[ColorEditor]") {
    ColorGroupEditor editor;
    QColor baseColor(Qt::cyan);

    SECTION("Zero deviation returns same color") {
        QColor deviated = editor.getDeviatedColor(baseColor, 0.0);
        REQUIRE(deviated == baseColor);
    }

    SECTION("Positive deviation returns a different color") {
        QColor deviated = editor.getDeviatedColor(baseColor, 0.5);
        REQUIRE(deviated != baseColor);

        // Check if hue and saturation are within a plausible range
        qreal h, s, v;
        deviated.getHsvF(&h, &s, &v);
        qreal base_h, base_s, base_v;
        baseColor.getHsvF(&base_h, &base_s, &base_v);

        // Hue should be reasonably close (deviation is +/- 0.25 max)
        REQUIRE(std::abs(h - base_h) < 0.26);
    }
}
