#pragma once
#include "Effect.h"

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
