#pragma once
#include"./instructions/instruction.h"

class oam
{
	oam() : dma(0x0), memory(160) {}
	~oam() {}
	oam(const oam&) = delete;
	oam& operator = (const oam&) = delete;

	//хранит верхние 8 бит адреса (нижние 8 всегда 0x00)
	//и инициирует передачу данных
	uint8_t dma;
	//сама область oam
	std::vector<uint8_t> memory;
	//должны быть методы для передачи информации
public:
	static oam& instance();

	//смена регистра и начало передачи
	void set_dma(const uint8_t dma, cartridge& cart);

	inline uint8_t get_value(const uint16_t address) const { return memory[address - 0xFE00]; }
	inline void set_value(const uint16_t address, const uint8_t value) { memory[address - 0xFE00] = value; }
};
