// AudioEngine.cpp
#define MINIAUDIO_IMPLEMENTATION 
#include "audio_engine.h"
#include "APU.h"      // где находится apu::instance()
#include <cstdio>

// Конструктор задает значения по умолчанию.
AudioEngine::AudioEngine()
    : m_sampleRate(44100), m_channels(1), m_running(false)
{
    m_cyclesPerSample = 4194304 / m_sampleRate;  // для Game Boy: тактовая частота ~4194304 Гц
}

// Деструктор: останавливаем аудио и деинициализируем устройство.
AudioEngine::~AudioEngine() {
    shutdown();
}

bool AudioEngine::init(unsigned int sampleRate, unsigned int channels)
{
    m_sampleRate = sampleRate;
    m_channels = channels;
    m_cyclesPerSample = 4194304 / m_sampleRate;  // пересчитываем

    // Инициализация конфигурации устройства.
    m_deviceConfig = ma_device_config_init(ma_device_type_playback);
    m_deviceConfig.sampleRate = m_sampleRate;
    m_deviceConfig.playback.channels = m_channels;
    m_deviceConfig.playback.format = ma_format_f32; // 32-битный float
    m_deviceConfig.dataCallback = AudioEngine::data_callback;
    m_deviceConfig.pUserData = nullptr; // Если нужно, можно передать указатель на AudioEngine или APU

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

// Callback-функция miniaudio: вызывается автоматически, когда аудиовыход требует сэмплов.
void AudioEngine::data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    (void)pInput;
    // Работаем с форматом f32, поэтому pOutput — это float*
    float* pOut = (float*)pOutput;

    // Для каждого аудиосэмпла вызываем методы APU:
    for (ma_uint32 i = 0; i < frameCount; i++) {

        // Получаем итоговый сэмпл с микшированием каналов.
        float sample = apu::instance().mix_sample();
        *pOut++ = sample;
    }
}
