#pragma once
#include <cstdint>
#include <array>
#include <iostream>
#include <memory>
#include "./channels/sweep_pulse_channel.h"
#include "./ring_buffer/ring_buffer.h"

constexpr uint16_t DIV_APU_CLOCK = 2048;
constexpr uint32_t SAMPLE_RATE = 44100;
/*integer value, maybe double in future*/
constexpr uint32_t SAMPLE_CLOCK = 1048576 / SAMPLE_RATE;

class apu//singleton apu class
{
	bool enabled;

	uint8_t NR50;
	uint8_t NR51;
	uint8_t NR52;

	uint16_t sample_cycles;
	uint16_t length_timer_cycles;
	uint16_t envelope_cycles;
	uint16_t sweep_cycles;

	std::unique_ptr<sweep_pulse_channel> ch1;
	std::unique_ptr<pulse_channel> ch2;

	ring_buffer<float> buffer;

	~apu(){ }

	apu(const apu&) = delete;
	apu& operator = (const apu&) = delete;

	apu();
	//updates NR52 register every time its read
	void update_channels_status();
public:
	static apu& instance();

	void enable();
	void disable();

	void set_register(uint16_t address, const uint8_t value);
	uint8_t get_register(uint16_t address);

	void cycle(int32_t mcycles);
	float mix_sample();
	/*feed samples to audio engine
	remained buffer filled with 0*/
	void feed_samples(float* output, const size_t count);
};
