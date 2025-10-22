#pragma once
#include "Effect.h"
#include "../../core/ColorGroupEditor.h"

class ColorFlowEffect : public Effect {
public:
    ColorFlowEffect(const ColorGroupEditor& colorEditor, const QString& paletteName, double speed);

    LightStateMap update(double deltaTime,
                         const QMap<int, Lamp>& lamps,
                         const AudioBand& audioBand) override;

    Mode getMode() const override { return Mode::REST; }

private:
    const ColorGroupEditor& m_colorEditor;
    QString m_paletteName;
    double m_speed;
    double m_time;
};
