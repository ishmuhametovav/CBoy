#pragma once
#include<cstdint>
#include<array>
#include"channel.h"

constexpr uint16_t DIV_APU_TICK = 1024; //количество M-Cycles для одного тика APU

class apu//singleton класс аудио чипа
{
	channel1* ch1;
	channel2* ch2;
	channel3* ch3;
	channel4* ch4;

	uint8_t NR50;//основное управление аудио
	uint8_t NR51;//панорамирование
	uint8_t NR52;//громкость

	//div-apu счетчик
	uint16_t counter;
	//накопленные такты (M-Cycles)
	uint16_t cycles_accumulated;

	~apu(){ }

	apu(const apu&) = delete;
	apu& operator = (const apu&) = delete;

	apu();

public:
	static apu& instance();

	void set_register(uint16_t address, uint8_t value);
	uint8_t get_register(uint16_t address);

	void cycle(int32_t m_cycles);
	float mix_sample();
	void reset();
};
