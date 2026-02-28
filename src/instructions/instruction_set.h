#pragma once

#include<string>
#include<functional> 
#include<array>
#include"./cartridge.h"

struct parsed_instruction
{
	uint8_t opcode;
	bool pref;
};

struct instruction
{
	std::string mnemonic;
	uint8_t length;//длина в байтах
	std::function<uint8_t(cartridge& cart)> func;//функция обработки инструкции
};


#include"instruction.h"

//множество инструкций
class instruction_set
{
	std::array<instruction, 256> instructions;
	std::array<instruction, 256> pref_instructions;

	instruction_set();

	instruction_set(const instruction_set&) = delete;
	instruction_set& operator = (const instruction_set&) = delete;
	~instruction_set(){}
public:
	static instruction_set& instance();

	inline instruction get_instruction(const uint8_t address) const 
	{ return instructions[address]; }

	inline instruction get_pref_instruction(const uint8_t address) const 
	{ return pref_instructions[address]; }
};
