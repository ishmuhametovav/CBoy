#include"memory.h"

interrupt_registers& interrupt_registers::instance()
{
	static interrupt_registers ir;
	return ir;
}

memory::memory()
{
	ram.resize(8 * 1024);//8 Kib
	high_ram.resize(127);//127 байт
}

memory& memory::instance()
{
	static memory m;
	return m;
}

uint8_t memory::get_value(const uint16_t address) const
{
	if(address >= 0xC000 && address <= 0xDFFF)
	{
		return ram[address - 0xC000];
	}

	else if (address >= 0xE000 && address <= 0xFDFF)
	{
		return ram[address - 0xE000];
	}
	throw std::runtime_error("no such address");
}

void memory::set_value(const uint16_t address, const uint8_t value)
{
	if (address >= 0xC000 && address <= 0xDFFF)
	{
		ram[address - 0xC000] = value;
	}

	else if (address >= 0xE000 && address <= 0xFDFF)
	{
		ram[address - 0xE000] = value;
	}
	else throw std::runtime_error("no such address");
}

uint8_t memory::get_hram_value(const uint16_t address) const
{
	return high_ram[address - 0xFF80];
}

void memory::set_hram_value(const uint16_t address, const uint8_t value)
{
	high_ram[address - 0xFF80] = value;
}
