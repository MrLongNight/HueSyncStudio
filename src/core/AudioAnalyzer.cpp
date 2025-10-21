#include "AudioAnalyzer.h"
#include "Logger.h"
#include <cmath>
#include <numeric>

AudioAnalyzer::AudioAnalyzer(ConfigManager& config)
    : m_config(config),
      m_bufferFrames(1024),
      m_sampleRate(44100),
      m_fftConfig(nullptr)
{
    // Load band settings from config
    try {
        m_lowBandEnd = m_config.get()["audio"]["bands"]["low"]["end"];
        m_midBandEnd = m_config.get()["audio"]["bands"]["mid"]["end"];
        m_highBandEnd = m_config.get()["audio"]["bands"]["high"]["end"];
    } catch (const nlohmann::json::exception& e) {
        Logger::get()->error("Failed to read audio bands from config: {}. Using defaults.", e.what());
        m_lowBandEnd = 250.0;
        m_midBandEnd = 2000.0;
        m_highBandEnd = 20000.0;
    }

    if (m_audio.getDeviceCount() < 1) {
        Logger::get()->error("No audio devices found.");
        return;
    }

    m_fftConfig = kiss_fft_alloc(m_bufferFrames, 0, nullptr, nullptr);
    if (!m_fftConfig) {
        Logger::get()->error("Failed to allocate KissFFT config.");
        return;
    }

    m_fftOut.resize(m_bufferFrames);
    m_fftMag.resize(m_bufferFrames / 2 + 1);

    m_latestBands = {0.0, 0.0, 0.0};
}

AudioAnalyzer::~AudioAnalyzer() {
    if (m_audio.isStreamOpen()) {
        stopStream();
    }
    kiss_fft_free(m_fftConfig);
}

bool AudioAnalyzer::startStream() {
    RtAudio::StreamParameters parameters;
    parameters.deviceId = m_audio.getDefaultInputDevice();
    parameters.nChannels = 1;

    try {
        m_audio.openStream(NULL, &parameters, RTAUDIO_FLOAT64,
                           m_sampleRate, &m_bufferFrames, &AudioAnalyzer::audioCallback, this);
        m_audio.startStream();
        Logger::get()->info("Audio stream started. Sample Rate: {}, Buffer Size: {}", m_sampleRate, m_bufferFrames);
    } catch (RtAudioError& e) {
        Logger::get()->error("Error starting audio stream: {}", e.getMessage());
        return false;
    }
    return true;
}

void AudioAnalyzer::stopStream() {
    try {
        if (m_audio.isStreamOpen()) {
            m_audio.stopStream();
            m_audio.closeStream();
            Logger::get()->info("Audio stream stopped.");
        }
    } catch (RtAudioError& e) {
        Logger::get()->error("Error stopping audio stream: {}", e.getMessage());
    }
}

AudioBand AudioAnalyzer::getFrequencyBands() const {
    return m_latestBands;
}

int AudioAnalyzer::audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
                                 double streamTime, RtAudioStreamStatus status, void* userData) {
    if (status) {
        Logger::get()->warn("Stream overflow detected!");
    }

    AudioAnalyzer* analyzer = static_cast<AudioAnalyzer*>(userData);
    analyzer->processFFT(static_cast<double*>(inputBuffer));

    return 0;
}

void AudioAnalyzer::processFFT(const double* buffer) {
    if (!m_fftConfig) return;

    std::vector<kiss_fft_cpx> fftIn(m_bufferFrames);
    for (unsigned int i = 0; i < m_bufferFrames; ++i) {
        fftIn[i].r = buffer[i];
        fftIn[i].i = 0.0;
    }

    kiss_fft(m_fftConfig, fftIn.data(), m_fftOut.data());

    for (unsigned int i = 0; i < m_fftMag.size(); ++i) {
        double real = m_fftOut[i].r;
        double imag = m_fftOut[i].i;
        m_fftMag[i] = std::sqrt(real * real + imag * imag);
    }

    double lowSum = 0, midSum = 0, highSum = 0;
    int lowCount = 0, midCount = 0, highCount = 0;

    double freqResolution = static_cast<double>(m_sampleRate) / m_bufferFrames;

    for (unsigned int i = 1; i < m_fftMag.size(); ++i) {
        double freq = i * freqResolution;
        if (freq <= m_lowBandEnd) {
            lowSum += m_fftMag[i];
            lowCount++;
        } else if (freq <= m_midBandEnd) {
            midSum += m_fftMag[i];
            midCount++;
        } else if (freq <= m_highBandEnd) {
            highSum += m_fftMag[i];
            highCount++;
        }
    }

    m_latestBands.low = (lowCount > 0) ? (lowSum / lowCount) : 0.0;
    m_latestBands.mid = (midCount > 0) ? (midSum / midCount) : 0.0;
    m_latestBands.high = (highCount > 0) ? (highSum / highCount) : 0.0;
}
