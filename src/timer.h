#pragma once
#include<cstdint>
#include"memory.h"

class timer//нужно реализовать отлавливание ложного тика!!!
{
	timer();
	~timer(){ }

	timer(const timer&) = delete;
	timer& operator = (const timer&) = delete;

	//FF04
	uint32_t counter;//при инструкции stop и записи сбрасывается. начинает работать после stop. Увеличивается со скоростью 16384 HZ
	//тоесть тактовая частота генератора делится на 256 когда поступает к таймеру для данного регистра
	//FF05
	uint16_t TIMA;//увеличивается с тактовой частотой, заданной в TAC. При переполнении TIMA = TMA, срабатывает прерывание
	//Если запись в TMA выполняется в том же машинном цикле, когда содержимое 
	
	// TMA переносится в TIMA из-за переполнения таймера, в TIMA будет передано старое значение.
	//FF06
	uint8_t TMA;
	//если 2 бит установлен, tima увеличивается на число первые 2 бита числа 
	//FF07
	uint8_t TAC;

	bool pending_overflow;//отложенное переполнение TIMA

	void handle_tima(int32_t cycles, uint32_t old_counter);
	bool check_tick(uint32_t new_counter, uint32_t old_counter);//тут возможно нужно все изменить
public:
	static timer& instance();

	void cycle(int32_t m_cycles);

	inline uint16_t get_counter() const { return counter; }//выберет только 16 бит
	inline uint8_t get_tima() const { return TIMA; }//выберет только 8 бит
	inline uint8_t get_tma() const { return TMA; }
	inline uint8_t get_tac() const { return TAC; }

	inline void set_counter() { this->counter &= 0x00FF; }
	inline void set_tima(const uint16_t TIMA) { this->TIMA = TIMA; }
	inline void set_tma(const uint8_t TMA) { this->TMA = TMA; }
	inline void set_tac(const uint8_t TAC) { this->TAC = TAC; }
};
