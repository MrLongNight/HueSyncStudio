#pragma once
#include "Effect.h"

class SpectrumEffect : public Effect {
public:
    SpectrumEffect();

    LightStateMap update(double deltaTime,
                         const QMap<int, Lamp>& lamps,
                         const AudioBand& audioBand) override;

    Mode getMode() const override { return Mode::DTLS; }
};
