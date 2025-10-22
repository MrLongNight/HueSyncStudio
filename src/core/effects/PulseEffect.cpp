#include "PulseEffect.h"
#include <algorithm>

PulseEffect::PulseEffect(const QColor& color) : m_color(color) {}

LightStateMap PulseEffect::update(double deltaTime,
                                   const QMap<int, Lamp>& lamps,
                                   const AudioBand& audioBand) {
    LightStateMap state;

    // Use the energy of the low frequency band to control brightness
    // Normalize and scale the audio input to a brightness value
    double brightness = std::min(1.0, audioBand.low * 2.0); // Simple scaling

    QColor c = m_color;
    c.setHsvF(c.hueF(), c.saturationF(), brightness);

    for (const auto& lamp : lamps) {
        state[lamp.id] = c;
    }

    return state;
}
