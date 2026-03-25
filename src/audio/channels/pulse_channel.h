#pragma once

#include <cstdint>
#include <array>

constexpr uint16_t ENVELOPE_CLOCK = 2048 * 8;// envelope clock is 64Hz (every 2048 * 8 mcycles)
constexpr uint16_t LENGTH_TIMER_CLOCK = 2048 * 2;// length timer clock is 256Hz (every 2048 * 2 mcycles)
constexpr uint16_t LENGTH_TIMER_MAX_VALUE = 64;// length timer counts up to 64
constexpr std::array<std::array<uint8_t, 8>, 4> WAVE_FORMS{{
	{0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 1, 1, 1},
	{0, 1, 1, 1, 1, 1, 1, 0}
}};

class pulse_channel 
{
protected:
	bool enabled;

	uint32_t cycles_accumulated;

	uint16_t current_period;
	uint16_t length_timer;
	uint8_t sample_index;
	uint8_t volume;
	//envelope alters volume every sweep_pace ticks(NR2)
	uint8_t envelope_ticks;

	uint8_t NR1;
	uint8_t NR2;
	uint8_t NR3;
	uint8_t NR4;

	bool dac_enabled() const { return (NR2 & 0xF8) != 0; }
	bool length_timer_enabled() const { return (NR4 & 0x40) != 0; }
	uint16_t get_period() const { return ( ((NR4 & 0x07) << 8) | NR3 ); }
	uint8_t get_initial_volume() const { return NR2 >> 4; }
	uint8_t get_envelope_period() const { return NR2 & 0x07; }
public:
	pulse_channel();
	void cycle(uint32_t mcycles);
	void cycle_length_timer();
	void cycle_volume();
	//returns analog signal
	float get_sample() const;
	//APU restarted
	void reset();
	//channel triggered
	void trigger();

	uint8_t get_nr1() const { return NR1 | 0x3F; }
	uint8_t get_nr2() const { return NR2; }
	uint8_t get_nr3() const { return 0xFF; }
	uint8_t get_nr4() const { return 0xBF | (NR4 & 0x40); }
	//write to the length timer takes effect immediatly
	void set_nr1(const uint8_t value) 
	{
		this->NR1 = value; 
		length_timer = NR1 & 0x3F;
	}
	// writing to the nr2 requires retrigerring
	void set_nr2(const uint8_t value) 
	{
		this->NR2 = value; 
		if (!dac_enabled()) enabled = false;
	}

	void set_nr3(const uint8_t value) { this->NR3 = value; }

	void set_nr4(const uint8_t value) 
	{
		this->NR4 = value;
		//write to the 7bit -> triggering
		bool triggered = value & 0x80;
		if (triggered) trigger();
	}

	bool get_enabled() { return enabled; }
};