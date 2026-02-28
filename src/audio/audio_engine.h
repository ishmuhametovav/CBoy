#pragma once
// AudioEngine.h
#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include "miniaudio.h"
#include <cstdint>

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    // Инициализация аудиоустройства; возвращает true, если успешно.
    bool init(unsigned int sampleRate = 44100, unsigned int channels = 1);

    // Остановка и деинициализация аудиоустройства.
    void shutdown();

    // Запуск аудиоустройства (если не запущено)
    bool start();

    // Остановка аудиоустройства
    void stop();

private:
    // Callback-функция, которая будет передана miniaudio.
    static void data_callback(ma_device* pDevice, void* pOutput,
        const void* pInput, ma_uint32 frameCount);

    // Устройство miniaudio.
    ma_device m_device;
    ma_device_config m_deviceConfig;

    // Частота дискретизации.
    unsigned int m_sampleRate;
    unsigned int m_channels;

    // Для примера, вычисляем, сколько машинных тактов соответствует одному аудиосэмплу.
    // (Можно вычислять: cyclesPerSample = (4194304 / sampleRate)).
    int m_cyclesPerSample;

    // Флаг, запущено ли аудио.
    bool m_running;
};

#endif // AUDIO_ENGINE_H
