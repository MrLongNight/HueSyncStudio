#pragma once
#include "Effect.h"
#include <QColor>

/**
 * @class SpectrumEffect
 * @brief Maps frequency bands to different colors across lamps.
 *
 * Low frequencies = blue, mid = green, high = red.
 */
class SpectrumEffect : public Effect {
public:
    SpectrumEffect();

    LightStateMap update(double deltaTime,
                         const QMap<int, Lamp>& lamps,
                         const AudioBand& audioBand) override;

    Mode getMode() const override { return Mode::DTLS; }
};

