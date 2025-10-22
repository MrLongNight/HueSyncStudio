#include "AudioDataModel.h"

AudioDataModel::AudioDataModel(QObject *parent)
    : QObject(parent)
{
}

double AudioDataModel::lowBandEnergy() const
{
    return m_lowBandEnergy;
}

void AudioDataModel::setLowBandEnergy(double value)
{
    if (qFuzzyCompare(m_lowBandEnergy, value))
        return;

    m_lowBandEnergy = value;
    emit lowBandEnergyChanged();
}

double AudioDataModel::midBandEnergy() const
{
    return m_midBandEnergy;
}

void AudioDataModel::setMidBandEnergy(double value)
{
    if (qFuzzyCompare(m_midBandEnergy, value))
        return;

    m_midBandEnergy = value;
    emit midBandEnergyChanged();
}

double AudioDataModel::highBandEnergy() const
{
    return m_highBandEnergy;
}

void AudioDataModel::setHighBandEnergy(double value)
{
    if (qFuzzyCompare(m_highBandEnergy, value))
        return;

    m_highBandEnergy = value;
    emit highBandEnergyChanged();
}
