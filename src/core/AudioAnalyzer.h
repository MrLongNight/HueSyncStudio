#pragma once

#include <RtAudio.h>
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

class AudioAnalyzer {
public:
    AudioAnalyzer(ConfigManager& config);
    ~AudioAnalyzer();

    bool startStream();
    void stopStream();

    AudioBand getFrequencyBands() const;

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

    // Band frequencies loaded from config
    double m_lowBandEnd;
    double m_midBandEnd;
    double m_highBandEnd;
};
