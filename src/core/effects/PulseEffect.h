#pragma once
#include "Effect.h"
#include <QColor>

/**
 * @class PulseEffect
 * @brief Creates a pulsing effect based on audio energy.
 *
 * All lamps pulse with a single color, with intensity
 * driven by the audio input.
 */
class PulseEffect : public Effect {
public:
    PulseEffect(const QColor& color);

    LightStateMap update(double deltaTime,
                         const QMap<int, Lamp>& lamps,
                         const AudioBand& audioBand) override;

    Mode getMode() const override { return Mode::DTLS; }

private:
    QColor m_color;
};

