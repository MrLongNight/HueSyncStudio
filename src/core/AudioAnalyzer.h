#pragma once

#include <QObject>
#include <rtaudio/RtAudio.h>
#include <vector>
#include <string>
#include <kiss_fft.h>
#include <memory>
#include "ConfigManager.h"

struct AudioBand {
    double low;
    double mid;
    double high;
};
Q_DECLARE_METATYPE(AudioBand);


class AudioAnalyzer : public QObject {
    Q_OBJECT

public:
    AudioAnalyzer(ConfigManager& config, QObject* parent = nullptr);
    ~AudioAnalyzer();

    bool startStream();
    void stopStream();

    AudioBand getFrequencyBands() const;

signals:
    void bandsUpdated(const AudioBand& bands);

public:
    // Public for testing
    void processFFT(const double* buffer);

private:
    static int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
                             double streamTime, RtAudioStreamStatus status, void* userData);

    ConfigManager& m_config;
    RtAudio m_audio;
    unsigned int m_bufferFrames;
    unsigned int m_sampleRate;

    kiss_fft_cfg m_fftConfig;
    std::vector<kiss_fft_cpx> m_fftOut;
    std::vector<double> m_fftMag;

    mutable AudioBand m_latestBands;

    // --- Configuration values ---
    double m_lowBandStart, m_lowBandEnd;
    double m_midBandStart, m_midBandEnd;
    double m_highBandStart, m_highBandEnd;
    double m_decayFactor;
};
