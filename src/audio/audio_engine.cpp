#define MINIAUDIO_IMPLEMENTATION
#include "audio_engine.h"
#include "APU.h"
#include <cstdio>

AudioEngine::AudioEngine()
    : m_sampleRate(44100)
    , m_channels(1)
    , m_cyclesPerSample(0)
    , m_testMode(false)
    , m_running(false)
{
}

AudioEngine::~AudioEngine() {
    shutdown();
}

bool AudioEngine::init(unsigned int sampleRate, unsigned int channels) {
    m_sampleRate = sampleRate;
    m_channels = channels;

    m_cyclesPerSample = static_cast<int>(1048576.0 / m_sampleRate);
    if (m_cyclesPerSample < 1) m_cyclesPerSample = 1;

    m_deviceConfig = ma_device_config_init(ma_device_type_playback);
    m_deviceConfig.playback.format = ma_format_f32;
    m_deviceConfig.playback.channels = m_channels;
    m_deviceConfig.sampleRate = m_sampleRate;
    m_deviceConfig.dataCallback = dataCallback;
    m_deviceConfig.pUserData = this;

    if (ma_device_init(nullptr, &m_deviceConfig, &m_device) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialize audio device.\n");
        return false;
    }

    return true;
}

bool AudioEngine::start() {
    if (ma_device_start(&m_device) != MA_SUCCESS) {
        fprintf(stderr, "Failed to start audio device.\n");
        return false;
    }
    m_running = true;
    return true;
}

void AudioEngine::stop() {
    if (m_running) {
        ma_device_stop(&m_device);
        m_running = false;
    }
}

void AudioEngine::shutdown() {
    stop();
    ma_device_uninit(&m_device);
}

void AudioEngine::dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    auto* engine = static_cast<AudioEngine*>(pDevice->pUserData);
    float* out = static_cast<float*>(pOutput);

    if (engine->m_testMode) {
        for (ma_uint32 i = 0; i < frameCount; ++i) {
            apu::instance().cycle(engine->m_cyclesPerSample);
            float sample = apu::instance().mix_sample();
            *out++ = sample;
        }
    }
    else {
        for (ma_uint32 i = 0; i < frameCount; ++i) {
            *out++ = 0.0f;
        }
    }
}