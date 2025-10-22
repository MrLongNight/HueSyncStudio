#pragma once

#include <QMap>
#include "../Lamp.h"
#include "../AudioAnalyzer.h"

// A map of Lamp ID to its calculated color state
using LightStateMap = QMap<int, QColor>;

/**
 * @class Effect
 * @brief Abstract base class for all lighting effects.
 *
 * Effects process audio data and lamp positions to generate
 * dynamic color states for each lamp.
 */
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

