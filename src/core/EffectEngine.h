#pragma once

#include <QObject>
#include <QTimer>

class AudioAnalyzer;
class LampGroupManager;
class ColorGroupEditor;
class Effect;

class EffectEngine : public QObject {
    Q_OBJECT

public:
    explicit EffectEngine(AudioAnalyzer& audioAnalyzer,
                          LampGroupManager& lampManager,
                          ColorGroupEditor& colorEditor,
                          QObject* parent = nullptr);

    void start();
    void stop();
    void setActiveEffect(Effect* effect);
    void manualOverride(const QString& groupName, const QColor& color, int brightness);

public slots:
    void update();

private:
    AudioAnalyzer& m_audioAnalyzer;
    LampGroupManager& m_lampManager;
    ColorGroupEditor& m_colorEditor;

    Effect* m_activeEffect;
    QTimer* m_updateTimer;
};
