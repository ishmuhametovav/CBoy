#pragma once
#include "miniaudio.h"
#include <atomic>

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    bool init(unsigned int sampleRate = 44100, unsigned int channels = 1);
    bool start();
    void stop();
    void shutdown();

    void setTestMode(bool enable) { m_testMode = enable; }
    bool isTestMode() const { return m_testMode; }

private:
    static void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

    ma_device m_device;
    ma_device_config m_deviceConfig;

    unsigned int m_sampleRate;
    unsigned int m_channels;
    int m_cyclesPerSample;
    std::atomic<bool> m_testMode;
    bool m_running;
};