#pragma once

#include <QMap>
#include "../Lamp.h"

// A map of Lamp ID to its calculated color state
using LightStateMap = QMap<int, QColor>;

class Effect {
public:
    virtual ~Effect() = default;

    // Called by the EffectEngine on every update tick.
    // Should return a map of lamp IDs to their new target colors.
    virtual LightStateMap update(double deltaTime,
                                 const QMap<int, Lamp>& lamps,
                                 const AudioBand& audioBand) = 0;

    // Effects can be REST-based (slow, for ambient) or DTLS-based (fast, for audio-sync)
    enum class Mode {
        REST,
        DTLS
    };

    virtual Mode getMode() const = 0;
};
