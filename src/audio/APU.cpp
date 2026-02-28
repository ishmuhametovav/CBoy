#include "APU.h"

//==========================================
apu::apu() : NR50(0x77), NR51(0xF3), NR52(0xF1), counter(0), cycles_accumulated(0)
{
    ch1 = new channel1(0, 0, 0, 0, 0);
    ch2 = new channel2(0, 0, 0, 0);
    ch3 = new channel3(0, 0, 0, 0, 0);
    ch4 = new channel4(0, 0, 0, 0);
}

apu& apu::instance()
{
    static apu inst;
    return inst;
}

uint8_t apu::get_register(uint16_t address) 
{
    // Глобальные регистры
    if (address == 0xFF24) return NR50;
    if (address == 0xFF25) return NR51;
    if (address == 0xFF26) return NR52;

    // Channel 1: 0xFF10 - 0xFF14
    if (address == 0xFF10) return ch1->get_nr0();
    if (address == 0xFF11) return ch1->get_nr1();
    if (address == 0xFF12) return ch1->get_nr2();
    if (address == 0xFF13) return ch1->get_nr3();
    if (address == 0xFF14) return ch1->get_nr4();

    // Channel 2: 0xFF16 - 0xFF19
    if (address == 0xFF16) return ch2->get_nr1();
    if (address == 0xFF17) return ch2->get_nr2();
    if (address == 0xFF18) return ch2->get_nr3();
    if (address == 0xFF19) return ch2->get_nr4();

    // Channel 3: 0xFF1A - 0xFF1E
    if (address == 0xFF1A) return ch3->get_nr0();
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
    if (address == 0xFF23) return ch4->get_nr4();

    return 0xFF; // Если адрес неизвестен, возвращаем 0xFF.
}

// Методы для записи в регистры
void apu::set_register(uint16_t address, uint8_t value) 
{
    // Глобальные регистры
    if (address == 0xFF24) { NR50 = value; return; }
    if (address == 0xFF25) { NR51 = value; return; }
    if (address == 0xFF26) 
    {
        // При записи в NR52 используется только бит 7 для включения APU
        NR52 = (value & 0x80) | (NR52 & 0X7F);
        if (!(value & 0x80))
        { // если APU выключается, отключаем каналы
            ch1->reset();
            ch2->reset();
            ch3->reset();
            ch4->reset();
        }
        else reset();

        return;
    }

    // Channel 1: 0xFF10 - 0xFF14
    if (address == 0xFF10) { ch1->set_nr0(value); return; }
    if (address == 0xFF11) { ch1->set_nr1(value); return; }
    if (address == 0xFF12) { ch1->set_nr2(value); return; }
    if (address == 0xFF13) { ch1->set_nr3(value); return; }
    if (address == 0xFF14) { ch1->set_nr4(value); return; }

    // Channel 2: 0xFF16 - 0xFF19
    if (address == 0xFF16) { ch2->set_nr1(value); return; }
    if (address == 0xFF17) { ch2->set_nr2(value); return; }
    if (address == 0xFF18) { ch2->set_nr3(value); return; }
    if (address == 0xFF19) { ch2->set_nr4(value); return; }

    // Channel 3: 0xFF1A - 0xFF1E
    if (address == 0xFF1A) { ch3->set_nr0(value); return; }
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
    if (address == 0xFF23) { ch4->set_nr4(value); return; }
}

void apu::cycle(int32_t m_cycles)
{
    if (!(NR52 >> 7)) return;//APU выключен

	cycles_accumulated += m_cycles;

    ch1->cycle(m_cycles);
    ch2->cycle(m_cycles);
    ch3->cycle(m_cycles);
    ch4->cycle(m_cycles);

    if (cycles_accumulated >= DIV_APU_TICK) 
    {
        cycles_accumulated -= DIV_APU_TICK;
        counter++;

        if (counter % 2 == 0)
        {
            if(ch1->is_enabled()) ch1->update_length_timer();
            if (ch2->is_enabled()) ch2->update_length_timer();
            if (ch3->is_enabled()) ch3->update_length_timer();
            if (ch4->is_enabled()) ch4->update_length_timer();
        }

        if (counter % 4 == 0)
        {
            if (ch1->is_enabled()) ch1->sweep_tick();
        }

        if (counter % 8 == 0)
        {
            if (ch1->is_enabled()) ch1->envelope_tick();
            if (ch2->is_enabled()) ch2->envelope_tick();
            if (ch3->is_enabled()) ch4->envelope_tick();
        }

    }
}

float apu::mix_sample()
{
    float s1 = ch1->get_sample();
    float s2 = ch2->get_sample();
    float s3 = ch3->get_sample();
    float s4 = ch4->get_sample();

    // Простейшее микширование: усредняем сэмплы каналов.
    float mixed = (s1 + s2 + s3 + s4) * 0.25f;

    // Применяем мастер-усиление из NR50.
    // В NR50: верхние 3 бита – левый объем, нижние 3 бита – правый.
    // Здесь берём среднее значение для простоты.
    uint8_t leftVol = (NR50 >> 4) & 0x07;
    uint8_t rightVol = NR50 & 0x07;
    float masterVolume = ((leftVol + rightVol) / 14.0f); // нормализуем к диапазону [0,1]
    return mixed * masterVolume;
}

void apu::reset()
{
    NR50 = 0x77;
    NR51 = 0xF3;
    NR52 = 0xF1;
    ch1->reset();
    ch2->reset();
    ch3->reset();
    ch4->reset();
}

