#include "APU.h"

//==========================================
apu::apu() : enabled(false), NR50(0x77), NR51(0xF3), NR52(0xF1), counter(0), cycles_accumulated(0),
    length_timer_cycles(0), envelope_cycles(0)
{
	ch2 = new pulse_channel();
}

apu& apu::instance()
{
    static apu inst;
    return inst;
}

void apu::init()
{
    enabled = true;

    cycles_accumulated = 0;
	length_timer_cycles = 0;
	envelope_cycles = 0;

    NR50 = 0;
    NR51 = 0;
    ch2->init();
}

uint8_t apu::get_register(uint16_t address) 
{
    if (address == 0xFF24) return NR50;
    if (address == 0xFF25) return NR51;
    if (address == 0xFF26) return NR52;

    // Channel 1: 0xFF10 - 0xFF14
    /*if (address == 0xFF10) return ch1->get_nr0();
    if (address == 0xFF11) return ch1->get_nr1();
    if (address == 0xFF12) return ch1->get_nr2();
    if (address == 0xFF13) return ch1->get_nr3();
    if (address == 0xFF14) return ch1->get_nr4();*/

    // Channel 2: 0xFF16 - 0xFF19
    if (address == 0xFF16) return ch2->get_nr1();
    if (address == 0xFF17) return ch2->get_nr2();
    if (address == 0xFF18) return ch2->get_nr3();
    if (address == 0xFF19) return ch2->get_nr4();

    // Channel 3: 0xFF1A - 0xFF1E
    /*if (address == 0xFF1A) return ch3->get_nr0();
    if (address == 0xFF1B) return ch3->get_nr1();
    if (address == 0xFF1C) return ch3->get_nr2();
    if (address == 0xFF1D) return ch3->get_nr3();
    if (address == 0xFF1E) return ch3->get_nr4();
    // Wave RAM: 0xFF30 - 0xFF3F
    if (address >= 0xFF30 && address <= 0xFF3F)
        return ch3->get_wave_ram(address);

    // Channel 4: 0xFF20 - 0xFF23
    if (address == 0xFF20) return ch4->get_nr1();
    if (address == 0xFF21) return ch4->get_nr2();
    if (address == 0xFF22) return ch4->get_nr3();
    if (address == 0xFF23) return ch4->get_nr4();*/

    return 0xFF;
}

// Методы для записи в регистры
void apu::set_register(uint16_t address, uint8_t value)
{
    //turning off APU making its registers read only(except NR52)
    //NR50
    if (address == 0xFF24)
    {
        NR50 = value;
    }
    //NR51
    if (address == 0xFF25)
    {
        NR51 = value;
    }
    //NR52
    if (address == 0xFF26) 
    {
        NR52 &= 0x7F;
        NR52 |= (value & 0x80);

        if((NR52 & 0x80) == 0 && enabled)
        {
            enabled = false;
        }
        if ((NR52 & 0x80) == 1 && !enabled)
        {
            init();
        }
    }

    // Channel 1: 0xFF10 - 0xFF14
    /*if (address == 0xFF10) { ch1->set_nr0(value); return; }
    if (address == 0xFF11) { ch1->set_nr1(value); return; }
    if (address == 0xFF12) { ch1->set_nr2(value); return; }
    if (address == 0xFF13) { ch1->set_nr3(value); return; }
    if (address == 0xFF14) { ch1->set_nr4(value); return; }*/

    // Channel 2: 0xFF16 - 0xFF19
    if (address == 0xFF16) { ch2->set_nr1(value); return; }
    if (address == 0xFF17) { ch2->set_nr2(value); return; }
    if (address == 0xFF18) { ch2->set_nr3(value); return; }
    if (address == 0xFF19) { ch2->set_nr4(value); return; }

    // Channel 3: 0xFF1A - 0xFF1E
    /*if (address == 0xFF1A) { ch3->set_nr0(value); return; }
    if (address == 0xFF1B) { ch3->set_nr1(value); return; }
    if (address == 0xFF1C) { ch3->set_nr2(value); return; }
    if (address == 0xFF1D) { ch3->set_nr3(value); return; }
    if (address == 0xFF1E) { ch3->set_nr4(value); return; }
    // Wave RAM: 0xFF30 - 0xFF3F
    if (address >= 0xFF30 && address <= 0xFF3F) 
    {
        ch3->set_wave_ram(address, value);
        return;
    }

    // Channel 4: 0xFF20 - 0xFF23
    if (address == 0xFF20) { ch4->set_nr1(value); return; }
    if (address == 0xFF21) { ch4->set_nr2(value); return; }
    if (address == 0xFF22) { ch4->set_nr3(value); return; }
    if (address == 0xFF23) { ch4->set_nr4(value); return; }*/
}

void apu::cycle(int32_t m_cycles)
{
    if (enabled) 
    {
        cycles_accumulated += m_cycles;
		length_timer_cycles += m_cycles;
		envelope_cycles += m_cycles;

        ch2->cycle(m_cycles);

        if (length_timer_cycles >= LENGTH_TIMER_CLOCK)
        {
            ch2->cycle_length_timer();
			length_timer_cycles -= LENGTH_TIMER_CLOCK;
        }
        if (envelope_cycles >= ENVELOPE_CLOCK)
        {
            ch2->cycle_volume();
            envelope_cycles -= ENVELOPE_CLOCK;
        }
    }
}

float apu::mix_sample()
{
    float s2 = ch2->get_sample();

    float mixed = s2;

    uint8_t leftVol = NR50 & 0x70;
    uint8_t rightVol = NR50 & 0x07;
    float masterVolume = ((leftVol + rightVol) / 14.0f);
    return mixed * masterVolume;
}
