#pragma once
#include<cstdint>
#include<sstream>
#include"cartridge.h"
#include"instructions/instruction_set.h"

/*	Возможная схема декодирования и выполнения инструкций
	cpu->decode_instruction(cartridge); uint8_t instr = cartridge->get_rom_value(PC); 
	IR = decode_instruction(instr); PC += 1;
*/

enum state{ RUNNING, HALTED, SLEEPING };//состояния процессора

class cpu
{
	state current_state;//текущее состояние процессора

	uint8_t IME;//Флаг мастера прерываний

	uint8_t IR;//текущая инструкция
	uint8_t A;//аккумулятор
	uint8_t F;//регистр флагов

	union 
	{
		uint16_t BC;
		struct
		{
			uint8_t C;
			uint8_t B;
		};
	}BC;

	union
	{
		uint16_t DE;
		struct
		{
			uint8_t E;
			uint8_t D;
		};
	}DE;

	union
	{
		uint16_t HL;
		struct
		{
			uint8_t L;
			uint8_t H;
		};
	} HL;

	uint16_t PC;//счетчик инструкций
	uint16_t SP;//указатель стека

	cpu(const cpu&) = delete;
	cpu& operator = (const cpu&) = delete;
	~cpu(){}

	cpu();

	parsed_instruction fetch_instruction(cartridge& cart);
	uint8_t execute_instruction(cartridge& cart, parsed_instruction pi);

	uint8_t handle_interrupts(cartridge& cart);

public:
	static cpu& instance();

	int32_t cycle(cartridge& cart);//выполнение инструкций согласно количеству машинных циклов, сменить количество машинных циклов

	std::string debug();

	//геттеры

	inline state get_current_state() const { return current_state; }

	inline uint8_t get_ime() const { return IME; }

	inline uint8_t get_ir() const { return IR; }
	inline uint8_t get_a() const { return A; }
	inline uint8_t get_f() const { return F; }

	inline uint16_t get_bc() const { return BC.BC; }
	inline uint8_t get_b() const { return BC.B; }
	inline uint8_t get_c() const { return BC.C; }

	inline uint16_t get_de() const { return DE.DE; }
	inline uint8_t get_d() const { return DE.D; }
	inline uint8_t get_e() const { return DE.E; }

	inline uint16_t get_hl() const { return HL.HL; }
	inline uint8_t get_h() const { return HL.H; }
	inline uint8_t get_l() const { return HL.L; }

	inline uint16_t get_pc() const { return PC; }
	inline uint16_t get_sp() const { return SP; }
	//сеттеры
	inline void set_current_state(const state current_state) { this->current_state = current_state; }

	inline void set_ime(const uint8_t IME) { this->IME = IME; }

	inline void set_ir(const uint8_t IR) { this->IR = IR; }
	inline void set_a(const uint8_t A) { this->A = A; }
	inline void set_f(const uint8_t F) { this->F = F; }

	inline void set_bc(const uint16_t BC) { this->BC.BC = BC; }
	inline void set_b(const uint8_t B) { this->BC.B = B; }
	inline void set_c(const uint8_t C) { this->BC.C = C; }

	inline void set_de(const uint16_t DE) { this->DE.DE = DE; }
	inline void set_d(const uint8_t D) { this->DE.D = D; }
	inline void set_e(const uint8_t E) { this->DE.E = E; }

	inline void set_hl(const uint16_t HL) { this->HL.HL = HL; }
	inline void set_h(const uint8_t H) { this->HL.H = H; }
	inline void set_l(const uint8_t L) { this->HL.L = L; }

	inline void set_pc(const uint16_t PC) { this->PC = PC; }
	inline void set_sp(const uint16_t SP) { this->SP = SP; }

};