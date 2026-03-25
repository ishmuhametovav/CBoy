#pragma once

#include "pulse_channel.h"

constexpr uint16_t SWEEP_CLOCK = 2048 * 4;//sweep clocks every 8192 mcycles

class sweep_pulse_channel : public pulse_channel
{
	uint8_t NR0;

	uint16_t shadow_register;
	uint8_t sweep_ticks;

	uint8_t get_sweep_step() const { return NR0 & 0x7; }
	uint8_t get_sweep_pace() const { return (NR0 >> 4) & 0x7; }
	bool sweep_enabled() const { return get_sweep_pace() > 0; }

	uint16_t calculate_new_period() const;
public:
	sweep_pulse_channel();
	void cycle_sweep();
	void reset();
	void trigger();

	uint8_t get_nr0() const { return NR0 | 0x80; }
	void set_nr0(const uint8_t value) { this->NR0 = value; }
};