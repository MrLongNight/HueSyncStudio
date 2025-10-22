#include "EffectEngine.h"
#include "AudioAnalyzer.h"
#include "LampGroupManager.h"
#include "ColorGroupEditor.h"
#include "effects/Effect.h"
#include "effects/PulseEffect.h"
#include "effects/SpectrumEffect.h"
#include "effects/ColorFlowEffect.h"
#include "Logger.h"

EffectEngine::EffectEngine(AudioAnalyzer& audioAnalyzer,
                           LampGroupManager& lampManager,
                           ColorGroupEditor& colorEditor,
                           QObject* parent)
    : QObject(parent),
      m_audioAnalyzer(audioAnalyzer),
      m_lampManager(lampManager),
      m_colorEditor(colorEditor),
      m_activeEffect(nullptr)
{
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &EffectEngine::update);
}

void EffectEngine::start() {
    // Target ~30 FPS for DTLS streaming limit
    m_updateTimer->start(34); // approx 30ms interval
    Logger::get()->info("Effect engine started.");
}

void EffectEngine::stop() {
    m_updateTimer->stop();
    Logger::get()->info("Effect engine stopped.");
}

void EffectEngine::setActiveEffect(Effect* effect) {
    m_activeEffect = effect;
    if (m_activeEffect) {
        Logger::get()->info("Active effect set.");
    } else {
        Logger::get()->info("Effects cleared.");
    }
}

QString EffectEngine::getActiveEffectName() const {
    if (!m_activeEffect) return "None";

    if (dynamic_cast<PulseEffect*>(m_activeEffect)) {
        return "Pulse";
    } else if (dynamic_cast<SpectrumEffect*>(m_activeEffect)) {
        return "Spectrum";
    } else if (dynamic_cast<ColorFlowEffect*>(m_activeEffect)) {
        return "ColorFlow";
    }

    return "Unknown";
}

void EffectEngine::manualOverride(const QString& groupName, const QColor& color, int brightness) {
    Logger::get()->info("Manual override activated for group '{}'", groupName.toStdString());
    // Deactivate current effect
    setActiveEffect(nullptr);

    // Send the command directly via the lamp manager using REST
    m_lampManager.sendGroupRestCommand(groupName, true, brightness, color);
}

void EffectEngine::update() {
    if (!m_activeEffect) return;

    // For now, update the "All" group. This will be configurable later.
    const auto& allLamps = m_lampManager.getAllLamps();
    if (allLamps.isEmpty()) return;

    const AudioBand audioBand = m_audioAnalyzer.getFrequencyBands();

    // Calculate the new state based on the effect
    LightStateMap newState = m_activeEffect->update(0.034, allLamps, audioBand); // Assuming ~30fps

    // Dispatch the state to the bridges based on the effect's mode
    if (m_activeEffect->getMode() == Effect::Mode::DTLS) {
        // Group states by bridge and send one DTLS packet per bridge
        QMap<QString, LightStateMap> bridgeStates;
        for (auto it = newState.begin(); it != newState.end(); ++it) {
            const Lamp& lamp = allLamps[it.key()];
            bridgeStates[lamp.bridgeId][lamp.id] = it.value();
        }
        m_lampManager.sendGroupDtlsStream(bridgeStates);

    } else { // REST mode
        for (auto it = newState.begin(); it != newState.end(); ++it) {
            int lampId = it.key();
            const QColor& color = it.value();
            // Convert color to brightness. Simple average for now.
            int brightness = (color.red() + color.green() + color.blue()) / 3;
            m_lampManager.sendSingleRestCommand(lampId, true, brightness, color);
        }
    }
}
