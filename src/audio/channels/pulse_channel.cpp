#include "pulse_channel.h"

pulse_channel::pulse_channel() : enabled(false), cycles_accumulated(0), current_period(0),
	length_timer(0), sample_index(0), volume(0), envelope_ticks(0), NR1(0), NR2(0), NR3(0), NR4(0){ }

void pulse_channel::cycle(uint32_t m_cycles)
{
	if (enabled) 
	{
		cycles_accumulated += m_cycles;

		uint16_t sample_rate = 2048 - current_period;//sample rate

		/*period counter is incremented every 1 M - Cycles(every 4 cycles of the CPU)
		when it overflows new period value is calculated from NR3 and NR4.*/
		if(cycles_accumulated >= sample_rate)//has to be if
		{
			cycles_accumulated -= sample_rate;
			sample_index = (sample_index + 1) % 8;
			current_period = get_period();
		}
	}
}

void pulse_channel::cycle_length_timer()
{
	/*length clock is actually decrementing but
	this implementation loads length clock with NR1 value
	and then incrementing it so it is the same*/
	if (enabled && length_timer_enabled())
	{
		length_timer++;

		if (length_timer >= LENGTH_TIMER_MAX_VALUE)
		{
			enabled = false;
		}
	}
}

void pulse_channel::cycle_volume()
{
	if (enabled) 
	{
		//envelope wouldnt work if period is 0
		uint8_t envelope_period = get_envelope_period();
		if (envelope_period == 0) return;

		envelope_ticks++;
		if (envelope_ticks >= envelope_period) 
		{
			envelope_ticks -= envelope_period;

			if (volume == 0 || volume == 15) return;

			int8_t direction = NR2 & 0x8 ? 1 : -1;
			volume += direction;
		}
	}
}

float pulse_channel::get_sample() const
{
	/*if dac or channel is disabled then return 0
	but we dont need to check if dac turn on because
	channel enabled <=> dac enabled(envelope mutes channel
	but dont turn off dac so the rule works)*/
	if (!dac_enabled()) return 0;
	if (!enabled) return -1;

	uint8_t wave_form = NR1 >> 6;
	uint8_t sample_value = WAVE_FORMS[wave_form][sample_index] * volume;
	float amplitude = sample_value / 15.0f;
	float sample = 2.0f * amplitude - 1.0f;
	return sample;
}

void pulse_channel::reset()
{
	//have to reset registers and all counters(NR1 stays in monochrome models)
	NR2 = NR3 = NR4 = 0;

	enabled = false;
	cycles_accumulated = 0;
	sample_index = 0;

	//length timer remains untouched on DMG models
}

void pulse_channel::trigger()
{
	//if DAC disabled, channel will not turn on
	if (dac_enabled()) 
	{
		enabled = true;
		/*it is not obvious what envelope timer should do
		so I decided to reset it here*/
		envelope_ticks = 0;

		/*length timer remains the same because in this 
		implementation timer counter increments, so 0 
		is equal to 64 */

		current_period = get_period();
		volume = get_initial_volume();
	}
}

