#pragma once
#include<iostream>//для отладки
#include"./cartridge.h"
#include"./CPU.h"
#include"./video/GPU.h"
#include"./memory.h"
#include"./timer.h"
#include"./audio/APU.h"
#include"./input.h"
#include"./video/oam.h"

uint8_t read_byte(const uint16_t address, cartridge& cart);
void write_byte(const uint16_t address, const uint8_t value, cartridge& cart);
//block0

namespace arithmetic
{
	uint8_t add_0x09(cartridge& cart);
	uint8_t add_0x19(cartridge& cart);
	uint8_t add_0x29(cartridge& cart);
	uint8_t add_0x39(cartridge& cart);

	uint8_t dec_0x0b(cartridge& cart);
	uint8_t dec_0x1b(cartridge& cart);
	uint8_t dec_0x2b(cartridge& cart);
	uint8_t dec_0x3b(cartridge& cart);

	uint8_t inc_0x03(cartridge& cart);
	uint8_t inc_0x13(cartridge& cart);
	uint8_t inc_0x23(cartridge& cart);
	uint8_t inc_0x33(cartridge& cart);

	uint8_t dec_0x05(cartridge& cart);
	uint8_t dec_0x15(cartridge& cart);
	uint8_t dec_0x25(cartridge& cart);
	uint8_t dec_0x35(cartridge& cart);
	uint8_t dec_0x0d(cartridge& cart);
	uint8_t dec_0x1d(cartridge& cart);
	uint8_t dec_0x2d(cartridge& cart);
	uint8_t dec_0x3d(cartridge& cart);

	uint8_t inc_0x04(cartridge& cart);
	uint8_t inc_0x14(cartridge& cart);
	uint8_t inc_0x24(cartridge& cart);
	uint8_t inc_0x34(cartridge& cart);
	uint8_t inc_0x0c(cartridge& cart);
	uint8_t inc_0x1c(cartridge& cart);
	uint8_t inc_0x2c(cartridge& cart);
	uint8_t inc_0x3c(cartridge& cart);

	uint8_t add_0x80(cartridge& cart);
	uint8_t add_0x81(cartridge& cart);
	uint8_t add_0x82(cartridge& cart);
	uint8_t add_0x83(cartridge& cart);
	uint8_t add_0x84(cartridge& cart);
	uint8_t add_0x85(cartridge& cart);
	uint8_t add_0x86(cartridge& cart);
	uint8_t add_0x87(cartridge& cart);

	uint8_t adc_0x88(cartridge& cart);
	uint8_t adc_0x89(cartridge& cart);
	uint8_t adc_0x8a(cartridge& cart);
	uint8_t adc_0x8b(cartridge& cart);
	uint8_t adc_0x8c(cartridge& cart);
	uint8_t adc_0x8d(cartridge& cart);
	uint8_t adc_0x8e(cartridge& cart);
	uint8_t adc_0x8f(cartridge& cart);

	uint8_t sub_0x90(cartridge& cart);
	uint8_t sub_0x91(cartridge& cart);
	uint8_t sub_0x92(cartridge& cart);
	uint8_t sub_0x93(cartridge& cart);
	uint8_t sub_0x94(cartridge& cart);
	uint8_t sub_0x95(cartridge& cart);
	uint8_t sub_0x96(cartridge& cart);
	uint8_t sub_0x97(cartridge& cart);

	uint8_t sbc_0x98(cartridge& cart);
	uint8_t sbc_0x99(cartridge& cart);
	uint8_t sbc_0x9a(cartridge& cart);
	uint8_t sbc_0x9b(cartridge& cart);
	uint8_t sbc_0x9c(cartridge& cart);
	uint8_t sbc_0x9d(cartridge& cart);
	uint8_t sbc_0x9e(cartridge& cart);
	uint8_t sbc_0x9f(cartridge& cart);

	uint8_t add_0xc6(cartridge& cart);
	uint8_t adc_0xce(cartridge& cart);
	uint8_t sub_0xd6(cartridge& cart);
	uint8_t sbc_0xde(cartridge& cart);

	uint8_t add_0xe8(cartridge& cart);
}

namespace logical
{
	uint8_t and_0xa0(cartridge& cart);
	uint8_t and_0xa1(cartridge& cart);
	uint8_t and_0xa2(cartridge& cart);
	uint8_t and_0xa3(cartridge& cart);
	uint8_t and_0xa4(cartridge& cart);
	uint8_t and_0xa5(cartridge& cart);
	uint8_t and_0xa6(cartridge& cart);
	uint8_t and_0xa7(cartridge& cart);

	uint8_t xor_0xa8(cartridge& cart);
	uint8_t xor_0xa9(cartridge& cart);
	uint8_t xor_0xaa(cartridge& cart);
	uint8_t xor_0xab(cartridge& cart);
	uint8_t xor_0xac(cartridge& cart);
	uint8_t xor_0xad(cartridge& cart);
	uint8_t xor_0xae(cartridge& cart);
	uint8_t xor_0xaf(cartridge& cart);

	uint8_t or_0xb0(cartridge& cart);
	uint8_t or_0xb1(cartridge& cart);
	uint8_t or_0xb2(cartridge& cart);
	uint8_t or_0xb3(cartridge& cart);
	uint8_t or_0xb4(cartridge& cart);
	uint8_t or_0xb5(cartridge& cart);
	uint8_t or_0xb6(cartridge& cart);
	uint8_t or_0xb7(cartridge& cart);

	uint8_t cp_0xb8(cartridge& cart);
	uint8_t cp_0xb9(cartridge& cart);
	uint8_t cp_0xba(cartridge& cart);
	uint8_t cp_0xbb(cartridge& cart);
	uint8_t cp_0xbc(cartridge& cart);
	uint8_t cp_0xbd(cartridge& cart);
	uint8_t cp_0xbe(cartridge& cart);
	uint8_t cp_0xbf(cartridge& cart);

	uint8_t and_0xe6(cartridge& cart);
	uint8_t xor_0xee(cartridge& cart);
	uint8_t or_0xf6(cartridge& cart);
	uint8_t cp_0xfe(cartridge& cart);
}

namespace load
{
	uint8_t ld_0x08(cartridge& cart);

	uint8_t ld_0x0a(cartridge& cart);
	uint8_t ld_0x1a(cartridge& cart);
	uint8_t ld_0x2a(cartridge& cart);
	uint8_t ld_0x3a(cartridge& cart);

	uint8_t ld_0x02(cartridge& cart);
	uint8_t ld_0x12(cartridge& cart);
	uint8_t ld_0x22(cartridge& cart);
	uint8_t ld_0x32(cartridge& cart);

	uint8_t ld_0x01(cartridge& cart);
	uint8_t ld_0x11(cartridge& cart);
	uint8_t ld_0x21(cartridge& cart);
	uint8_t ld_0x31(cartridge& cart);

	uint8_t ld_0x06(cartridge& cart);
	uint8_t ld_0x16(cartridge& cart);
	uint8_t ld_0x26(cartridge& cart);
	uint8_t ld_0x36(cartridge& cart);
	uint8_t ld_0x0e(cartridge& cart);
	uint8_t ld_0x1e(cartridge& cart);
	uint8_t ld_0x2e(cartridge& cart);
	uint8_t ld_0x3e(cartridge& cart);

	uint8_t ld_r(cartridge& cart);

	uint8_t ldh_0xe2(cartridge& cart);
	uint8_t ldh_0xe0(cartridge& cart);
	uint8_t ld_0xea(cartridge& cart);
	uint8_t ldh_0xf2(cartridge& cart);
	uint8_t ldh_0xf0(cartridge& cart);
	uint8_t ld_0xfa(cartridge& cart);

	uint8_t ld_0xf8(cartridge& cart);
	uint8_t ld_0xf9(cartridge& cart);
}

namespace flow
{
	uint8_t nop(cartridge& cart);

	uint8_t jr_0x18(cartridge& cart);

	uint8_t jr_0x20(cartridge& cart);
	uint8_t jr_0x30(cartridge& cart);
	uint8_t jr_0x28(cartridge& cart);
	uint8_t jr_0x38(cartridge& cart);

	uint8_t stop_0x10(cartridge& cart);

	uint8_t halt_0x76(cartridge& cart);

	uint8_t ret_0xc0(cartridge& cart);
	uint8_t ret_0xd0(cartridge& cart);
	uint8_t ret_0xc8(cartridge& cart);
	uint8_t ret_0xd8(cartridge& cart);
	uint8_t ret_0xc9(cartridge& cart);
	uint8_t reti_0xd9(cartridge& cart);

	uint8_t jp_0xc2(cartridge& cart);
	uint8_t jp_0xd2(cartridge& cart);
	uint8_t jp_0xca(cartridge& cart);
	uint8_t jp_0xda(cartridge& cart);
	uint8_t jp_0xc3(cartridge& cart);
	uint8_t jp_0xe9(cartridge& cart);

	uint8_t call_0xc4(cartridge& cart);
	uint8_t call_0xd4(cartridge& cart);
	uint8_t call_0xcc(cartridge& cart);
	uint8_t call_0xdc(cartridge& cart);
	uint8_t call_0xcd(cartridge& cart);

	uint8_t rst_0xc7(cartridge& cart);
	uint8_t rst_0xd7(cartridge& cart);
	uint8_t rst_0xe7(cartridge& cart);
	uint8_t rst_0xf7(cartridge& cart);
	uint8_t rst_0xcf(cartridge& cart);
	uint8_t rst_0xdf(cartridge& cart);
	uint8_t rst_0xef(cartridge& cart);
	uint8_t rst_0xff(cartridge& cart);

	uint8_t pop_0xc1(cartridge& cart);
	uint8_t pop_0xd1(cartridge& cart);
	uint8_t pop_0xe1(cartridge& cart);
	uint8_t pop_0xf1(cartridge& cart);

	uint8_t push_0xc5(cartridge& cart);
	uint8_t push_0xd5(cartridge& cart);
	uint8_t push_0xe5(cartridge& cart);
	uint8_t push_0xf5(cartridge& cart);

	uint8_t di_0xf3(cartridge& cart);
	uint8_t ei_0xfb(cartridge& cart);
}

namespace flag
{
	uint8_t ccf_0x3f(cartridge& cart);
	uint8_t scf_0x37(cartridge& cart);
	uint8_t cpl_0x2f(cartridge& cart);
	uint8_t daa_0x27(cartridge& cart);
	uint8_t rra_0x1f(cartridge& cart);
	uint8_t rla_0x17(cartridge& cart);
	uint8_t rrca_0x0f(cartridge& cart);
	uint8_t rlca_0x07(cartridge& cart);
}

namespace pref
{
	uint8_t rlc_0x00(cartridge& cart);
	uint8_t rlc_0x01(cartridge& cart);
	uint8_t rlc_0x02(cartridge& cart);
	uint8_t rlc_0x03(cartridge& cart);
	uint8_t rlc_0x04(cartridge& cart);
	uint8_t rlc_0x05(cartridge& cart);
	uint8_t rlc_0x06(cartridge& cart);
	uint8_t rlc_0x07(cartridge& cart);

	uint8_t rrc_0x08(cartridge& cart);
	uint8_t rrc_0x09(cartridge& cart);
	uint8_t rrc_0x0a(cartridge& cart);
	uint8_t rrc_0x0b(cartridge& cart);
	uint8_t rrc_0x0c(cartridge& cart);
	uint8_t rrc_0x0d(cartridge& cart);
	uint8_t rrc_0x0e(cartridge& cart);
	uint8_t rrc_0x0f(cartridge& cart);

	uint8_t rl_0x10(cartridge& cart);
	uint8_t rl_0x11(cartridge& cart);
	uint8_t rl_0x12(cartridge& cart);
	uint8_t rl_0x13(cartridge& cart);
	uint8_t rl_0x14(cartridge& cart);
	uint8_t rl_0x15(cartridge& cart);
	uint8_t rl_0x16(cartridge& cart);
	uint8_t rl_0x17(cartridge& cart);

	uint8_t rr_0x18(cartridge& cart);
	uint8_t rr_0x19(cartridge& cart);
	uint8_t rr_0x1a(cartridge& cart);
	uint8_t rr_0x1b(cartridge& cart);
	uint8_t rr_0x1c(cartridge& cart);
	uint8_t rr_0x1d(cartridge& cart);
	uint8_t rr_0x1e(cartridge& cart);
	uint8_t rr_0x1f(cartridge& cart);

	uint8_t sla_0x20(cartridge& cart);
	uint8_t sla_0x21(cartridge& cart);
	uint8_t sla_0x22(cartridge& cart);
	uint8_t sla_0x23(cartridge& cart);
	uint8_t sla_0x24(cartridge& cart);
	uint8_t sla_0x25(cartridge& cart);
	uint8_t sla_0x26(cartridge& cart);
	uint8_t sla_0x27(cartridge& cart);

	uint8_t sra_0x28(cartridge& cart);
	uint8_t sra_0x29(cartridge& cart);
	uint8_t sra_0x2a(cartridge& cart);
	uint8_t sra_0x2b(cartridge& cart);
	uint8_t sra_0x2c(cartridge& cart);
	uint8_t sra_0x2d(cartridge& cart);
	uint8_t sra_0x2e(cartridge& cart);
	uint8_t sra_0x2f(cartridge& cart);

	uint8_t swap_0x30(cartridge& cart);
	uint8_t swap_0x31(cartridge& cart);
	uint8_t swap_0x32(cartridge& cart);
	uint8_t swap_0x33(cartridge& cart);
	uint8_t swap_0x34(cartridge& cart);
	uint8_t swap_0x35(cartridge& cart);
	uint8_t swap_0x36(cartridge& cart);
	uint8_t swap_0x37(cartridge& cart);

	uint8_t srl_0x38(cartridge& cart);
	uint8_t srl_0x39(cartridge& cart);
	uint8_t srl_0x3a(cartridge& cart);
	uint8_t srl_0x3b(cartridge& cart);
	uint8_t srl_0x3c(cartridge& cart);
	uint8_t srl_0x3d(cartridge& cart);
	uint8_t srl_0x3e(cartridge& cart);
	uint8_t srl_0x3f(cartridge& cart);

	uint8_t bit_0x40_0x7f(cartridge& cart);
	uint8_t res_0x80_0xbf(cartridge& cart);
	uint8_t set_0xc0_0xff(cartridge& cart);
}

//uint8_t prefix();

/*The following opcodes are invalid, and hard-lock the CPU until 
the console is powered off: $D3, $DB, $DD, $E3, $E4, $EB, $EC, $ED, $F4, $FC, and $FD.*/

//========================================block4

/*не реализованы:
inc_0x34
ld_0x36
stop_0x10
prefix -- нужно ли?
*/