#pragma once
#include<vector>
#include<stdexcept>
#include<cstdint>

class interrupt_registers//singleton обертка для регистров прерываний
{
	uint8_t IE;
	uint8_t IF;

	interrupt_registers(const interrupt_registers&) = delete;
	interrupt_registers& operator = (const interrupt_registers&) = delete;

	~interrupt_registers(){ }

	interrupt_registers() :IE(0x0), IF(0xE1) { }

public:
	static interrupt_registers& instance();

	inline uint8_t get_ie() const { return IE; }
	inline uint8_t get_if() const { return IF; }

	inline void set_ie(const uint8_t IE) { this->IE = IE; }
	inline void set_if(const uint8_t IF) { this->IF = IF; }
};

class memory//ОЗУ
{
	std::vector<uint8_t> ram;
	std::vector<uint8_t> high_ram;

	memory(const memory&) = delete;
	memory& operator = (const memory&) = delete;

	memory();
	~memory(){ }

public:
	static memory& instance();

	uint8_t get_value(const uint16_t address) const;
	void set_value(const uint16_t address, const uint8_t value);

	uint8_t get_hram_value(const uint16_t address) const;
	void set_hram_value(const uint16_t address, const uint8_t value);
};
