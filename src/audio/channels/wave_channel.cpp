#include "wave_channel.h"

wave_channel::wave_channel()
{
}

void wave_channel::cycle(uint32_t mcycles)
{
	if (enabled) 
	{
		cycles_accumulated += mcycles;

		uint16_t sample_rate = (2048 - current_period) * 2;
		if (cycles_accumulated >= sample_rate)
		{
			sample_index = (sample_index + 1) % 32;
			current_period = get_period();
			cycles_accumulated -= sample_rate;
		}
	}
}

void wave_channel::cycle_length_timer()
{
	if (enabled)
	{
		length_timer++;

		if (length_timer >= WAVE_LENGTH_TIMER_MAX_VALUE)
		{
			enabled = false;
		}
	}
}

float wave_channel::get_sample() const
{
	uint8_t wave_ram_byte = wave_ram[sample_index / 2];
	uint8_t sample_value = sample_index % 2 ? wave_ram_byte & 0xF : (wave_ram_byte >> 4) & 0xF;
	sample_value = sample_value * volume;
	return 0.0f;
}

void wave_channel::trigger()
{
	enabled = true;

	/*length timer remains the same because in this
	implementation timer counter increments, so 0
	is equal to 256 */

	sample_index = 0;
	current_period = get_period();
	volume = get_initial_volume();

	if (dac_enabled()) enabled = false;
}

void wave_channel::reset()
{
	NR0 = NR2 = NR3 = NR4 = 0;
	trigger();
}
