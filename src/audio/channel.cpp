#include "channel.h"

uint16_t channel::get_initial_length_timer() const
{
	return NR1 & 0x3F; //для остальных каналов длина таймера берется из младших 6 бит NR1
}

channel::channel(uint8_t NR1, uint8_t NR2, uint8_t NR3, uint8_t NR4)
	: enabled(false), NR1(NR1), NR2(NR2), NR3(NR3), NR4(NR4), cycles_accumulated(0)
{
	length_timer = 0;
	period = 0;
	volume = 0;
}

void channel::update_length_timer()
{
	bool timer_enabled = NR4 & 0x40; //проверка бита активации таймера длины

	if (timer_enabled)
	{
		if (length_timer > 0) 
		{
			length_timer--;
			if (length_timer == 0)//выключаем канал, если таймер длины истек
				enabled = false;
		}
	}
}

void channel::set_nr3(const uint8_t NR3)
{
	this->NR3 = NR3;
	period = 2048 - (NR3 | static_cast<uint16_t>(((NR4 & 0x7) << 8))); //обновление периода
}

void channel::set_nr4(const uint8_t NR4)
{
	this->NR4 = NR4;
	bool timer_enabled = NR4 & 0x40; //проверка бита активации таймера длины
	bool triggered = NR4 & 0x80; //проверка бита триггера
	if (triggered) //если триггер активирован, сбрасываем состояние канала
	{
		reset();
	}
	if (timer_enabled)
	{
		length_timer = get_initial_length_timer();
	}
	else
	{
		length_timer = 0; //если таймер не активен, сбрасываем его
	}
}

channel1::channel1(uint8_t NR0, uint8_t NR1, uint8_t NR2, uint8_t NR3, uint8_t NR4)
	: channel(NR1, NR2, NR3, NR4), NR0(NR0), duty_step(0), env_counter(0), sweep_counter(0)
{
	length_timer = NR1 & 0x3F;
	shadow_register = (NR3 | static_cast<uint16_t>(((NR4 & 0x7) << 8)));//теневой регистр для сдвига
	period = 2048 - shadow_register;
	volume = NR2 >> 4;//громкость от 0 до 15
}

void channel1::cycle(int32_t mcycles)
{
	if((NR2 & 0xF8) == 0)//если DAC выключен
	{
		enabled = false;
		return;
	}

	cycles_accumulated += mcycles;

	if(cycles_accumulated >= period)//переход к следующему шагу волны
	{
		duty_step = (duty_step + 1) % 8;
		cycles_accumulated -= period;
	}
}

void channel1::reset()
{
	enabled = true;
	cycles_accumulated = 0;
	period = 2048 - (NR3 | static_cast<uint16_t>(((NR4 & 0x7) << 8)));
	volume = NR2 >> 4; //громкость от 0 до 15
	shadow_register = (NR3 | static_cast<uint16_t>(((NR4 & 0x7) << 8))); //теневой регистр для сдвига
	length_timer = get_initial_length_timer(); //инициализация таймера длины
}

float channel1::get_sample()
{
	const uint8_t duty_patterns[4][8]{
			{0,0,0,0,0,0,0,1}, // 12.5%
			{1,0,0,0,0,0,0,1}, // 25%  
			{1,1,1,1,0,0,0,0}, // 50%
			{0,1,1,1,1,1,1,0}  // 75%
	};

	uint8_t duty = NR1 >> 6;//скважность

	uint8_t pattern = duty_patterns[duty][duty_step]; //получаем текущий шаг волны
	uint16_t digital_sample = pattern * volume; //умножаем на громкость

	float amplitude = digital_sample / 15.0f; //масштабируем значение в диапазон [0, 1]
	float sample = 1.0f - 2.0f * amplitude;

	return sample;
}


void channel1::envelope_tick()
{
	uint8_t sweep_pace = NR2 & 0x7; //pace of envelope
	if (sweep_pace == 0) return; //если pace равен 0, огибающая отключена
	int8_t sweep_direction = (NR2 & 0x8) ? 1 : -1; //направление огибающей
	env_counter++;

	if (env_counter >= sweep_pace)
	{
		env_counter = 0;
		volume += sweep_direction;

		if (volume > 15) volume = 15; //ограничение громкости
	}
}


void channel1::sweep_tick()
{
	uint8_t pace = (NR0 >> 4) & 0x7; // pace of sweep
	int8_t direction = (NR0 & 0x8) ? -1 : 1; //направление сдвига
	uint8_t shift = NR0 & 0x7; //количество бит сдвига

	if (pace == 0 || shift == 0) return; //если pace равен 0 сдвиг не выполняется

	sweep_counter++;

	if (sweep_counter < pace) return; //если счетчик сдвига меньше pace, сдвиг не выполняется
	sweep_counter = 0; //сброс счетчика сдвига

	uint16_t delta = (shadow_register >> shift) * direction;

	uint16_t new_shadow_register = shadow_register + delta;

	if(new_shadow_register > 2047) //проверка на переполнение
	{
		enabled = false; //если переполнение, канал выключается
		return;
	}

	shadow_register = new_shadow_register; //обновление теневого регистра
	NR4 = (NR4 & 0xF8) | (shadow_register >> 8); //обновление NR4 с новым значением старших битов
	NR3 = shadow_register & 0xFF; //обновление NR3 с новым значением младших битов
	period = 2048 - shadow_register; //обновление периода канала


}

channel2::channel2(uint8_t NR1, uint8_t NR2, uint8_t NR3, uint8_t NR4)
	: channel(NR1, NR2, NR3, NR4), duty_step(0), env_counter(0)
{
	length_timer = NR1 & 0x3F; //инициализация таймера длины
	period = (2048 - (NR3 | static_cast<uint16_t>(((NR4 & 0x7) << 8)))) * 4; //инициализация периода
}

void channel2::cycle(int32_t mcycles)
{
	if ((NR2 & 0xF8) == 0)//если DAC выключен
	{
		enabled = false;
		return;
	}

	cycles_accumulated += mcycles;

	if (cycles_accumulated >= period)//переход к следующему шагу волны
	{
		duty_step = (duty_step + 1) % 8;
		cycles_accumulated -= period;
	}
}

void channel2::reset()
{
	enabled = true;
	cycles_accumulated = 0;
	period = 2048 - (NR3 | static_cast<uint16_t>(((NR4 & 0x7) << 8)));
	volume = NR2 >> 4; //громкость от 0 до 15
	length_timer = get_initial_length_timer(); //инициализация таймера длины
}

float channel2::get_sample()
{
	const uint8_t duty_patterns[4][8]{
			{0,0,0,0,0,0,0,1}, // 12.5%
			{1,0,0,0,0,0,0,1}, // 25%  
			{1,1,1,1,0,0,0,0}, // 50%
			{0,1,1,1,1,1,1,0}  // 75%
	};

	uint8_t duty = NR1 >> 6;//скважность

	uint8_t pattern = duty_patterns[duty][duty_step]; //получаем текущий шаг волны
	uint16_t digital_sample = pattern * volume; //умножаем на громкость

	float amplitude = digital_sample / 15.0f; //масштабируем значение в диапазон [0, 1]
	float sample = 1.0f - 2.0f * amplitude;

	return sample;
}

void channel2::envelope_tick() 
{
	uint8_t sweep_pace = NR2 & 0x7; //pace of envelope
	if (sweep_pace == 0) return; //если pace равен 0, огибающая отключена
	int8_t sweep_direction = (NR2 & 0x8) ? 1 : -1; //направление огибающей
	env_counter++;

	if (env_counter >= sweep_pace)
	{
		env_counter = 0;
		volume += sweep_direction;

		if (volume > 15) volume = 15; //ограничение громкости
	}
}

channel3::channel3(uint8_t NR0, uint8_t NR1, uint8_t NR2, uint8_t NR3, uint8_t NR4)
	: channel(NR1, NR2, NR3, NR4), NR0(NR0), waveform_step(0)
{
	length_timer = NR1 & 0x3F; //инициализация таймера длины
	period = (2048 - (NR3 | static_cast<uint16_t>(((NR4 & 0x7) << 8)))) * 2; //инициализация периода
	volume = (NR2 >> 5) & 0x03; //громкость от 0 до 3
	wave_ram.fill(0); //инициализация волнового ОЗУ нулями
}

void channel3::cycle(int32_t mcycles)
{
	if ((NR0 & 0x80) == 0) //если DAC выключен
	{
		enabled = false;
		return;
	}
	cycles_accumulated += mcycles;

	if (cycles_accumulated >= period / 2) //переход к следующему шагу волны(код неверный, но оставлен для примера)
	{
		waveform_step = (waveform_step + 1) % 32;
		cycles_accumulated -= period;
	}
}

void channel3::reset() 
{
	if(NR0 & 0x80) enabled = true;
	cycles_accumulated = 0;
	period = (2048 - (NR3 | static_cast<uint16_t>(((NR4 & 0x7) << 8)))) * 2; //инициализация периода
	volume = (NR2 >> 5) & 0x03; //громкость от 0 до 3
	waveform_step = 0; //сброс шага волны
	length_timer = get_initial_length_timer(); //инициализация таймера длины
}

float channel3::get_sample() 
{
	if (volume == 0) return 0;

	uint8_t wram_value = wave_ram[waveform_step / 2]; //получаем значение из волнового ОЗУ
	wram_value = waveform_step & 1 ? wram_value & 0x0F : wram_value >> 4; //если шаг нечетный, берем младший ниббл, иначе старший

	uint16_t digital_sample;
	if(volume == 1) digital_sample = wram_value;
	else digital_sample = wram_value >> (volume - 1); //масштабируем значение в зависимости от громкости

	float amplitude = digital_sample / 15.0f; //масштабируем значение в диапазон [0, 1]
	float sample = 1.0f - 2.0f * amplitude;

	return sample;
}

channel4::channel4(uint8_t NR1, uint8_t NR2, uint8_t NR3, uint8_t NR4)
	: channel(NR1, NR2, NR3, NR4), env_counter(0), outbit(0), LFSR(0)
{
	length_timer = NR1 & 0x3F; //инициализация таймера длины

	uint8_t clock_shift = NR3 >> 4;
	uint8_t clock_divider = NR3 & 0x7;
	period = (clock_shift * std::pow(2, clock_divider + 4)) / 4; //инициализация периода

	volume = NR2 >> 4;//громкость от 0 до 15

}

void channel4::cycle(int32_t mcycles)
{
	if ((NR2 & 0xF8) == 0) //если DAC выключен
	{
		enabled = false;
		return;
	}

	cycles_accumulated += mcycles;
	if (cycles_accumulated >= period) //переход к следующему шагу волны
	{
		cycles_accumulated -= period;
		
		//вычисление нового значения LFSR
		uint8_t bit0 = LFSR & 1;
		uint8_t bit1 = (LFSR >> 1) & 1;
		uint8_t fb = (bit0 == bit1) ? 1 : 0;

		// запишем feedback в bit15 (т.е. позицию выше используемой части)
		LFSR = (LFSR & 0x7FFF) | (fb << 15);

		// если short mode, скопировать bit15 в bit7
		bool short_mode = (NR3 >> 3) & 0b1;

		if (short_mode) 
		{
			if (fb) LFSR |= (1 << 7);
			else LFSR &= ~(1 << 7);
		}

		outbit = LFSR & 1;
		LFSR = LFSR >> 1;
	}
}

void channel4::set_nr3(const uint8_t NR3)
{
	this->NR3 = NR3;

	uint8_t clock_shift = NR3 >> 4;
	uint8_t clock_divider = NR3 & 0x7;
	period = (clock_shift * std::pow(2, clock_divider + 4)) / 4; //инициализация периода
}

void channel4::envelope_tick() 
{
	uint8_t sweep_pace = NR2 & 0x7; //pace of envelope
	if (sweep_pace == 0) return; //если pace равен 0, огибающая отключена
	int8_t sweep_direction = (NR2 & 0x8) ? 1 : -1; //направление огибающей
	env_counter++;

	if (env_counter >= sweep_pace)
	{
		env_counter = 0;
		volume += sweep_direction;

		if (volume > 15) volume = 15; //ограничение громкости
	}
}

void channel4::reset() 
{
	enabled = true;
	cycles_accumulated = 0;
	period = (NR3 >> 4) * std::pow(2, (NR3 & 0x7) + 4) / 4; //инициализация периода
	volume = NR2 >> 4; //громкость от 0 до 15
	LFSR = 0; //сброс LFSR
	outbit = 0; //сброс выходного бита
	length_timer = get_initial_length_timer(); //инициализация таймера длины
}

float channel4::get_sample() 
{
	if (volume == 0 || outbit == 0) return 0;
	uint16_t digital_sample = outbit * volume; //масштабируем значение в зависимости от громкости
	float amplitude = digital_sample / 15.0f; //масштабируем значение в диапазон [0, 1]
	float sample = 1.0f - 2.0f * amplitude;
	return sample;
}