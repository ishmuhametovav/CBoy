#include "sweep_pulse_channel.h"

uint16_t sweep_pulse_channel::calculate_new_period() const
{
	uint8_t sweep_direction = (NR0 >> 3) & 0x1 ? 1 : -1;//may create a function
	uint8_t sweep_step = get_sweep_step();
	uint16_t new_period = shadow_register + sweep_direction * (shadow_register >> sweep_step);
	return new_period;
}

sweep_pulse_channel::sweep_pulse_channel() : NR0(0), shadow_register(0), sweep_ticks(0), pulse_channel()
{
}

void sweep_pulse_channel::cycle_sweep()
{
	uint8_t sweep_pace = get_sweep_pace();

	/*we know that sweep could be enabled because sweep_step > 0
	(see triggering), but it does not matter if sweep pace is 0.
	Sweep unit couldnt do anything because of that even if its enabled*/
	if (!enabled || !sweep_enabled()) return;
	sweep_ticks++;

	if(sweep_ticks >= sweep_pace)
	{
		uint16_t new_period = calculate_new_period();
		uint8_t sweep_step = get_sweep_step();
		if(new_period <= 2047 && sweep_step != 0)
		{
			shadow_register = new_period;
			//setting up new period
			current_period = new_period;
			NR0 = new_period & 0xFF;
			NR1 = (NR1 & 0xF8) | ((new_period >> 8) & 0x7);

			/*after setting up current period we have to
			immediatly calculate and check new period value
			but not save it*/
			new_period = calculate_new_period();
			if (new_period > 2047 || sweep_step == 0)
			{
				enabled = false;
			}
		}
		else
		{
			enabled = false;
		}
		sweep_ticks -= sweep_pace;
	}
}

void sweep_pulse_channel::reset()
{
	pulse_channel::reset();
	//also clearing up nr0
	NR0 = 0;
}

void sweep_pulse_channel::trigger()
{
	pulse_channel::trigger();

	if(dac_enabled())
	{
		uint8_t sweep_step = get_sweep_step();

		shadow_register = get_period();
		sweep_ticks = 0;

		if(sweep_enabled() && sweep_step != 0)
		{
			/*here period will be recalculated
			and buffer overflow will be checked*/
			uint16_t new_period = calculate_new_period();
			if(new_period > 2047)
			{
				enabled = false;
			}
		}
	}
}
