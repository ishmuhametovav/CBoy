#pragma once

#include "miniaudio.h"
#include "APU.h"

class audio_engine
{
public:
    audio_engine(uint32_t channels = 1);
    ~audio_engine();

    void init();
    void start();
    void stop();
    void shutdown();

private:
    ma_device device;
    ma_device_config device_config;

    unsigned int channels;
    bool running;
};

void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count);