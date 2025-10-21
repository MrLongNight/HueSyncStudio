#include "ColorFlowEffect.h"

ColorFlowEffect::ColorFlowEffect(const ColorGroupEditor& colorEditor, const QString& paletteName, double speed)
    : m_colorEditor(colorEditor), m_paletteName(paletteName), m_speed(speed), m_time(0.0) {}

LightStateMap ColorFlowEffect::update(double deltaTime,
                                      const QMap<int, Lamp>& lamps,
                                      const AudioBand& audioBand) {
    m_time += deltaTime * m_speed;
    if (m_time > 1.0) m_time -= 1.0;

    LightStateMap state;
    for (const auto& lamp : lamps) {
        // Normalize x-position to create a spatial offset for the flow
        double x_norm = (lamp.x + 1.0) / 2.0;

        double colorIndex = m_time + (x_norm * 0.5); // Add spatial offset
        if (colorIndex > 1.0) colorIndex -= 1.0;

        state[lamp.id] = m_colorEditor.getColorFromPalette(m_paletteName, colorIndex);
    }

    return state;
}
