#include "CPU.h"

cpu& cpu::instance()
{
	static cpu cpu;

	return cpu;
}

//вроде работает корректно
int32_t cpu::cycle(cartridge& cart)
{
	int32_t cycles_wasted = handle_interrupts(cart);

	if (current_state == HALTED) return 0;//нет затраченных m_cycles если процессор в состоянии halted
	
	parsed_instruction pi = fetch_instruction(cart);
	cycles_wasted += execute_instruction(cart, pi);//возвращает число затраченных m_cycles( в том числе отрицательное)
	
	return cycles_wasted;
}

std::string cpu::debug()
{
	std::stringstream log;
	log << "IME: " << std::hex << (unsigned)IME << " A: " << (unsigned)A << " F: " << (unsigned)F <<
		" PC: " << (unsigned)PC << " SP: " << (unsigned)SP << " IR: " << (unsigned)IR << " B: " << (unsigned)BC.B << " C: " << (unsigned)BC.C << " D: "
		 << (unsigned)DE.D << " E: " << (unsigned)DE.E << " H: " << (unsigned)HL.H << " L: " << (unsigned)HL.L
	 << " IE: "<< (unsigned)interrupt_registers::instance().get_ie() << " IF: " << (unsigned)interrupt_registers::instance().get_if();

	return log.str();
}

parsed_instruction cpu::fetch_instruction(cartridge& cart)
{
	parsed_instruction pi;
	
	pi.opcode = read_byte(this->PC, cart);
	pi.pref = false;

	this->PC++;
	this->IR = pi.opcode;

	if (pi.opcode == 0xCB) 
	{
		pi.opcode = read_byte(this->PC, cart);
		pi.pref = true;

		this->IR = pi.opcode;
		this->PC++;
	}

	return pi;
}

uint8_t cpu::execute_instruction(cartridge& cart, parsed_instruction pi)
{
	instruction instr;

	if(pi.pref) instr = instruction_set::instance().get_pref_instruction(pi.opcode);
	else instr = instruction_set::instance().get_instruction(pi.opcode);

	//std::cout << "инструкция: " << instr.mnemonic << " длинна: " << (uint16_t)instr.length << std::endl;

	return instr.func(cart);
}

uint8_t cpu::handle_interrupts(cartridge& cart)//возвращает количество затраченных машинных циклов (5 или 0)
{//все же IME отключается только если IF & IE != 0
	interrupt_registers& ir = interrupt_registers::instance();
	uint8_t IE = ir.get_ie();
	uint8_t IF = ir.get_if();

	if (IF & IE) current_state = RUNNING;//если получено прерывание то процессор просыпается
	//нужно подумать как реализовать halt bug
	if (IME) 
	{//если прерывание есть, то IME = 0 и соответствующий бит в IF сбрасывается
		//std::cout << "some interruption" << std::endl;
		//если есть прерывание то процессор просыпается от halt
		if ((IF & 0b000000001) & IE)//vblank
		{

			//std::cout << "VBLANK INTERRUPTION!" << std::endl;

			IF &= ~0b000000001;
			ir.set_if(IF);
			IME = 0;

			SP -= 1;
			write_byte(SP, PC >> 8, cart);//старшая часть
			SP -= 1;
			write_byte(SP, PC & 0xFF, cart);//младшая часть

			PC = 0x40;

			return 5;
		}
		else if ((IF & 0b000000010) & IE)//lcd
		{
			//std::cout << "STAT INTERRUPTION!" << std::endl;

			IF &= ~0b000000010;
			ir.set_if(IF);
			IME = 0;

			SP -= 1;
			write_byte(SP, PC >> 8, cart);//старшая часть
			SP -= 1;
			write_byte(SP, PC & 0xFF, cart);//младшая часть

			PC = 0x48;

			return 5;
		}
		else if ((IF & 0b000000100) & IE)//timer
		{
			IF &= ~0b000000100;
			ir.set_if(IF);
			IME = 0;

			SP -= 1;
			write_byte(SP, PC >> 8, cart);//старшая часть
			SP -= 1;
			write_byte(SP, PC & 0xFF, cart);//младшая часть

			PC = 0x50;

			//std::cout << "TIMER INTERRUPTION!" << std::endl;

			return 5;
		}
		else if ((IF & 0b000001000) & IE)//serial
		{
			IF &= ~0b000001000;
			ir.set_if(IF);
			IME = 0;

			SP -= 1;
			write_byte(SP, PC >> 8, cart);//старшая часть
			SP -= 1;
			write_byte(SP, PC & 0xFF, cart);//младшая часть

			PC = 0x58;

			return 5;
		}
		else if ((IF & 0b000010000) & IE)//joypad
		{
			//std::cout << "JOYPAD" << std::endl;

			IF &= ~0b000010000;
			ir.set_if(IF);
			IME = 0;

			SP -= 1;
			write_byte(SP, PC >> 8, cart);//старшая часть
			SP -= 1;
			write_byte(SP, PC & 0xFF, cart);//младшая часть

			PC = 0x60;

			return 5;
		}
	}
	return 0;
}

/*cpu::cpu() : current_state(RUNNING), IME(0x0), A(0x01), F(0xB0), PC(0x0100), SP(0xFFFE), IR(0x0) // -----оригинал
{
	BC.B = 0x00;
	BC.C = 0x13;
	DE.D = 0x00;
	DE.E = 0xD8;
	HL.H = 0x01;
	HL.L = 0x4D;
}*/

cpu::cpu() : current_state(RUNNING), IME(0x0), A(0x11), F(0x80), PC(0x100), SP(0xFFFE), IR(0x0)
{
	BC.B = 0x00;
	BC.C = 0x00;
	DE.D = 0xFF;
	DE.E = 0x56;
	HL.H = 0x00;
	HL.L = 0x0D;
}