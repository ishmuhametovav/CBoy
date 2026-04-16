#pragma once

#include <cstdint>
#include <array>

constexpr uint8_t WAVE_RAM_SIZE = 16;
constexpr uint16_t WAVE_RAM_START_ADDRESS = 0xFF30;
constexpr uint16_t WAVE_LENGTH_TIMER_MAX_VALUE = 256;

class wave_channel
{
	uint8_t NR0;
	uint8_t NR1;
	uint8_t NR2;
	uint8_t NR3;
	uint8_t NR4;

	std::array<uint8_t, WAVE_RAM_SIZE> wave_ram;

	bool enabled;
	uint32_t cycles_accumulated;

	uint8_t sample_index;
	uint16_t current_period;
	uint16_t length_timer;
	uint8_t volume;

	bool dac_enabled() const { return NR0 & 0x80; }
	uint16_t get_period() const { return (((NR4 & 0x07) << 8) | NR3); }
	uint8_t get_initial_volume() const { return (NR2 >> 5) & 0x3; }
public:
	wave_channel();

	void cycle(uint32_t mcycles);
	void cycle_length_timer();
	float get_sample() const;
	void trigger();
	void reset();

	uint8_t get_nr0() const { return NR0 | 0x7F; }
	uint8_t get_nr1() const { return NR1 | 0xFF; }
	uint8_t get_nr2() const { return NR2 | 0x9F; }
	uint8_t get_nr3() const { return NR3 | 0xFF; }
	uint8_t get_nr4() const { return NR4 | 0xBF; }
	uint8_t get_wave_ram(const uint16_t address) const { return wave_ram[address - WAVE_RAM_START_ADDRESS]; }

	void set_nr0(const uint8_t value) { this->NR0 = value; }
	void set_nr1(const uint8_t value) { this->NR1 = value; }
	void set_nr2(const uint8_t value) { this->NR2 = value; }
	void set_nr3(const uint8_t value) { this->NR3 = value; }
	void set_nr4(const uint8_t value) { this->NR4 = value; }
	void set_wave_ram(const uint16_t address, const uint8_t value) { wave_ram[address - WAVE_RAM_START_ADDRESS] = value; }

	bool get_enabled() const { return enabled; }
};