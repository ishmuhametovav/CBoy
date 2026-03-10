#pragma once
#include <cstdint>
#include <array>
#include <iostream>
#include "./channels/pulse_channel.h"
#include "./ring_buffer/ring_buffer.h"

constexpr uint16_t DIV_APU_CLOCK = 2048;

class apu//singleton apu class
{
	bool enabled;

	uint8_t NR50;
	uint8_t NR51;
	uint8_t NR52;

	uint16_t length_timer_cycles;
	uint16_t envelope_cycles;
	pulse_channel* ch2;

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

	void set_register(uint16_t address, uint8_t value);
	uint8_t get_register(uint16_t address);

	void cycle(int32_t m_cycles);
	float mix_sample();
};
