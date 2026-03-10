#include "APU.h"

//==========================================
apu::apu() : enabled(true), NR50(0x77), NR51(0xF3), NR52(0xF1), length_timer_cycles(0), envelope_cycles(0), buffer{ }
{
	ch2 = new pulse_channel();
}

void apu::update_channels_status()
{
    uint8_t status = 0;
    //channel2 enabled
    if (ch2->get_enabled()) status |= 0x02;

    //updating NR52
    status |= (NR52 & 0x80);
    NR52 = status;
}

apu& apu::instance()
{
    static apu inst;
    return inst;
}

void apu::enable()
{
    /*when apu powered on sequencer, square duty, wave 
    channel's sample buffer is reset to 0
    this behaviour was implemented in reset methods of the channel
    (see disable method) because apu is powered off it should work fine*/
    enabled = true;
	length_timer_cycles = 0;
	envelope_cycles = 0;
}

void apu::disable()
{
    /*when apu powered off registers(except NR52 and NRx1) cleared*/
    enabled = false;
    NR50 = NR51 = 0;
    ch2->reset();
}

uint8_t apu::get_register(uint16_t address) 
{
    if (address == 0xFF24) return NR50;
    if (address == 0xFF25) return NR51;
    if (address == 0xFF26) { update_channels_status(); return NR52 | 0x70; }

    // Channel 2: 0xFF16 - 0xFF19
    if (address == 0xFF16) return ch2->get_nr1();
    if (address == 0xFF17) return ch2->get_nr2();
    if (address == 0xFF18) return ch2->get_nr3();
    if (address == 0xFF19) return ch2->get_nr4();

    return 0xFF;
}

void apu::set_register(uint16_t address, uint8_t value)
{
    //turning off APU making its registers read only(except NR52 and channels NRX1 on DMG)
    if (!enabled && address != 0xFF26 && address != 0xFF11 && address != 0xFF16
        && address != 0xFF1B && address != 0xFF20) return;

    if (address == 0xFF24) { NR50 = value; } //NR50
    if (address == 0xFF25) { NR51 = value; } //NR51
    if (address == 0xFF26) //NR52
    {
        NR52 = value;

        if((NR52 & 0x80) == 0 && enabled)
        {
            //disabling apu when 7 bit of NR52 is set to 0
            disable();
        }
        if ((NR52 & 0x80) == 1 && !enabled)
        {
            //enabling apu when 7 bit of NR52 is set to 1
            enable();
        }
        //updating NR52 register so its value is always correct
        update_channels_status();
    }

    // Channel 2: 0xFF16 - 0xFF19
    if (address == 0xFF16) { ch2->set_nr1(value); return; }
    if (address == 0xFF17) { ch2->set_nr2(value); return; }
    if (address == 0xFF18) { ch2->set_nr3(value); return; }
    if (address == 0xFF19) { ch2->set_nr4(value); return; }
}

void apu::cycle(int32_t m_cycles)
{
    if (enabled) 
    {
		length_timer_cycles += m_cycles;
		envelope_cycles += m_cycles;

        ch2->cycle(m_cycles);

        while (length_timer_cycles >= LENGTH_TIMER_CLOCK)//has to be if
        {
            ch2->cycle_length_timer();
			length_timer_cycles -= LENGTH_TIMER_CLOCK;
        }
        while (envelope_cycles >= ENVELOPE_CLOCK)//has to be if
        {
            ch2->cycle_volume();
            envelope_cycles -= ENVELOPE_CLOCK;
        }
    }
}

float apu::mix_sample()
{
    if (!enabled) return 0.0f;

    float s1 = 0.0f;
    float s2 = ch2->get_sample();
    float s3 = 0.0f;
    float s4 = 0.0f;

    float left_sum = 0.0f;
    float right_sum = 0.0f;

    if (NR51 & 0x01) left_sum += s1;
    if (NR51 & 0x10) right_sum += s1;
    if (NR51 & 0x02) left_sum += s2;
    if (NR51 & 0x20) right_sum += s2;
    if (NR51 & 0x04) left_sum += s3;
    if (NR51 & 0x40) right_sum += s3;
    if (NR51 & 0x08) left_sum += s4;
    if (NR51 & 0x80) right_sum += s4;

    float left_vol = ((NR50 >> 4) & 0x07) + 1;
    float right_vol = (NR50 & 0x07) + 1;
    left_sum *= left_vol;
    right_sum *= right_vol;

    float mono_sample = (left_sum + right_sum) / 2.0f;

    return mono_sample;
}
