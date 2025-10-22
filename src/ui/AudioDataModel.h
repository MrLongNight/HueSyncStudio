#pragma once

#include <QObject>
#include <QColor>

/**
 * @class AudioDataModel
 * @brief Provides real-time audio analysis data to the QML frontend.
 *
 * This class acts as a bridge between the C++ `AudioAnalyzer` and the QML user interface.
 * It exposes the calculated energy for each frequency band as Q_PROPERTY members,
 * making them available for data binding in QML.
 */
class AudioDataModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double lowBandEnergy READ lowBandEnergy WRITE setLowBandEnergy NOTIFY lowBandEnergyChanged)
    Q_PROPERTY(double midBandEnergy READ midBandEnergy WRITE setMidBandEnergy NOTIFY midBandEnergyChanged)
    Q_PROPERTY(double highBandEnergy READ highBandEnergy WRITE setHighBandEnergy NOTIFY highBandEnergyChanged)

public:
    explicit AudioDataModel(QObject *parent = nullptr);

    double lowBandEnergy() const;
    void setLowBandEnergy(double value);

    double midBandEnergy() const;
    void setMidBandEnergy(double value);

    double highBandEnergy() const;
    void setHighBandEnergy(double value);

signals:
    void lowBandEnergyChanged();
    void midBandEnergyChanged();
    void highBandEnergyChanged();

private:
    double m_lowBandEnergy = 0.0;
    double m_midBandEnergy = 0.0;
    double m_highBandEnergy = 0.0;
};
