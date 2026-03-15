#define MINIAUDIO_IMPLEMENTATION
#include "audio_engine.h"
#include "APU.h"
#include <cstdio>

audio_engine::audio_engine() : running(false), channels(0), device{}, device_config{}
{

}

audio_engine::~audio_engine()
{
    shutdown();
}

void audio_engine::init(uint32_t channels)
{
    this->channels = channels;

    device_config = ma_device_config_init(ma_device_type_playback);
    device_config.playback.format = ma_format_f32;
    device_config.playback.channels = channels;
    device_config.sampleRate = SAMPLE_RATE;
    device_config.dataCallback = data_callback;
    device_config.pUserData = this;

    if (ma_device_init(nullptr, &device_config, &device) != MA_SUCCESS)
    {
        throw std::runtime_error("failed to initialize audio engine");
    }
}

void audio_engine::start() 
{
    if (ma_device_start(&device) != MA_SUCCESS) 
    {
        throw std::runtime_error("failed to start audio engine");
    }
    running = true;
}

void audio_engine::stop()
{
    if (running) 
    {
        ma_device_stop(&device);
        running = false;
    }
}

void audio_engine::shutdown()
{
    stop();
    ma_device_uninit(&device);
}

void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frame_count) 
{
    auto* engine = static_cast<audio_engine*>(device->pUserData);
    float* out = static_cast<float*>(output);

    apu::instance().feed_samples(out, frame_count);
}