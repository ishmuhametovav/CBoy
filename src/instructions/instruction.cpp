#include "instruction.h"

//чтение и запись данных
uint8_t read_byte(const uint16_t address, cartridge& cart)
{
	//std::cout << "Reading data " << std::hex << address << std::endl;
	if(address >= 0x0000 && address <= 0x7FFF)//картридж
	{
		return cart.get_rom_value(address);
	}
	else if (address >= 0x8000 && address <= 0x9FFF)//видеопамять
	{
		if (gpu::instance().get_mode() == 3)
		{
			//std::cout << "нельзя читать из vram" << std::endl;
			return 0xFF;
		}
		else return gpu::instance().get_vram(address);
	}
	else if(address >= 0xA000 && address <= 0xBFFF)//ОЗУ картриджа
	{
		return cart.get_ram_value(address);
	}
	else if(address >= 0xC000 && address <= 0xFDFF)//Внутренняя ОЗУ
	{
		return memory::instance().get_value(address);
	}
	else if(address >= 0xFE00 && address <= 0xFE9F)//OAM
	{
		//std::cout << "Reading from OAM!" << std::endl;
		if (gpu::instance().get_mode() == 3 || gpu::instance().get_mode() == 2) return 0xFF;
		else { return oam::instance().get_value(address); }
	}
	else if(address >= 0xFF00 && address <= 0xFF7F)//IO регистры
	{
		if(address == 0xFF00)//joypad
		{
			return input::instance().get_p1();
		}
		else if (address == 0xFF04) //div
		{
			return (timer::instance().get_counter() >> 8);
		}
		else if (address == 0xFF05)//tima
		{
			return timer::instance().get_tima();
		}
		else if (address == 0xFF06)//tma
		{
			return timer::instance().get_tma();
		}
		else if (address == 0xFF07)//tac
		{
			return timer::instance().get_tac();
		}
		else if(address == 0xFF0F)//регистр флагов прерывания IF
		{
			return interrupt_registers::instance().get_if();
		}
		else if(address >= 0xFF10 && address <= 0xFF26)//APU
		{
			//std::cout << "APU is working" << std::endl;
			return apu::instance().get_register(address);
		}
		else if(address >= 0xFF40 && address <= 0xFF4B)//GPU
		{
			return gpu::instance().get_register(address);
		}
	}
	else if(address >= 0xFF80 && address <= 0xFFFE)//high ram
	{
		//std::cout << "reading hram" << std::endl;
		return memory::instance().get_hram_value(address);
	}
	else if (address == 0xFFFF) //регистр разрешения прерываний IE
	{
		return interrupt_registers::instance().get_ie();
	}

	return 0;
	//throw std::runtime_error("unsupported device");
}

void write_byte(const uint16_t address, const uint8_t value, cartridge& cart)
{
	//std::cout << "Writing data " << std::hex << address << std::endl;
	if (address >= 0x0000 && address <= 0x7FFF)//картридж
	{
		cart.set_mbc_register(address, value);
	}
	else if (address >= 0x8000 && address <= 0x9FFF)//видеопамять
	{
		if (gpu::instance().get_mode() == 3);// std::cout<<"нельзя записать в vram"<<std::endl;

		else 
		{ 
			gpu::instance().set_vram(address, value); 
			if(address >= 0x8000 && address <= 0x97FF)//обновляем tile_list
			{
				gpu::instance().get_tile_list().update(address, cart);
			}
			else if (address >= 0x9800 && address <= 0x9FFF)//обновляем tile_map
			{
				gpu::instance().get_tile_map().update(address, value, cart);
			}
		}
	}
	else if (address >= 0xA000 && address <= 0xBFFF)//ОЗУ картриджа
	{

		cart.set_ram_value(address, value);
	}
	else if (address >= 0xC000 && address <= 0xFDFF)//Внутренняя ОЗУ
	{
		memory::instance().set_value(address, value);
	}
	else if (address >= 0xFE00 && address <= 0xFE9F)//OAM
	{
		/*std::cout << "[OAM WRITE] mode="
			<< (int)gpu::instance().get_mode()
			<< " addr=0x" << std::hex << address
			<< " val=0x" << (int)value << "\n";*/

		if (gpu::instance().get_mode() == 3 || gpu::instance().get_mode() == 2);
		else 
		{ 
			oam::instance().set_value(address, value); 
			gpu::instance().get_sprite_list().update(address, value);
		}
	}
	else if (address >= 0xFF00 && address <= 0xFF7F)//IO регистры
	{
		if (address == 0xFF00)//joypad
		{
			input::instance().set_p1(value);
		}
		else if (address == 0xFF04) //div
		{
			timer::instance().set_counter();
		}
		else if (address == 0xFF05)//tima
		{
			timer::instance().set_tima(value);
		}
		else if (address == 0xFF06)//tma
		{
			timer::instance().set_tma(value);
		}
		else if (address == 0xFF07)//tac
		{
			timer::instance().set_tac(value);
		}
		if (address == 0xFF0F)//регистр флагов прерывания IF
		{
			interrupt_registers::instance().set_if(value);
		}
		else if (address >= 0xFF10 && address <= 0xFF26)//APU
		{
			//std::cout << "APU is setting" << std::endl;
			apu::instance().set_register(address, value);
		}
		else if (address >= 0xFF40 && address <= 0xFF4B)//GPU
		{
			if (address == 0xFF46) 
			{ 
				//std::cout << "DMA Transfer!" << std::endl; 
				oam::instance().set_dma(value, cart); //DMA передача
				gpu::instance().get_sprite_list().dma_write(cart);//обновление sprite_list
			}
			else gpu::instance().set_register(address, value);
		}

	}
	else if (address >= 0xFF80 && address <= 0xFFFE)//high ram
	{
		//std::cout << "writing hram" << std::endl;
		memory::instance().set_hram_value(address, value);
	}
	else if (address == 0xFFFF) //регистр разрешения прерываний IE
	{
		interrupt_registers::instance().set_ie(value);
	}

	//throw std::runtime_error("unsupported device");
}

//обработчики инструкций
//block0

uint8_t flow::nop(cartridge& cart)
{
	return 1;
}

uint8_t load::ld_0x0a(cartridge& cart)//ld a, (bc)
{
	cpu& cp = cpu::instance();

	uint8_t z = read_byte(cp.get_bc(), cart);
	cp.set_a(z);

	return 2;
}

uint8_t load::ld_0x1a(cartridge& cart)//ld a, (de)
{
	cpu& cp = cpu::instance();

	uint8_t z = read_byte(cp.get_de(), cart);
	cp.set_a(z);

	return 2;
}

uint8_t load::ld_0x2a(cartridge& cart)//ld a, (hl+)
{
	cpu& cp = cpu::instance();

	uint8_t z = read_byte(cp.get_hl(), cart);
	cp.set_a(z);

	cp.set_hl(cp.get_hl() + 1);

	return 2;
}

uint8_t load::ld_0x3a(cartridge& cart)//ld a, (hl-)
{
	cpu& cp = cpu::instance();

	uint8_t z = read_byte(cp.get_hl(), cart);
	cp.set_a(z);

	cp.set_hl(cp.get_hl() - 1);

	return 2;
}

uint8_t load::ld_0x02(cartridge& cart)//ld (bc), a
{
	cpu& cp = cpu::instance();

	//if (cp.get_bc() >= 0xFF40 && cp.get_hl() <= 0xFF4B)
	//std::cout << "[LOAD] ld (bc), a: "
		//<< "bc=0x" << std::hex << cp.get_bc()
		//<< " a=0x" << (int)cp.get_a() << "\n";

	write_byte(cp.get_bc(), cp.get_a(), cart);
	return 2;
}

uint8_t load::ld_0x12(cartridge& cart)//ld (de), a
{
	cpu& cp = cpu::instance();

	//if (cp.get_de() >= 0xFF40 && cp.get_hl() <= 0xFF4B)
	//std::cout << "[LOAD] ld (de), a: "
		//<< "de=0x" << std::hex << cp.get_de()
		//<< " a=0x" << (int)cp.get_a() << "\n";

	write_byte(cp.get_de(), cp.get_a(), cart);

	return 2;
}

uint8_t load::ld_0x22(cartridge& cart)//ld (hl+), a
{
	cpu& cp = cpu::instance();

	//if (cp.get_hl() >= 0xFF40 && cp.get_hl() <= 0xFF4B)
	//std::cout << "[LOAD] ld (hl+), a: "
		//<< "hl=0x" << std::hex << cp.get_hl()
		//<< " a=0x" << (int)cp.get_a() << "\n";

	write_byte(cp.get_hl(), cp.get_a(), cart);
	cp.set_hl(cp.get_hl() + 1);

	return 2;
}

uint8_t load::ld_0x32(cartridge& cart)//ld (hl-), a
{
	cpu& cp = cpu::instance();

	//if (cp.get_hl() >= 0xFF40 && cp.get_hl() <= 0xFF4B)
	//std::cout << "[LOAD] ld (hl-), a: "
		//<< "hl=0x" << std::hex << cp.get_hl()
		//<< " a=0x" << (int)cp.get_a() << "\n";

	write_byte(cp.get_hl(), cp.get_a(), cart);
	cp.set_hl(cp.get_hl() - 1);

	return 2;
}
//==========================================возможно оптимизировать(вынести повтораяющийся код в функцию)===================== - выполнено

static uint16_t read_imm16(cartridge& cart)//чтение 16 битного числа и увеличение pc
{
	cpu& cp = cpu::instance();

	uint8_t z = read_byte(cp.get_pc(), cart);
	cp.set_pc(cp.get_pc() + 1);

	uint8_t w = read_byte(cp.get_pc(), cart);
	cp.set_pc(cp.get_pc() + 1);

	uint16_t res = ((uint16_t)w << 8) | z;
	return res;
}


uint8_t load::ld_0x08(cartridge& cart)//LD [imm16], SP
{
	cpu& cp = cpu::instance();
	//считываем 2 байтовый адрес
	uint16_t address = read_imm16(cart);
	//делим sp на 2 части
	uint8_t spl = cp.get_sp() & 0b11111111;
	uint8_t sph = cp.get_sp() >> 8;
	//запись
	write_byte(address, spl, cart);
	write_byte(address + 1, sph, cart);

	return 5;
}

uint8_t load::ld_0x01(cartridge& cart)//ld bc, nn
{
	cpu& cp = cpu::instance();
	cp.set_bc(read_imm16(cart));

	return 3;
}

uint8_t load::ld_0x11(cartridge& cart)//ld de, nn
{
	cpu& cp = cpu::instance();
	cp.set_de(read_imm16(cart));

	return 3;
}

uint8_t load::ld_0x21(cartridge& cart)//ld hl, nn
{
	cpu& cp = cpu::instance();
	cp.set_hl(read_imm16(cart));

	return 3;
}

uint8_t load::ld_0x31(cartridge& cart)//ld sp, nn
{
	cpu& cp = cpu::instance();
	cp.set_sp(read_imm16(cart));

	return 3;
}
//===================================

static void add_hl_r16(uint16_t r16) //- тут будет надо проверить
{
	cpu& cp = cpu::instance();

	uint16_t hl = cp.get_hl();
	uint8_t f = 0;

	//сложение
	uint32_t result = hl + r16;
	cp.set_hl(result & 0xFFFF);

	//установка флагов
	if (result > 0xFFFF) f |= 0b00010000;
	if ((hl & 0x0FFF) + (r16 & 0x0FFF) > 0x0FFF) f |= 0b00100000;

	cp.set_f((cp.get_f() & 0b10000000) | f);
}

uint8_t arithmetic::add_0x09(cartridge& cart)//add hl, bc 
{
	add_hl_r16(cpu::instance().get_bc());

	return 2;
}

uint8_t arithmetic::add_0x19(cartridge& cart)//add hl, de
{
	add_hl_r16(cpu::instance().get_de());

	return 2;
}

uint8_t arithmetic::add_0x29(cartridge& cart)//add hl, hl
{
	add_hl_r16(cpu::instance().get_hl());

	return 2;
}

uint8_t arithmetic::add_0x39(cartridge& cart)//add hl, sp
{
	add_hl_r16(cpu::instance().get_sp());

	return 2;
}

//==============================================

uint8_t arithmetic::dec_0x0b(cartridge& cart)//dec bc
{
	cpu& cp = cpu::instance();
	cp.set_bc(cp.get_bc() - 1);

	return 2;
}
uint8_t arithmetic::dec_0x1b(cartridge& cart)//dec de
{
	cpu& cp = cpu::instance();
	cp.set_de(cp.get_de() - 1);

	return 2;
}
uint8_t arithmetic::dec_0x2b(cartridge& cart)//dec hl
{
	cpu& cp = cpu::instance();
	cp.set_hl(cp.get_hl() - 1);

	return 2;
}
uint8_t arithmetic::dec_0x3b(cartridge& cart)//dec sp
{
	cpu& cp = cpu::instance();
	cp.set_sp(cp.get_sp() - 1);

	return 2;
}

uint8_t arithmetic::inc_0x03(cartridge& cart)//inc bc
{
	cpu& cp = cpu::instance();
	cp.set_bc(cp.get_bc() + 1);

	return 2;
}

uint8_t arithmetic::inc_0x13(cartridge& cart)//inc de
{
	cpu& cp = cpu::instance();
	cp.set_de(cp.get_de() + 1);

	return 2;
}

uint8_t arithmetic::inc_0x23(cartridge& cart)//inc hl
{
	cpu& cp = cpu::instance();
	cp.set_hl(cp.get_hl() + 1);

	return 2;
}

uint8_t arithmetic::inc_0x33(cartridge& cart)//inc sp
{
	cpu& cp = cpu::instance();
	cp.set_sp(cp.get_sp() + 1);

	return 2;
}
//=========================================================dec тоже можно оптимизировать(скорее всего) - выполнено

static uint8_t dec_r8(uint8_t r8)//dec r8 и изменение регистра флагов
{
	cpu& cp = cpu::instance();

	uint8_t f = 0;
	uint8_t res = r8 - 1;


	f = res == 0 ? f |= 0b10000000 : f;
	f |= 0b01000000;
	f = (r8 & 0x0F) == 0x00 ? f | 0b00100000 : f;

	cp.set_f((cp.get_f() & 0b00010000) | f);

	return res;
}

uint8_t arithmetic::dec_0x05(cartridge& cart)//dec b
{
	cpu& cp = cpu::instance();
	cp.set_b(dec_r8(cp.get_b()));

	return 1;
}

uint8_t arithmetic::dec_0x15(cartridge& cart)//dec d
{
	cpu& cp = cpu::instance();
	cp.set_d(dec_r8(cp.get_d()));

	return 1;
}

uint8_t arithmetic::dec_0x25(cartridge& cart)//dec h
{
	cpu& cp = cpu::instance();
	cp.set_h(dec_r8(cp.get_h()));

	return 1;
}

uint8_t arithmetic::dec_0x35(cartridge& cart)//dec (hl) -- тут нечего оптимизировать
{
	cpu& cp = cpu::instance();

	/*std::cout << "[DEC] dec (hl): "
		<< "hl=0x" << std::hex << cp.get_hl()
		<< " val=0x" << (int)read_byte(cp.get_hl(), cart) << "\n";*/

	write_byte(cp.get_hl(), dec_r8(read_byte(cp.get_hl(), cart)), cart);

	return 3;
}

uint8_t arithmetic::dec_0x0d(cartridge& cart)//dec c
{
	cpu& cp = cpu::instance();
	cp.set_c(dec_r8(cp.get_c()));

	return 1;
}

uint8_t arithmetic::dec_0x1d(cartridge& cart)//dec e
{
	cpu& cp = cpu::instance();
	cp.set_e(dec_r8(cp.get_e()));

	return 1;
}

uint8_t arithmetic::dec_0x2d(cartridge& cart)//dec l
{
	cpu& cp = cpu::instance();
	cp.set_l(dec_r8(cp.get_l()));

	return 1;
}

uint8_t arithmetic::dec_0x3d(cartridge& cart)//dec a
{
	cpu& cp = cpu::instance();
	cp.set_a(dec_r8(cp.get_a()));

	return 1;
}
//===============================

static uint8_t read_imm8(cartridge& cart)//чтение 1 байтого числа и инкремент pc
{
	cpu& cp = cpu::instance();

	uint8_t data = read_byte(cp.get_pc(), cart);
	cp.set_pc(cp.get_pc() + 1);
	return data;
}

static uint8_t inc_r8(uint8_t r8)//inc r8 и изменение регистра флагов
{
	cpu& cp = cpu::instance();

	uint8_t f = 0;
	uint8_t res = r8 + 1;

	f = res == 0 ? f |= 0b10000000 : f;
	f = (r8 & 0x0F) == 0x0F ? f | 0b00100000 : f;

	cp.set_f((cp.get_f() & 0b00010000) | f);

	return res;
}

uint8_t arithmetic::inc_0x04(cartridge& cart)//inc b
{
	cpu& cp = cpu::instance();
	cp.set_b(inc_r8(cp.get_b()));

	return 1;
}

uint8_t arithmetic::inc_0x14(cartridge& cart)//inc d
{
	cpu& cp = cpu::instance();
	cp.set_d(inc_r8(cp.get_d()));

	return 1;
}

uint8_t arithmetic::inc_0x24(cartridge& cart)//inc h
{
	cpu& cp = cpu::instance();
	cp.set_h(inc_r8(cp.get_h()));

	return 1;
}

uint8_t arithmetic::inc_0x34(cartridge& cart)//inc (hl)
{
	cpu& cp = cpu::instance();

	write_byte(cp.get_hl(), inc_r8(read_byte(cp.get_hl(), cart)), cart);
	return 3;
}

uint8_t arithmetic::inc_0x0c(cartridge& cart)//inc c
{
	cpu& cp = cpu::instance();
	cp.set_c(inc_r8(cp.get_c()));

	return 1;
}

uint8_t arithmetic::inc_0x1c(cartridge& cart)//inc e
{
	cpu& cp = cpu::instance();
	cp.set_e(inc_r8(cp.get_e()));

	return 1;
}

uint8_t arithmetic::inc_0x2c(cartridge& cart)//inc l
{
	cpu& cp = cpu::instance();
	cp.set_l(inc_r8(cp.get_l()));

	return 1;
}

uint8_t arithmetic::inc_0x3c(cartridge& cart)//inc a
{
	cpu& cp = cpu::instance();
	cp.set_a(inc_r8(cp.get_a()));

	return 1;
}
//=====================================================================================
uint8_t load::ld_0x06(cartridge& cart)//ld b, n
{
	cpu& cp = cpu::instance();
	cp.set_b(read_imm8(cart));

	return 2;
}

uint8_t load::ld_0x16(cartridge& cart)//ld d, n
{
	cpu& cp = cpu::instance();
	cp.set_d(read_imm8(cart));

	return 2;
}

uint8_t load::ld_0x26(cartridge& cart)//ld h, n
{
	cpu& cp = cpu::instance();
	cp.set_h(read_imm8(cart));

	return 2;
}

uint8_t load::ld_0x36(cartridge& cart)//ld (hl), n - надо реализовать
{
	cpu& cp = cpu::instance();

	if(cp.get_hl() >= 0xFF40 && cp.get_hl() <= 0xFF4B)
		std::cout << "[LOAD] ld (hl), n: "
			<< "hl=0x" << std::hex << cp.get_hl()
			<< " val=0x" << (int)read_imm8(cart) << "\n";

	write_byte(cp.get_hl(), read_imm8(cart), cart);

	return 3;
}

uint8_t load::ld_0x0e(cartridge& cart)//ld c, n
{
	cpu& cp = cpu::instance();
	cp.set_c(read_imm8(cart));

	return 2;
}

uint8_t load::ld_0x1e(cartridge& cart)//ld e, n
{
	cpu& cp = cpu::instance();
	cp.set_e(read_imm8(cart));

	return 2;
}

uint8_t load::ld_0x2e(cartridge& cart)//ld l, n
{
	cpu& cp = cpu::instance();
	cp.set_l(read_imm8(cart));

	return 2;
}

uint8_t load::ld_0x3e(cartridge& cart)//ld a, n
{
	cpu& cp = cpu::instance();
	cp.set_a(read_imm8(cart));

	return 2;
}
//===================================================================
uint8_t flag::ccf_0x3f(cartridge& cart)
{
	cpu& cp = cpu::instance();
	uint8_t f = cp.get_f();
	// Инвертировать флаг C: если установлен, сбросить, иначе установить
	if (f & 0b00010000)
		f &= ~0b00010000;
	else
		f |= 0b00010000;
	// Сбросить флаги N и H
	f &= ~0b01100000;
	cp.set_f(f);

	return 1;
}

uint8_t flag::scf_0x37(cartridge& cart)
{
	cpu& cp = cpu::instance();
	uint8_t f = cp.get_f();
	f = (f & 0b10000000) | 0b00010000; // сохраняем Z, сбрасываем N и H, устанавливаем C
	cp.set_f(f);

	return 1;
}


uint8_t flag::cpl_0x2f(cartridge& cart)
{
	cpu& cp = cpu::instance();

	cp.set_a(~cp.get_a());
	uint8_t f = (cp.get_f() & 0b10010000) | 0b01100000;
	cp.set_f(f);

	return 1;
}

uint8_t flag::daa_0x27(cartridge& cart)//=================================================разобраться как работает
{
	cpu& cp = cpu::instance();
	uint8_t a = cp.get_a();
	uint8_t f = cp.get_f();
	uint8_t correction = 0;
	bool setC = false;

	// Если предыдущая операция была сложением (N = 0)
	if (!(f & 0x40)) 
	{
		if ((f & 0x20) || ((a & 0x0F) > 0x9)) correction += 0x06;

		if ((f & 0x10) || (a > 0x99)) 
		{
			correction += 0x60;
			setC = true;
		}
		a += correction;
	}
	else 
	{ // Если предыдущая операция была вычитанием (N = 1)
		if (f & 0x20) correction += 0x06;
		if (f & 0x10) correction += 0x60;

		a -= correction;
	}

	// Обновляем флаги:
	// Сбрасываем H
	f &= ~0x20;
	// Z: устанавливается, если результат равен 0
	if (a == 0) f |= 0x80;
	else f &= ~0x80;
	// N флаг остается неизменным
	// Устанавливаем флаг C согласно вычислению
	if (!(f & 0x40)) 
	{
		if (setC) f |= 0x10;
		else f &= ~0x10;
	}

	cp.set_a(a);
	cp.set_f(f);

	return 1;
}


uint8_t flag::rra_0x1f(cartridge& cart)
{
	cpu& cp = cpu::instance();
	uint8_t a = cp.get_a();
	uint8_t oldC = (cp.get_f() & 0x10) ? 1 : 0;
	uint8_t newC = a & 0x01;
	a = (a >> 1) | (oldC << 7);
	cp.set_a(a);
	cp.set_f(newC << 4); // Z, N, H = 0, C = newC

	return 1;
}


uint8_t flag::rla_0x17(cartridge& cart)
{
	cpu& cp = cpu::instance();
	uint8_t a = cp.get_a();
	uint8_t oldC = (cp.get_f() & 0x10) ? 1 : 0;
	uint8_t newC = (a & 0x80) >> 7;
	a = (a << 1) | oldC;
	cp.set_a(a);
	cp.set_f(newC << 4); // Z, N, H = 0, C = newC

	return 1;
}


uint8_t flag::rrca_0x0f(cartridge& cart)
{
	cpu& cp = cpu::instance();
	uint8_t a = cp.get_a();
	uint8_t newC = a & 0x01; // младший бит A
	a = (a >> 1) | (newC << 7);
	cp.set_a(a);
	cp.set_f(newC << 4);

	return 1;
}


uint8_t flag::rlca_0x07(cartridge& cart)
{
	cpu& cp = cpu::instance();
	uint8_t a = cp.get_a();
	uint8_t newC = (a & 0x80) >> 7; // старший бит A
	a = (a << 1) | newC;
	// Устанавливаем флаги: Z=0, N=0, H=0, C=newC
	cp.set_a(a);
	cp.set_f(newC << 4);

	return 1;
}

uint8_t flow::jr_0x18(cartridge& cart)
{
	cpu& cp = cpu::instance();
	int8_t e = read_imm8(cart);

	cp.set_pc(cp.get_pc() + e);

	return 3;
}
//========================================================================
static uint8_t cond_jr(cartridge& cart, bool expr)//jump при условии - может занимать на 1 машинный цикл меньше
{
	cpu& cp = cpu::instance();

	int8_t e = read_imm8(cart);
	if (expr)
	{
		cp.set_pc(cp.get_pc() + e);
		return 3;
	}

	return 2;
}

uint8_t flow::jr_0x20(cartridge& cart)// jr nz, e
{
	cpu& cp = cpu::instance();
	return cond_jr(cart, !(cp.get_f() & 0b10000000));
}

uint8_t flow::jr_0x30(cartridge& cart)// jr nc, e
{
	cpu& cp = cpu::instance();
	return cond_jr(cart, !(cp.get_f() & 0b00010000));
}

uint8_t flow::jr_0x28(cartridge& cart)// jr z, e
{
	cpu& cp = cpu::instance();
	return cond_jr(cart, cp.get_f() & 0b10000000);
}

uint8_t flow::jr_0x38(cartridge& cart)// jr c, e
{
	cpu& cp = cpu::instance();
	return cond_jr(cart, cp.get_f() & 0b00010000);
}

uint8_t flow::stop_0x10(cartridge& cart)//остановка cpu
{
	//std::cout << "Stop"<<std::endl;
	return 1;
}
//====================block1
uint8_t load::ld_r(cartridge& cart)//ld r, r'
{
	cpu& cp = cpu::instance();
	uint8_t ret = 1;

	auto get_reg = [&](uint8_t ind) 
	{
			if (ind == 0x00) return cp.get_b();
			else if (ind == 0x01) return cp.get_c();
			else if (ind == 0x02) return cp.get_d();
			else if (ind == 0x03) return cp.get_e();
			else if (ind == 0x04) return cp.get_h();
			else if (ind == 0x05) return cp.get_l();
			else if (ind == 0x06) { ret = 2; return read_byte(cp.get_hl(), cart);}//чтение по адресу
			else if (ind == 0x07) return cp.get_a();
			else throw std::runtime_error("no such reg");
	};
	auto set_reg = [&](uint8_t ind, uint8_t val)
	{
			if (ind == 0x00) cp.set_b(val);
			else if (ind == 0x01) cp.set_c(val);
			else if (ind == 0x02) cp.set_d(val);
			else if (ind == 0x03) cp.set_e(val);
			else if (ind == 0x04) cp.set_h(val);
			else if (ind == 0x05) cp.set_l(val);
			else if (ind == 0x06) { ret = 2; write_byte(cp.get_hl(), val, cart); }
			else if (ind == 0x07) cp.set_a(val);
			else throw std::runtime_error("no such reg");
	};

	uint8_t src = cp.get_ir() & 0x07;
	uint8_t dest = (cp.get_ir() >> 3) & 0b00000111;

	if (src == 0x06 && dest == 0x06) throw std::runtime_error("this should be halt");

	uint8_t value = get_reg(src);
	set_reg(dest, value);

	/*std::cout << "[LOAD] ld r, r': "
		<< "src=0x" << std::hex << (int)src
		<< " dest=0x" << (int)dest
		<< " value=0x" << (int)value << "\n";*/

	return ret;
}

//======================block2===============================================

uint8_t flow::halt_0x76(cartridge& cart)//halt bug не реализован
{
	cpu::instance().set_current_state(HALTED);
	return 1;
}

static void add_r8(uint8_t r8)
{
	cpu& cp = cpu::instance();
	uint8_t f = 0;

	uint16_t res = cp.get_a() + r8;
	f = ((res & 0xFF) == 0) ? 0b10000000 : f;
	if (res > 0xFF) f |= 0b00010000;
	if ((cp.get_a() & 0b1111) + (r8 & 0b1111) > 0b1111) f |= 0b00100000;

	cp.set_a(res & 0xFF);
	cp.set_f(f);
}

uint8_t arithmetic::add_0x80(cartridge& cart)//add a, b
{
	cpu& cp = cpu::instance();
	add_r8(cp.get_b());

	return 1;
}
uint8_t arithmetic::add_0x81(cartridge& cart)//add a, c
{
	cpu& cp = cpu::instance();
	add_r8(cp.get_c());

	return 1;
}
uint8_t arithmetic::add_0x82(cartridge& cart)//add a, d
{
	cpu& cp = cpu::instance();
	add_r8(cp.get_d());

	return 1;
}
uint8_t arithmetic::add_0x83(cartridge& cart)//add a, e
{
	cpu& cp = cpu::instance();
	add_r8(cp.get_e());

	return 1;
}
uint8_t arithmetic::add_0x84(cartridge& cart)//add a, h
{
	cpu& cp = cpu::instance();
	add_r8(cp.get_h());

	return 1;
}
uint8_t arithmetic::add_0x85(cartridge& cart)//add a, l
{
	cpu& cp = cpu::instance();
	add_r8(cp.get_l());

	return 1;
}
uint8_t arithmetic::add_0x86(cartridge& cart)//add a, (hl)
{
	cpu& cp = cpu::instance();
	add_r8(read_byte(cp.get_hl(), cart));

	return 2;
}
uint8_t arithmetic::add_0x87(cartridge& cart)//add a, a
{
	cpu& cp = cpu::instance();
	add_r8(cp.get_a());

	return 1;
}
//============

static void adc_r8(uint8_t r8)
{
	cpu& cp = cpu::instance();
	uint8_t f = 0;
	uint8_t c = (cp.get_f() >> 4) & 0b1; // получаем флаг переноса

	uint16_t res = cp.get_a() + r8 + c;
	f = ((res & 0xFF) == 0) ? 0b10000000 : f;
	if (res > 0xFF) f |= 0b00010000;
	if ((cp.get_a() & 0b1111) + (r8 & 0b1111) + c > 0b1111) f |= 0b00100000;

	cp.set_a(res & 0xFF);
	cp.set_f(f);
}

uint8_t arithmetic::adc_0x88(cartridge& cart)//adc a, b -- тут возможно переполнение, надо будет посмотреть
{
	cpu& cp = cpu::instance();
	adc_r8(cp.get_b());

	return 1;
}
uint8_t arithmetic::adc_0x89(cartridge& cart)//adc a, c
{
	cpu& cp = cpu::instance();
	adc_r8(cp.get_c());

	return 1;
}
uint8_t arithmetic::adc_0x8a(cartridge& cart)//adc a, d
{
	cpu& cp = cpu::instance();
	adc_r8(cp.get_d());

	return 1;
}
uint8_t arithmetic::adc_0x8b(cartridge& cart)//adc a, e
{
	cpu& cp = cpu::instance();
	adc_r8(cp.get_e());

	return 1;
}
uint8_t arithmetic::adc_0x8c(cartridge& cart)//adc a, h
{
	cpu& cp = cpu::instance();
	adc_r8(cp.get_h());

	return 1;
}
uint8_t arithmetic::adc_0x8d(cartridge& cart)//adc a, l
{
	cpu& cp = cpu::instance();
	adc_r8(cp.get_l());

	return 1;
}
uint8_t arithmetic::adc_0x8e(cartridge& cart)//adc a, (hl)
{
	cpu& cp = cpu::instance();
	adc_r8(read_byte(cp.get_hl(), cart));

	return 2;
}
uint8_t arithmetic::adc_0x8f(cartridge& cart)//adc a, a
{
	cpu& cp = cpu::instance();
	adc_r8(cp.get_a());

	return 1;
}
//=============
static void sub_r8(uint8_t r8)
{
	cpu& cp = cpu::instance();
	uint8_t f = 0;

	uint8_t a = cp.get_a();
	uint16_t res = a - r8;

	if ((res & 0xFF) == 0)
		f |= 0b10000000;

	f |= 0b01000000;

	if (r8 > a)
		f |= 0b00010000;
	if ((a & 0x0F) < (r8 & 0x0F))
		f |= 0b00100000;

	cp.set_a((uint8_t)res);
	cp.set_f(f);
}

uint8_t arithmetic::sub_0x90(cartridge& cart)//sub a, b
{
	cpu& cp = cpu::instance();
	sub_r8(cp.get_b());

	return 1;
}

uint8_t arithmetic::sub_0x91(cartridge& cart)//sub a, c
{
	cpu& cp = cpu::instance();
	sub_r8(cp.get_c());

	return 1;
}

uint8_t arithmetic::sub_0x92(cartridge& cart)//sub a, d
{
	cpu& cp = cpu::instance();
	sub_r8(cp.get_d());

	return 1;
}

uint8_t arithmetic::sub_0x93(cartridge& cart)//sub a, e
{
	cpu& cp = cpu::instance();
	sub_r8(cp.get_e());

	return 1;
}

uint8_t arithmetic::sub_0x94(cartridge& cart)//sub a, h
{
	cpu& cp = cpu::instance();
	sub_r8(cp.get_h());

	return 1;
}

uint8_t arithmetic::sub_0x95(cartridge& cart)//sub a, l
{
	cpu& cp = cpu::instance();
	sub_r8(cp.get_l());

	return 1;
}

uint8_t arithmetic::sub_0x96(cartridge& cart)//sub a, (hl)
{
	cpu& cp = cpu::instance();
	sub_r8(read_byte(cp.get_hl(), cart));

	return 2;
}

uint8_t arithmetic::sub_0x97(cartridge& cart)//sub a, a
{
	cpu& cp = cpu::instance();
	sub_r8(cp.get_a());

	return 1;
}
//=============================================================

static void sbc_r8(uint8_t r8)
{
	cpu& cp = cpu::instance();
	uint8_t f = 0;
	uint8_t c = (cp.get_f() >> 4) & 0b1; // получаем флаг переноса

	uint8_t a = cp.get_a();
	uint16_t res = a - r8 - c;

	if ((res & 0xFF) == 0) f |= 0b10000000;

	f |= 0b01000000;

	if (r8 + c > a) f |= 0b00010000;
	if ((a & 0x0F) < ((r8 & 0x0F) + c) ) f |= 0b00100000;

	cp.set_a((uint8_t)res);
	cp.set_f(f);
}

uint8_t arithmetic::sbc_0x98(cartridge& cart)//sbc a, b
{
	cpu& cp = cpu::instance();
	sbc_r8(cp.get_b());

	return 1;
}

uint8_t arithmetic::sbc_0x99(cartridge& cart)//sbc a, c
{
	cpu& cp = cpu::instance();
	sbc_r8(cp.get_c());

	return 1;
}

uint8_t arithmetic::sbc_0x9a(cartridge& cart)//sbc a, d
{
	cpu& cp = cpu::instance();
	sbc_r8(cp.get_d());

	return 1;
}

uint8_t arithmetic::sbc_0x9b(cartridge& cart)//sbc a, e
{
	cpu& cp = cpu::instance();
	sbc_r8(cp.get_e());

	return 1;
}

uint8_t arithmetic::sbc_0x9c(cartridge& cart)//sbc a, h
{
	cpu& cp = cpu::instance();
	sbc_r8(cp.get_h());

	return 1;
}

uint8_t arithmetic::sbc_0x9d(cartridge& cart)//sbc a, l
{
	cpu& cp = cpu::instance();
	sbc_r8(cp.get_l());

	return 1;
}

uint8_t arithmetic::sbc_0x9e(cartridge& cart)//sbc a, (hl)
{
	cpu& cp = cpu::instance();
	sbc_r8(read_byte(cp.get_hl(), cart));

	return 2;
}

uint8_t arithmetic::sbc_0x9f(cartridge& cart)//sbc a, a
{
	cpu& cp = cpu::instance();
	sbc_r8(cp.get_a());

	return 1;
}
//==============
static void and_r8(uint8_t r8)
{
	cpu& cp = cpu::instance();
	uint8_t res = cp.get_a() & r8;

	cp.set_a(res);

	uint8_t f = res == 0 ? 0b10100000 : 0b00100000;

	cp.set_f(f);
}

uint8_t logical::and_0xa0(cartridge& cart)//and a, b
{
	cpu& cp = cpu::instance();
	and_r8(cp.get_b());

	return 1;
}

uint8_t logical::and_0xa1(cartridge& cart)//and a, c
{
	cpu& cp = cpu::instance();
	and_r8(cp.get_c());

	return 1;
}

uint8_t logical::and_0xa2(cartridge& cart)//and a, d
{
	cpu& cp = cpu::instance();
	and_r8(cp.get_d());

	return 1;
}

uint8_t logical::and_0xa3(cartridge& cart)//and a, e
{
	cpu& cp = cpu::instance();
	and_r8(cp.get_e());

	return 1;
}

uint8_t logical::and_0xa4(cartridge& cart)//and a, h
{
	cpu& cp = cpu::instance();
	and_r8(cp.get_h());

	return 1;
}

uint8_t logical::and_0xa5(cartridge& cart)//and a, l
{
	cpu& cp = cpu::instance();
	and_r8(cp.get_l());

	return 1;
}

uint8_t logical::and_0xa6(cartridge& cart)//and a, (hl)
{
	cpu& cp = cpu::instance();
	and_r8(read_byte(cp.get_hl(), cart));

	return 2;
}

uint8_t logical::and_0xa7(cartridge& cart)//and a, a
{
	cpu& cp = cpu::instance();
	and_r8(cp.get_a());

	return 1;
}
//===================

static void xor_r8(uint8_t r8)
{
	cpu& cp = cpu::instance();
	uint8_t res = cp.get_a() ^ r8;

	cp.set_a(res);

	uint8_t f = res == 0 ? 0b10000000 : 0b00000000;

	cp.set_f(f);
}

uint8_t logical::xor_0xa8(cartridge& cart)//xor a, b
{
	cpu& cp = cpu::instance();
	xor_r8(cp.get_b());

	return 1;
}

uint8_t logical::xor_0xa9(cartridge& cart)//xor a, c
{
	cpu& cp = cpu::instance();
	xor_r8(cp.get_c());

	return 1;
}

uint8_t logical::xor_0xaa(cartridge& cart)//xor a, d
{
	cpu& cp = cpu::instance();
	xor_r8(cp.get_d());

	return 1;
}

uint8_t logical::xor_0xab(cartridge& cart)//xor a, e
{
	cpu& cp = cpu::instance();
	xor_r8(cp.get_e());

	return 1;
}

uint8_t logical::xor_0xac(cartridge& cart)//xor a, h
{
	cpu& cp = cpu::instance();
	xor_r8(cp.get_h());

	return 1;
}

uint8_t logical::xor_0xad(cartridge& cart)//xor a, l
{
	cpu& cp = cpu::instance();
	xor_r8(cp.get_l());

	return 1;
}

uint8_t logical::xor_0xae(cartridge& cart)//xor a, (hl)
{
	cpu& cp = cpu::instance();
	xor_r8(read_byte(cp.get_hl(), cart));

	return 2;
}

uint8_t logical::xor_0xaf(cartridge& cart)//xor a, a
{
	cpu& cp = cpu::instance();
	xor_r8(cp.get_a());

	return 1;
}
//===================

static void or_r8(uint8_t r8)
{
	cpu& cp = cpu::instance();
	uint8_t res = cp.get_a() | r8;

	cp.set_a(res);

	uint8_t f = res == 0 ? 0b10000000 : 0b00000000;

	cp.set_f(f);
}

uint8_t logical::or_0xb0(cartridge& cart)// or a, b
{
	cpu& cp = cpu::instance();
	or_r8(cp.get_b());

	return 1;
}
uint8_t logical::or_0xb1(cartridge& cart)// or a, c
{
	cpu& cp = cpu::instance();
	or_r8(cp.get_c());

	return 1;
}
uint8_t logical::or_0xb2(cartridge& cart)// or a, d
{
	cpu& cp = cpu::instance();
	or_r8(cp.get_d());

	return 1;
}
uint8_t logical::or_0xb3(cartridge& cart)// or a, e
{
	cpu& cp = cpu::instance();
	or_r8(cp.get_e());

	return 1;
}
uint8_t logical::or_0xb4(cartridge& cart)// or a, h
{
	cpu& cp = cpu::instance();
	or_r8(cp.get_h());

	return 1;
}
uint8_t logical::or_0xb5(cartridge& cart)// or a, l
{
	cpu& cp = cpu::instance();
	or_r8(cp.get_l());

	return 1;
}
uint8_t logical::or_0xb6(cartridge& cart)// or a, (hl)
{
	cpu& cp = cpu::instance();
	or_r8(read_byte(cp.get_hl(), cart));

	return 2;
}
uint8_t logical::or_0xb7(cartridge& cart)// or a, a
{
	cpu& cp = cpu::instance();
	or_r8(cp.get_a());

	return 1;
}

//==================

static void cp_r8(uint8_t r8)
{
	cpu& cp = cpu::instance();

	uint8_t a = cp.get_a();
	uint16_t res = a - r8;

	uint8_t f = 0;

	if (static_cast<uint8_t>(res) == 0)
		f |= 0b10000000;

	f |= 0b01000000;

	if (r8 > a)
		f |= 0b00010000;

	if ((a & 0x0F) < (r8 & 0x0F))
		f |= 0b00100000;

	cp.set_f(f);
}


uint8_t logical::cp_0xb8(cartridge& cart) //cp a, b
{
	cp_r8(cpu::instance().get_b());

	return 1;
}

uint8_t logical::cp_0xb9(cartridge& cart) //cp a, c
{
	cp_r8(cpu::instance().get_c());

	return 1;
}

uint8_t logical::cp_0xba(cartridge& cart) //cp a, d
{
	cp_r8(cpu::instance().get_d());

	return 1;
}

uint8_t logical::cp_0xbb(cartridge& cart) //cp a, e
{
	cp_r8(cpu::instance().get_e());

	return 1;
}

uint8_t logical::cp_0xbc(cartridge& cart) //cp a, h
{
	cp_r8(cpu::instance().get_h());

	return 1;
}

uint8_t logical::cp_0xbd(cartridge& cart) //cp a, l
{
	cp_r8(cpu::instance().get_l());

	return 1;
}

uint8_t logical::cp_0xbe(cartridge& cart) //cp a, (hl)
{
	cp_r8(read_byte(cpu::instance().get_hl(), cart));

	return 2;
}

uint8_t logical::cp_0xbf(cartridge& cart) //cp a, a
{
	cp_r8(cpu::instance().get_a());

	return 1;
}

//==================block3===============================

uint8_t arithmetic::add_0xc6(cartridge& cart)//add a, imm8
{
	cpu& cp = cpu::instance();
	uint8_t value = read_imm8(cart);
	add_r8(value);

	return 2;
}

uint8_t arithmetic::adc_0xce(cartridge& cart)//adc a, imm8
{
	cpu& cp = cpu::instance();
	uint8_t value = read_imm8(cart);
	adc_r8(value);

	return 2;
}

uint8_t arithmetic::sub_0xd6(cartridge& cart)//sub a, imm8
{
	cpu& cp = cpu::instance();
	uint8_t value = read_imm8(cart);
	sub_r8(value);

	return 2;
}

uint8_t arithmetic::sbc_0xde(cartridge& cart)//sbc a, imm8
{
	cpu& cp = cpu::instance();

	uint8_t value = read_imm8(cart);
	sbc_r8(value);

	return 2;
}

uint8_t logical::and_0xe6(cartridge& cart)//and a, imm8
{
	cpu& cp = cpu::instance();
	uint8_t value = read_imm8(cart);
	and_r8(value);

	return 2;
}

uint8_t logical::xor_0xee(cartridge& cart)//xor a, imm8
{
	cpu& cp = cpu::instance();
	uint8_t value = read_imm8(cart);
	xor_r8(value);

	return 2;
}

uint8_t logical::or_0xf6(cartridge& cart)//or a, imm8
{
	cpu& cp = cpu::instance();
	uint8_t value = read_imm8(cart);
	or_r8(value);

	return 2;
}

uint8_t logical::cp_0xfe(cartridge& cart)//cp a, imm8
{
	cpu& cp = cpu::instance();
	uint8_t value = read_imm8(cart);
	cp_r8(value);

	return 2;
}
//==============================================================
static uint8_t ret_cond(cartridge& cart, bool expr)
{
	cpu& cp = cpu::instance();
	if (expr)
	{
		uint8_t z = read_byte(cp.get_sp(), cart);//младшая часть
		cp.set_sp(cp.get_sp() + 1);

		uint8_t w = read_byte(cp.get_sp(), cart);//старшая часть
		cp.set_sp(cp.get_sp() + 1);

		uint16_t address = (static_cast<uint16_t>(w) << 8) | z;
		cp.set_pc(address);

		return 5;
	}

	return 2;
}

uint8_t flow::ret_0xc0(cartridge& cart)//ret nz - разное потребление машинных циклов
{
	uint8_t f = cpu::instance().get_f();
	return ret_cond(cart, !(f & 0b10000000));
}

uint8_t flow::ret_0xd0(cartridge& cart)//ret nc
{
	uint8_t f = cpu::instance().get_f();
	return ret_cond(cart, !(f & 0b00010000));
}

uint8_t flow::ret_0xc8(cartridge& cart)//ret z
{
	uint8_t f = cpu::instance().get_f();
	return ret_cond(cart, (f & 0b10000000));
}

uint8_t flow::ret_0xd8(cartridge& cart)//ret c
{
	uint8_t f = cpu::instance().get_f();
	return ret_cond(cart, (f & 0b00010000));
}

uint8_t flow::ret_0xc9(cartridge& cart)//ret
{
	ret_cond(cart, true);
	return 4;
}

uint8_t flow::reti_0xd9(cartridge& cart)//reti -- возможна ошибка!!!
{
	flow::ei_0xfb(cart);
	flow::ret_0xc9(cart);

	//std::cout << "RETI called - returning" << std::endl;
	return 4;
}
//======================================================================

static uint8_t jp_cond_imm16(cartridge& cart, bool expr)// cond jp, imm16
{
	uint16_t address = read_imm16(cart);
	if (expr)
	{
		cpu::instance().set_pc(address);
		return 4;
	}
	return 3;
}

uint8_t flow::jp_0xc2(cartridge& cart)// jp nz, imm16 ----------- также разное потребление машинных циклов(как и все в этой группе)
{
	uint8_t f = cpu::instance().get_f();
	return jp_cond_imm16(cart, !(f & 0b10000000));
}

uint8_t flow::jp_0xd2(cartridge& cart)// jp nc, imm16
{
	uint8_t f = cpu::instance().get_f();
	return jp_cond_imm16(cart, !(f & 0b00010000));
}

uint8_t flow::jp_0xca(cartridge& cart)// jp z, imm16
{
	uint8_t f = cpu::instance().get_f();
	return jp_cond_imm16(cart, (f & 0b10000000));
}

uint8_t flow::jp_0xda(cartridge& cart)// jp c, imm16
{
	uint8_t f = cpu::instance().get_f();
	return jp_cond_imm16(cart, (f & 0b00010000));
}

uint8_t flow::jp_0xc3(cartridge& cart)// jp imm16
{
	jp_cond_imm16(cart, true);
	return 4;
}

uint8_t flow::jp_0xe9(cartridge& cart)// jp hl
{
	cpu& cp = cpu::instance();
	cp.set_pc(cp.get_hl());

	return 1;
}
//========================

static uint8_t call_imm16(cartridge& cart, bool expr)//call cc
{
	cpu& cp = cpu::instance();

	uint16_t address = read_imm16(cart);
	if (expr)
	{
		cp.set_sp(cp.get_sp() - 1);
		write_byte(cp.get_sp(), cp.get_pc() >> 8, cart);
		
		cp.set_sp(cp.get_sp() - 1);
		write_byte(cp.get_sp(), cp.get_pc() & 0b11111111, cart);

		cp.set_pc(address);

		return 6;
	}
	return 3;
}

uint8_t flow::call_0xc4(cartridge& cart)// call nz
{
	uint8_t f = cpu::instance().get_f();
	return call_imm16(cart, !(f & 0b10000000));
}

uint8_t flow::call_0xd4(cartridge& cart)// call nc
{
	uint8_t f = cpu::instance().get_f();
	return call_imm16(cart, !(f & 0b00010000));
}

uint8_t flow::call_0xcc(cartridge& cart)// call z
{
	uint8_t f = cpu::instance().get_f();
	return call_imm16(cart, (f & 0b10000000));
}

uint8_t flow::call_0xdc(cartridge& cart)// call c
{
	uint8_t f = cpu::instance().get_f();
	return call_imm16(cart, (f & 0b00010000));
}

uint8_t flow::call_0xcd(cartridge& cart)//call imm16
{
	return call_imm16(cart, true);
}
//=================

static uint8_t rst(cartridge& cart, uint16_t address)//rst - то же что и call но с фикс. адресами
{
	cpu& cp = cpu::instance();

	cp.set_sp(cp.get_sp() - 1);
	write_byte(cp.get_sp(), cp.get_pc() >> 8, cart);

	cp.set_sp(cp.get_sp() - 1);
	write_byte(cp.get_sp(), cp.get_pc() & 0b11111111, cart);

	cp.set_pc(address);

	return 4;
}

uint8_t flow::rst_0xc7(cartridge& cart)//rst 0x00
{
	return rst(cart, 0x00);
}

uint8_t flow::rst_0xd7(cartridge& cart)//rst 0x10
{
	return rst(cart, 0x10);
}

uint8_t flow::rst_0xe7(cartridge& cart)//rst 0x20
{
	return rst(cart, 0x20);
}

uint8_t flow::rst_0xf7(cartridge& cart)//rst 0x30
{
	return rst(cart, 0x30);
}

uint8_t flow::rst_0xcf(cartridge& cart)//rst 0x08
{
	return rst(cart, 0x08);
}

uint8_t flow::rst_0xdf(cartridge& cart)//rst 0x18
{
	return rst(cart, 0x18);
}

uint8_t flow::rst_0xef(cartridge& cart)//rst 0x28
{
	return rst(cart, 0x28);
}

uint8_t flow::rst_0xff(cartridge& cart)//rst 0x38
{
	//std::cout << "RST 0x38 called - entering" << std::endl;
	return rst(cart, 0x38);
}

static uint16_t pop(cartridge& cart)//pop 
{
	cpu& cp = cpu::instance();

	uint8_t z = read_byte(cp.get_sp(), cart);
	cp.set_sp(cp.get_sp() + 1);

	uint8_t w = read_byte(cp.get_sp(), cart);
	cp.set_sp(cp.get_sp() + 1);

	return (static_cast<uint16_t>(w) << 8) | z;
}

//====================

//pop bc
uint8_t flow::pop_0xc1(cartridge& cart)
{
	cpu::instance().set_bc(pop(cart));
	return 3;
}

//pop de
uint8_t flow::pop_0xd1(cartridge& cart)
{
	cpu::instance().set_de(pop(cart));
	return 3;
}

//pop hl
uint8_t flow::pop_0xe1(cartridge& cart)
{
	cpu::instance().set_hl(pop(cart));
	return 3;
}

//pop af
uint8_t flow::pop_0xf1(cartridge& cart)
{
	uint16_t res = pop(cart);
	cpu::instance().set_a(res >> 8);
	cpu::instance().set_f(res & 0xF0);

	return 3;
}
//===================

static void push(cartridge& cart, uint16_t value)//push
{
	cpu& cp = cpu::instance();

	cp.set_sp(cp.get_sp() - 1);
	write_byte(cp.get_sp(), value >> 8, cart);

	cp.set_sp(cp.get_sp() - 1);
	write_byte(cp.get_sp(), value & 0b11111111, cart);
}

uint8_t flow::push_0xc5(cartridge& cart)//push bc
{
	push(cart, cpu::instance().get_bc());
	return 4;
}

uint8_t flow::push_0xd5(cartridge& cart)//push de
{
	push(cart, cpu::instance().get_de());
	return 4;
}

uint8_t flow::push_0xe5(cartridge& cart)//push hl
{
	push(cart, cpu::instance().get_hl());
	return 4;
}

uint8_t flow::push_0xf5(cartridge& cart)//push af
{
	cpu& cp = cpu::instance();

	uint8_t a = cp.get_a();
	uint8_t f = cp.get_f() & 0xf0;

	uint16_t value = (static_cast<uint16_t>(a) << 8) | f;
	push(cart, value);

	return 4;
}

//ld (c), a
uint8_t load::ldh_0xe2(cartridge& cart)
{
	cpu& cp = cpu::instance();
	write_byte(0xFF00 | cp.get_c(), cp.get_a(), cart);

	return 2;
}

//ld (n), a
uint8_t load::ldh_0xe0(cartridge& cart)
{
	cpu& cp = cpu::instance();

	uint8_t value = read_imm8(cart);
	write_byte(0xFF00 | value, cp.get_a(), cart);

	return 3;
}

//ld (nn), a
uint8_t load::ld_0xea(cartridge& cart)
{
	uint16_t value = read_imm16(cart);
	write_byte(value, cpu::instance().get_a(), cart);

	return 4;
}

//ld a, (c)
uint8_t load::ldh_0xf2(cartridge& cart)
{
	cpu::instance().set_a(read_byte(0xFF00 | cpu::instance().get_c(), cart));

	return 2;
}

//ld a, (n)
uint8_t load::ldh_0xf0(cartridge& cart)
{
	uint8_t value = read_imm8(cart);
	//std::cout << "ldh a, " << std::hex << (0xFF00 | value) << std::endl;
	cpu::instance().set_a(read_byte(0xFF00 | value, cart));

	return 3;
}

//ld a, (nn)
uint8_t load::ld_0xfa(cartridge& cart)
{
	uint16_t address = read_imm16(cart);
	cpu::instance().set_a(read_byte(address, cart));

	return 4;
}

//add sp, e
uint8_t arithmetic::add_0xe8(cartridge& cart)
{
	cpu& cp = cpu::instance();

	uint8_t value = read_imm8(cart);

	uint16_t sp = cp.get_sp();
	uint16_t result = sp + (int8_t)value;

	// Флаги
	uint8_t f = 0;// z и n сброшены автоматически!

	// Проверка half carry и carry для младшего байта
	if ((sp & 0xFF) + (value) > 0xFF) f |= 0b00010000;
	if ((sp & 0xF) + (value & 0xF) > 0xF) f |= 0b00100000;

	cp.set_sp(result);
	cp.set_f(f);

	return 4;
}

//ld hl, sp+e
uint8_t load::ld_0xf8(cartridge& cart)//===================================возможна ошибка!!!
{
	cpu& cp = cpu::instance();

	uint8_t value = read_imm8(cart);

	uint16_t sp = cp.get_sp();
	uint16_t result = sp + (int8_t)value;

	// Флаги
	uint8_t f = 0;// z и n сброшены автоматически!

	// Проверка half carry и carry для младшего байта
	if ((sp & 0xFF) + (value) > 0xFF) f |= 0b00010000;
	if ((sp & 0xF) + (value & 0xF) > 0xF) f |= 0b00100000;

	cp.set_hl(result);
	cp.set_f(f);

	return 3;
}

//ld sp, hl
uint8_t load::ld_0xf9(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_sp(cp.get_hl());

	return 2;
}

//di
uint8_t flow::di_0xf3(cartridge& cart)
{
	cpu::instance().set_ime(0);
	return 1;
}

//ei
uint8_t flow::ei_0xfb(cartridge& cart)//по идее нужно включать ime только после выполнения СЛЕДУЮЩЕЙ инструкции
{
	cpu::instance().set_ime(1);
	return 1;
}

//========================================block4=================================================================

static uint8_t rlc(uint8_t r8)
{
	uint8_t res = r8 << 1 | (r8 >> 7);

	uint8_t f = 0;
	if (res == 0) f |= 0b10000000;
	if (r8 >> 7) f |= 0b00010000;

	cpu::instance().set_f(f);
	return res;
}

//rlc b
uint8_t pref::rlc_0x00(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_b(rlc(cp.get_b()));

	return 2;
}

//rlc c
uint8_t pref::rlc_0x01(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_c(rlc(cp.get_c()));

	return 2;
}

//rlc d
uint8_t pref::rlc_0x02(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_d(rlc(cp.get_d()));

	return 2;
}

//rlc e
uint8_t pref::rlc_0x03(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_e(rlc(cp.get_e()));

	return 2;
}

//rlc h
uint8_t pref::rlc_0x04(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_h(rlc(cp.get_h()));

	return 2;
}

//rlc l
uint8_t pref::rlc_0x05(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_l(rlc(cp.get_l()));

	return 2;
}

//rlc (hl)
uint8_t pref::rlc_0x06(cartridge& cart)
{
	cpu& cp = cpu::instance();
	write_byte(cp.get_hl(), rlc(read_byte(cp.get_hl(), cart)), cart);

	return 4;
}

//rlc a
uint8_t pref::rlc_0x07(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_a(rlc(cp.get_a()));

	return 2;
}

//================

static uint8_t rrc(uint8_t r8)
{
	uint8_t res = r8 >> 1 | ( (r8 & 0b1) << 7);
	uint8_t f = 0;

	if (res == 0) f |= 0b10000000;
	if (r8 & 0b1) f |= 0b00010000;

	cpu::instance().set_f(f);

	return res;
}

//rrc b
uint8_t pref::rrc_0x08(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_b(rrc(cp.get_b()));

	return 2;
}

//rrc c
uint8_t pref::rrc_0x09(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_c(rrc(cp.get_c()));

	return 2;
}

//rrc d
uint8_t pref::rrc_0x0a(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_d(rrc(cp.get_d()));

	return 2;
}

//rrc e
uint8_t pref::rrc_0x0b(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_e(rrc(cp.get_e()));

	return 2;
}

//rrc h
uint8_t pref::rrc_0x0c(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_h(rrc(cp.get_h()));

	return 2;
}

//rrc l
uint8_t pref::rrc_0x0d(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_l(rrc(cp.get_l()));

	return 2;
}

//rrc (hl)
uint8_t pref::rrc_0x0e(cartridge& cart)
{
	cpu& cp = cpu::instance();
	write_byte(cp.get_hl(), rrc(read_byte(cp.get_hl(), cart)), cart);

	return 4;
}

//rrc a
uint8_t pref::rrc_0x0f(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_a(rrc(cp.get_a()));

	return 2;
}
//============

static uint8_t rl(uint8_t r8)
{
	cpu& cp = cpu::instance();
	
	uint8_t res = (r8 << 1) | ( (cp.get_f() & 0b00010000) >> 4);

	uint8_t f = 0;
	if (r8 & 0b10000000) f |= 0b00010000;
	if (res == 0) f |= 0b10000000;

	cpu::instance().set_f(f);

	return res;
}

//rl b
uint8_t pref::rl_0x10(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_b(rl(cp.get_b()));

	return 2;
}

//rl c
uint8_t pref::rl_0x11(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_c(rl(cp.get_c()));

	return 2;
}

//rl d
uint8_t pref::rl_0x12(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_d(rl(cp.get_d()));

	return 2;
}

//rl e
uint8_t pref::rl_0x13(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_e(rl(cp.get_e()));

	return 2;
}

//rl h
uint8_t pref::rl_0x14(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_h(rl(cp.get_h()));

	return 2;
}

//rl l
uint8_t pref::rl_0x15(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_l(rl(cp.get_l()));

	return 2;
}

//rl (hl)
uint8_t pref::rl_0x16(cartridge& cart)
{
	cpu& cp = cpu::instance();
	write_byte(cp.get_hl(), rl(read_byte(cp.get_hl(), cart)), cart);

	return 4;
}

//rl a
uint8_t pref::rl_0x17(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_a(rl(cp.get_a()));

	return 2;
}
//==========================
static uint8_t rr(uint8_t r8)
{
	cpu& cp = cpu::instance();

	uint8_t oldC = (cp.get_f() & 0x10) >> 4;
	uint8_t newC = r8 & 0x01;

	uint8_t res = (oldC << 7) | (r8 >> 1);

	uint8_t f = 0;
	if (res == 0)
		f |= 0x80; // Z = 1

	if (newC)
		f |= 0x10; // C = 1

	cp.set_f(f);
	return res;
}

// RR B (opcode CB 0x18)
uint8_t pref::rr_0x18(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_b(rr(cp.get_b()));

	return 2;
}

// RR C (opcode CB 0x19)
uint8_t pref::rr_0x19(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_c(rr(cp.get_c()));

	return 2;
}

// RR D (opcode CB 0x1A)
uint8_t pref::rr_0x1a(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_d(rr(cp.get_d()));

	return 2;
}

// RR E (opcode CB 0x1B)
uint8_t pref::rr_0x1b(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_e(rr(cp.get_e()));

	return 2;
}

// RR H (opcode CB 0x1C)
uint8_t pref::rr_0x1c(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_h(rr(cp.get_h()));

	return 2;
}

// RR L (opcode CB 0x1D)
uint8_t pref::rr_0x1d(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_l(rr(cp.get_l()));

	return 2;
}

// RR (HL) (opcode CB 0x1E)
uint8_t pref::rr_0x1e(cartridge& cart)
{
	cpu& cp = cpu::instance();
	uint8_t value = rr(read_byte(cp.get_hl(), cart));
	write_byte(cp.get_hl(), value, cart);

	return 4;
}

// RR A (opcode CB 0x1F)
uint8_t pref::rr_0x1f(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_a(rr(cp.get_a()));

	return 2;
}

//==================================
static uint8_t sla(uint8_t r8)//========================возможны ошибки
{
	uint8_t newC = (r8 >> 7) & 1;
	uint8_t res = r8 << 1;

	uint8_t f = 0;
	if (res == 0)
		f |= 0x80;
	if (newC)
		f |= 0x10;

	cpu::instance().set_f(f);
	return res;
}

// SLA B
uint8_t pref::sla_0x20(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_b(sla(cp.get_b()));

	return 2;
}

// SLA C
uint8_t pref::sla_0x21(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_c(sla(cp.get_c()));

	return 2;
}

// SLA D
uint8_t pref::sla_0x22(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_d(sla(cp.get_d()));

	return 2;
}

// SLA E
uint8_t pref::sla_0x23(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_e(sla(cp.get_e()));

	return 2;
}

// SLA H
uint8_t pref::sla_0x24(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_h(sla(cp.get_h()));

	return 2;
}

// SLA L
uint8_t pref::sla_0x25(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_l(sla(cp.get_l()));

	return 2;
}

// SLA (HL)
uint8_t pref::sla_0x26(cartridge& cart)
{
	cpu& cp = cpu::instance();
	uint8_t value = sla(read_byte(cp.get_hl(), cart));
	write_byte(cp.get_hl(), value, cart);

	return 4;
}

// SLA A
uint8_t pref::sla_0x27(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_a(sla(cp.get_a()));

	return 2;
}


//====================================

static uint8_t sra(uint8_t r8)//========================возможны ошибки
{
	uint8_t newC = r8 & 0b1;
	uint8_t res = (r8 >> 1) | (r8 &0b10000000);

	uint8_t f = 0;
	if (res == 0)
		f |= 0x80;
	if (newC)
		f |= 0x10;

	cpu::instance().set_f(f);
	return res;
}

//sra b
uint8_t pref::sra_0x28(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_b(sra(cp.get_b()));

	return 2;
}

//sra c
uint8_t pref::sra_0x29(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_c(sra(cp.get_c()));

	return 2;
}

//sra d
uint8_t pref::sra_0x2a(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_d(sra(cp.get_d()));

	return 2;
}

//sra e
uint8_t pref::sra_0x2b(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_e(sra(cp.get_e()));

	return 2;
}

//sra h
uint8_t pref::sra_0x2c(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_h(sra(cp.get_h()));

	return 2;
}

//sra l
uint8_t pref::sra_0x2d(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_l(sra(cp.get_l()));

	return 2;
}

//sra (hl)
uint8_t pref::sra_0x2e(cartridge& cart)
{
	cpu& cp = cpu::instance();
	write_byte(cp.get_hl(), sra(read_byte(cp.get_hl(), cart)), cart);

	return 4;
}

//sra a
uint8_t pref::sra_0x2f(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_a(sra(cp.get_a()));

	return 2;
}
//============================

static uint8_t swap(uint8_t r8)
{
	uint8_t res = (r8 << 4) | (r8 >> 4);
	uint8_t f = (res == 0 ? 0x80 : 0);
	cpu::instance().set_f(f);
	return res;
}

//swap b
uint8_t pref::swap_0x30(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_b(swap(cp.get_b()));

	return 2;
}

//swap c
uint8_t pref::swap_0x31(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_c(swap(cp.get_c()));

	return 2;
}

//swap d
uint8_t pref::swap_0x32(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_d(swap(cp.get_d()));

	return 2;
}

//swap e
uint8_t pref::swap_0x33(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_e(swap(cp.get_e()));

	return 2;
}

//swap h
uint8_t pref::swap_0x34(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_h(swap(cp.get_h()));

	return 2;
}

//swap l
uint8_t pref::swap_0x35(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_l(swap(cp.get_l()));

	return 2;
}

//swap (hl)
uint8_t pref::swap_0x36(cartridge& cart)
{
	cpu& cp = cpu::instance();
	write_byte(cp.get_hl(), swap(read_byte(cp.get_hl(), cart)), cart);

	return 4;
}

//swap a
uint8_t pref::swap_0x37(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_a(swap(cp.get_a()));

	return 2;
}

//==============================
static uint8_t srl(uint8_t r8)
{
	cpu& cp = cpu::instance();

	uint8_t newC = r8 & 0x01;
	uint8_t res = r8 >> 1;

	uint8_t f = 0;
	if (res == 0)
		f |= 0x80;
	if (newC)
		f |= 0x10;

	cp.set_f(f);
	return res;
}

// SRL B
uint8_t pref::srl_0x38(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_b(srl(cp.get_b()));

	return 2;
}

// SRL C
uint8_t pref::srl_0x39(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_c(srl(cp.get_c()));

	return 2;
}

// SRL D
uint8_t pref::srl_0x3a(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_d(srl(cp.get_d()));

	return 2;
}

// SRL E
uint8_t pref::srl_0x3b(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_e(srl(cp.get_e()));

	return 2;
}

// SRL H
uint8_t pref::srl_0x3c(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_h(srl(cp.get_h()));

	return 2;
}

// SRL L
uint8_t pref::srl_0x3d(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_l(srl(cp.get_l()));

	return 2;
}

// SRL (HL)
uint8_t pref::srl_0x3e(cartridge& cart)
{
	cpu& cp = cpu::instance();
	uint8_t value = srl(read_byte(cp.get_hl(), cart));
	write_byte(cp.get_hl(), value, cart);

	return 4;
}

// SRL A
uint8_t pref::srl_0x3f(cartridge& cart)
{
	cpu& cp = cpu::instance();
	cp.set_a(srl(cp.get_a()));

	return 2;
}
//=====================

//bit u3, r8
uint8_t pref::bit_0x40_0x7f(cartridge& cart)
{
	cpu& cp = cpu::instance();
	uint8_t ret = 2;

	auto select_bit = [&](uint8_t b) -> uint8_t 
	{
		return 1 << b;
	};

	auto test_bit = [&](uint8_t r, uint8_t mask)
	{
			if (r == 0) return cp.get_b() & mask;
			else if (r == 1) return cp.get_c() & mask;
			else if (r == 2) return cp.get_d() & mask;
			else if (r == 3) return cp.get_e() & mask;
			else if (r == 4) return cp.get_h() & mask;
			else if (r == 5) return cp.get_l() & mask;
			else if (r == 6) { ret = 3; return read_byte(cp.get_hl(), cart) & mask;}//3 машинных цикла а не 2!!!
			else if (r == 7) return cp.get_a() & mask;
			else throw std::runtime_error("invalid command");
	};

	uint8_t bit = (cp.get_ir() >> 3) & 0b111;
	uint8_t r8 = cp.get_ir() & 0b111;

	uint8_t mask = select_bit(bit);
	uint8_t res = test_bit(r8, mask);

	uint8_t f = 0b00100000 | (cp.get_f() & 0b00010000);//сохраняю c

	if (res == 0) f |= 0b10000000;

	cp.set_f(f);

	return ret;
}

//res u3, r8
uint8_t pref::res_0x80_0xbf(cartridge& cart)
{
	cpu& cp = cpu::instance();

	uint8_t ret = 2;

	auto select_bit = [&](uint8_t b) -> uint8_t 
	{
		return 1 << b;
	};

	auto set_bit = [&](uint8_t r, uint8_t mask)
	{
		if (r == 0) cp.set_b((cp.get_b() & mask));
		else if (r == 1) cp.set_c((cp.get_c() & mask));
		else if (r == 2) cp.set_d((cp.get_d() & mask));
		else if (r == 3) cp.set_e((cp.get_e() & mask));
		else if (r == 4) cp.set_h((cp.get_h() & mask));
		else if (r == 5) cp.set_l((cp.get_l() & mask));
		else if (r == 6) { ret = 4; write_byte(cp.get_hl(), read_byte(cp.get_hl(), cart) & mask, cart); }//4 машинных цикла а не 2!!!
		else if (r == 7) cp.set_a((cp.get_a() & mask));
		else throw std::runtime_error("invalid command");
	};

	uint8_t bit = (cp.get_ir() >> 3) & 0b111;
	uint8_t r8 = cp.get_ir() & 0b111;

	uint8_t mask = ~select_bit(bit);
	set_bit(r8, mask);

	return ret;
}

//set u3, r8
uint8_t pref::set_0xc0_0xff(cartridge& cart)
{
	cpu& cp = cpu::instance();
	uint8_t ret = 2;

	auto select_bit = [&](uint8_t b) -> uint8_t
	{
		return 1 << b;
	};

	auto set_bit = [&](uint8_t r, uint8_t mask)
	{
		if (r == 0) cp.set_b((cp.get_b() | mask));
		else if (r == 1) cp.set_c((cp.get_c() | mask));
		else if (r == 2) cp.set_d((cp.get_d() | mask));
		else if (r == 3) cp.set_e((cp.get_e() | mask));
		else if (r == 4) cp.set_h((cp.get_h() | mask));
		else if (r == 5) cp.set_l((cp.get_l() | mask));
		else if (r == 6) {ret = 4; write_byte(cp.get_hl(), read_byte(cp.get_hl(), cart) | mask, cart);}//3 машинных цикла а не 2!!!
		else if (r == 7) cp.set_a((cp.get_a() | mask));
		else throw std::runtime_error("invalid command");
	};

	uint8_t bit = (cp.get_ir() >> 3) & 0b111;
	uint8_t r8 = cp.get_ir() & 0b111;

	uint8_t mask = select_bit(bit);
	set_bit(r8, mask);
	
	return ret;
}