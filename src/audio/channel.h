#pragma once
#include<cstdint>
#include<array>
#include<cmath>

class channel
{
protected:
	//флаг включения канала
	bool enabled;
	//аккумулятор тактов
	int32_t cycles_accumulated;
	//таймер длины
	uint16_t length_timer;
	//период звуковой волны
	uint16_t period;
	//громкость канала
	uint8_t volume;
	//управление таймером длины
	uint8_t NR1;
	//управление громкостью и огибающей
	uint8_t NR2;
	//управление периодом(частично)
	uint8_t NR3;
	//бит активации, включение таймера длины, старшие биты перехода
	uint8_t NR4;

	virtual uint16_t get_initial_length_timer() const;

public:
	channel(uint8_t NR1, uint8_t NR2, uint8_t NR3, uint8_t NR4);
	virtual void cycle(int32_t mcycles) = 0;
	virtual void reset() = 0;
	virtual float get_sample() = 0;
	void update_length_timer();

	inline bool is_enabled() const { return enabled; }
	inline uint8_t get_nr1() const { return NR1; }
	inline uint8_t get_nr2() const { return NR2; }
	inline uint8_t get_nr3() const { return NR3; }
	inline uint8_t get_nr4() const { return NR4; }

	inline void set_nr1(const uint8_t NR1) { this->NR1 = NR1; }
	virtual void set_nr2(const uint8_t NR2) { this->NR2 = NR2; }
	virtual void set_nr3(const uint8_t NR3);
	void set_nr4(const uint8_t NR4);
};

class channel1 : public channel
{
	//sweep регистр
	uint8_t NR0;
	//шаг в фазе волны
	uint8_t duty_step;
	//счетчик огибающей
	uint8_t env_counter;
	//теневой регистр
	uint16_t shadow_register;
	//счетчик сдвига
	uint8_t sweep_counter;
public:

	channel1(uint8_t NR0, uint8_t NR1, uint8_t NR2, uint8_t NR3, uint8_t NR4);
	void cycle(int32_t mcycles) override;
	void reset() override;
	float get_sample() override;
	void envelope_tick();
	void sweep_tick();

	inline uint8_t get_nr0() const { return NR0; }
	inline void set_nr0(const uint8_t NR0) { this->NR0 = NR0; }
};

class channel2 : public channel
{
	//шаг в фазе волны
	uint8_t duty_step;
	//счетчик огибающей
	uint8_t env_counter;
public:
	channel2(uint8_t NR1, uint8_t NR2, uint8_t NR3, uint8_t NR4);
	void cycle(int32_t mcycles) override;
	void reset() override;
	float get_sample() override;
	void envelope_tick();
};

constexpr uint16_t WAVE_RAM_START = 0xFF30; //начало волнового ОЗУ

class channel3 : public channel 
{
	//волновое ОЗУ
	std::array<uint8_t, 16> wave_ram;
	//шаг в фазе волны
	uint8_t waveform_step;
	uint8_t NR0;

	uint16_t get_initial_length_timer() const override { return NR1; }
public:
	channel3(uint8_t NR0, uint8_t NR1, uint8_t NR2, uint8_t NR3, uint8_t NR4);
	void cycle(int32_t mcycles) override;
	void reset() override;
	float get_sample() override;

	inline uint8_t get_wave_ram(uint16_t address) const { return wave_ram[address - WAVE_RAM_START]; }
	inline void set_wave_ram(uint16_t address, uint8_t value) { wave_ram[address - WAVE_RAM_START] = value; }
	void set_nr2(const uint8_t NR2) override { this->NR2 = NR2; volume = (NR2 >> 5) & 0x03; }
	inline uint8_t get_nr0() const { return NR0; }
	inline void set_nr0(const uint8_t NR0) { this->NR0 = NR0; }
};

class channel4 : public channel
{
	//linear feedback shift register
	uint16_t LFSR;
	//счетчик огибающей
	uint8_t env_counter;
	//выходной сэмпл
	uint8_t outbit;
public:
	channel4(uint8_t NR1, uint8_t NR2, uint8_t NR3, uint8_t NR4);
	void cycle(int32_t mcycles) override;
	void reset() override;
	float get_sample() override;
	void envelope_tick();
	void set_nr3(const uint8_t NR3) override;
};