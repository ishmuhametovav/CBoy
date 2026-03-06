#include "pulse_channel.h"

pulse_channel::pulse_channel() : enabled(false), cycles_accumulated(0), current_period(0), period_counter(0),
	length_timer(0), sample_index(0), volume(0), NR1(0), NR2(0), NR3(0), NR4(0){ }

void pulse_channel::cycle(uint32_t m_cycles)
{
	if (enabled) 
	{
		cycles_accumulated += m_cycles;

		uint16_t sample_rate = 2048 - current_period;//sample rate
		if(cycles_accumulated >= sample_rate)
		{
			cycles_accumulated -= sample_rate;
			sample_index = (sample_index + 1) % 8;
		}

		/*period counter is incremented every 1 M - Cycles(every 4 cycles of the CPU)
		when it overflows new period value is calculated from NR3 and NR4.*/
		period_counter++;

		if(period_counter >= PERIOD_COUNTER_MAX_VALUE)
		{
			period_counter = current_period = get_period();
		}
	}
}

void pulse_channel::cycle_length_timer()
{
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
		if (volume == 0 || volume == 15) return;

		int8_t direction = NR2 & 0x8 ? 1 : -1;
		volume += direction;
	}
}

float pulse_channel::get_sample() const
{
	uint8_t wave_form = NR1 >> 6;
	uint8_t sample_value = WAVE_FORMS[wave_form][sample_index] * volume;
	float amplitude = sample_value / 15.0f;
	float sample = 1.0f - 2.0f * amplitude;
	return sample;
}

void pulse_channel::init()
{
	//have to reset registers?
	reset();
	sample_index = 0;
}

void pulse_channel::reset()
{
	//if DAC disabled, channel will not turn on
	if (dac_enabled()) 
	{
		enabled = true;
		cycles_accumulated = 0;

		if(length_timer == 0)
		{
			init_length_timer();//only on monochrome models
		}
		period_counter = current_period = get_period();
		volume = get_initial_volume();
	}
}

