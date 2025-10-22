#include "SpectrumEffect.h"
#include <algorithm>

SpectrumEffect::SpectrumEffect() {}

LightStateMap SpectrumEffect::update(double deltaTime,
                                     const QMap<int, Lamp>& lamps,
                                     const AudioBand& audioBand) {
    LightStateMap state;

    for (const auto& lamp : lamps) {
        // Normalize lamp's x-position from [-1, 1] to [0, 1]
        double x_norm = (lamp.x + 1.0) / 2.0;
        x_norm = std::max(0.0, std::min(1.0, x_norm));

        double brightness;
        QColor color;

        // Map position to frequency band and color
        if (x_norm < 0.33) {
            brightness = std::min(1.0, audioBand.low * 2.0);
            color = QColor::fromHsvF(0.0, 1.0, brightness); // Red for bass
        } else if (x_norm < 0.66) {
            brightness = std::min(1.0, audioBand.mid * 2.0);
            color = QColor::fromHsvF(0.33, 1.0, brightness); // Green for mid
        } else {
            brightness = std::min(1.0, audioBand.high * 2.0);
            color = QColor::fromHsvF(0.66, 1.0, brightness); // Blue for high
        }

        state[lamp.id] = color;
    }

    return state;
}
