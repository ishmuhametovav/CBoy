#include"instruction_set.h"

instruction_set::instruction_set() 
{
	// «аполнение первых 50 €чеек массива instructions:
	instructions[0x00] = { "nop", 1, flow::nop };
	instructions[0x01] = { "ld bc d16", 3, load::ld_0x01 };
	instructions[0x02] = { "ld (bc),a", 1, load::ld_0x02 };
	instructions[0x03] = { "inc bc", 1, arithmetic::inc_0x03 };
	instructions[0x04] = { "inc b", 1, arithmetic::inc_0x04 };
	instructions[0x05] = { "dec b", 1, arithmetic::dec_0x05 };
	instructions[0x06] = { "ld b,d8", 2, load::ld_0x06 };
	instructions[0x07] = { "rlca", 1, flag::rlca_0x07 };
	instructions[0x08] = { "ld (a16),sp", 3, load::ld_0x08 };
	instructions[0x09] = { "add hl,bc", 1, arithmetic::add_0x09 };
	instructions[0x0A] = { "ld a,(bc)", 1, load::ld_0x0a };
	instructions[0x0B] = { "dec bc", 1, arithmetic::dec_0x0b };
	instructions[0x0C] = { "inc c", 1, arithmetic::inc_0x0c };
	instructions[0x0D] = { "dec c", 1, arithmetic::dec_0x0d };
	instructions[0x0E] = { "ld c,d8", 2, load::ld_0x0e };
	instructions[0x0F] = { "rrca", 1, flag::rrca_0x0f };

	instructions[0x10] = { "stop", 2, flow::stop_0x10 };
	instructions[0x11] = { "ld de,d16", 3, load::ld_0x11 };
	instructions[0x12] = { "ld (de),a", 1, load::ld_0x12 };
	instructions[0x13] = { "inc de", 1, arithmetic::inc_0x13 };
	instructions[0x14] = { "inc d", 1, arithmetic::inc_0x14 };
	instructions[0x15] = { "dec d", 1, arithmetic::dec_0x15 };
	instructions[0x16] = { "ld d,d8", 2, load::ld_0x16 };
	instructions[0x17] = { "rla", 1, flag::rla_0x17 };

	instructions[0x18] = { "jr r8", 2, flow::jr_0x18 };
	instructions[0x19] = { "add hl,de", 1, arithmetic::add_0x19 };
	instructions[0x1A] = { "ld a,(de)", 1, load::ld_0x1a };
	instructions[0x1B] = { "dec de", 1, arithmetic::dec_0x1b };
	instructions[0x1C] = { "inc e", 1, arithmetic::inc_0x1c };
	instructions[0x1D] = { "dec e", 1, arithmetic::dec_0x1d };
	instructions[0x1E] = { "ld e,d8", 2, load::ld_0x1e };
	instructions[0x1F] = { "rra", 1, flag::rra_0x1f };

	instructions[0x20] = { "jr nz,r8", 2, flow::jr_0x20 };
	instructions[0x21] = { "ld hl,d16", 3, load::ld_0x21 };
	instructions[0x22] = { "ld (hl+),a", 1, load::ld_0x22 };
	instructions[0x23] = { "inc hl", 1, arithmetic::inc_0x23 };
	instructions[0x24] = { "inc h", 1, arithmetic::inc_0x24 };
	instructions[0x25] = { "dec h", 1, arithmetic::dec_0x25 };
	instructions[0x26] = { "ld h,d8", 2, load::ld_0x26 };
	instructions[0x27] = { "daa", 1, flag::daa_0x27 };

	instructions[0x28] = { "jr z,r8", 2, flow::jr_0x28 };
	instructions[0x29] = { "add hl,hl", 1, arithmetic::add_0x29 };
	instructions[0x2A] = { "ld a,(hl+)", 1, load::ld_0x2a };
	instructions[0x2B] = { "dec hl", 1, arithmetic::dec_0x2b };
	instructions[0x2C] = { "inc l", 1, arithmetic::inc_0x2c };
	instructions[0x2D] = { "dec l", 1, arithmetic::dec_0x2d };
	instructions[0x2E] = { "ld l,d8", 2, load::ld_0x2e };
	instructions[0x2F] = { "cpl", 1, flag::cpl_0x2f };

	instructions[0x30] = { "jr nc,r8", 2, flow::jr_0x30 };
	instructions[0x31] = { "ld sp,d16", 3, load::ld_0x31 };


	// «аполнение €чеек с opcode 0x32 до 0x3F (0x32 .. 0x3F = 14 €чеек)
	instructions[0x32] = { "ld (hl-),a", 1, load::ld_0x32 };
	instructions[0x33] = { "inc sp",     1, arithmetic::inc_0x33 };
	instructions[0x34] = { "inc (hl)",   1, arithmetic::inc_0x34 };
	instructions[0x35] = { "dec (hl)",   1, arithmetic::dec_0x35 };
	instructions[0x36] = { "ld (hl),d8",  2, load::ld_0x36 };
	instructions[0x37] = { "scf",        1, flag::scf_0x37 };
	instructions[0x38] = { "jr c,r8",    2, flow::jr_0x38 };
	instructions[0x39] = { "add hl,sp",  1, arithmetic::add_0x39 };
	instructions[0x3A] = { "ld a,(hl-)", 1, load::ld_0x3a };
	instructions[0x3B] = { "dec sp",     1, arithmetic::dec_0x3b };
	instructions[0x3C] = { "inc a",      1, arithmetic::inc_0x3c };
	instructions[0x3D] = { "dec a",      1, arithmetic::dec_0x3d };
	instructions[0x3E] = { "ld a,d8",    2, load::ld_0x3e };
	instructions[0x3F] = { "ccf",        1, flag::ccf_0x3f };

	// «аполнение €чеек с opcode 0x40 до 0x63 (0x40 .. 0x63 = 36 €чеек)
	// Ёто группа инструкций "LD r, r'" Ч дл€ всех них используем общий обработчик load::ld_r.
	instructions[0x40] = { "ld b,b", 1, load::ld_r };
	instructions[0x41] = { "ld b,c", 1, load::ld_r };
	instructions[0x42] = { "ld b,d", 1, load::ld_r };
	instructions[0x43] = { "ld b,e", 1, load::ld_r };
	instructions[0x44] = { "ld b,h", 1, load::ld_r };
	instructions[0x45] = { "ld b,l", 1, load::ld_r };
	instructions[0x46] = { "ld b,(hl)", 1, load::ld_r };
	instructions[0x47] = { "ld b,a", 1, load::ld_r };

	instructions[0x48] = { "ld c,b", 1, load::ld_r };
	instructions[0x49] = { "ld c,c", 1, load::ld_r };
	instructions[0x4A] = { "ld c,d", 1, load::ld_r };
	instructions[0x4B] = { "ld c,e", 1, load::ld_r };
	instructions[0x4C] = { "ld c,h", 1, load::ld_r };
	instructions[0x4D] = { "ld c,l", 1, load::ld_r };
	instructions[0x4E] = { "ld c,(hl)", 1, load::ld_r };
	instructions[0x4F] = { "ld c,a", 1, load::ld_r };

	instructions[0x50] = { "ld d,b", 1, load::ld_r };
	instructions[0x51] = { "ld d,c", 1, load::ld_r };
	instructions[0x52] = { "ld d,d", 1, load::ld_r };
	instructions[0x53] = { "ld d,e", 1, load::ld_r };
	instructions[0x54] = { "ld d,h", 1, load::ld_r };
	instructions[0x55] = { "ld d,l", 1, load::ld_r };
	instructions[0x56] = { "ld d,(hl)", 1, load::ld_r };
	instructions[0x57] = { "ld d,a", 1, load::ld_r };

	instructions[0x58] = { "ld e,b", 1, load::ld_r };
	instructions[0x59] = { "ld e,c", 1, load::ld_r };
	instructions[0x5A] = { "ld e,d", 1, load::ld_r };
	instructions[0x5B] = { "ld e,e", 1, load::ld_r };
	instructions[0x5C] = { "ld e,h", 1, load::ld_r };
	instructions[0x5D] = { "ld e,l", 1, load::ld_r };
	instructions[0x5E] = { "ld e,(hl)", 1, load::ld_r };
	instructions[0x5F] = { "ld e,a", 1, load::ld_r };

	instructions[0x60] = { "ld h,b", 1, load::ld_r };
	instructions[0x61] = { "ld h,c", 1, load::ld_r };
	instructions[0x62] = { "ld h,d", 1, load::ld_r };
	instructions[0x63] = { "ld h,e", 1, load::ld_r };

	
	// ќпкоды 0x64 Ч 0x67: LD H, r (оставшиес€ дл€ регистра H)
	instructions[0x64] = { "ld h,h",    1, load::ld_r };
	instructions[0x65] = { "ld h,l",    1, load::ld_r };
	instructions[0x66] = { "ld h,(hl)", 1, load::ld_r };
	instructions[0x67] = { "ld h,a",    1, load::ld_r };

	// ќпкоды 0x68 Ч 0x6F: LD L, r
	instructions[0x68] = { "ld l,b",    1, load::ld_r };
	instructions[0x69] = { "ld l,c",    1, load::ld_r };
	instructions[0x6A] = { "ld l,d",    1, load::ld_r };
	instructions[0x6B] = { "ld l,e",    1, load::ld_r };
	instructions[0x6C] = { "ld l,h",    1, load::ld_r };
	instructions[0x6D] = { "ld l,l",    1, load::ld_r };
	instructions[0x6E] = { "ld l,(hl)", 1, load::ld_r };
	instructions[0x6F] = { "ld l,a",    1, load::ld_r };

	// ќпкоды 0x70 Ч 0x7F: LD (hl), r и LD a, r
	instructions[0x70] = { "ld (hl),b", 1, load::ld_r };
	instructions[0x71] = { "ld (hl),c", 1, load::ld_r };
	instructions[0x72] = { "ld (hl),d", 1, load::ld_r };
	instructions[0x73] = { "ld (hl),e", 1, load::ld_r };
	instructions[0x74] = { "ld (hl),h", 1, load::ld_r };
	instructions[0x75] = { "ld (hl),l", 1, load::ld_r };
	instructions[0x76] = { "halt",      1, flow::halt_0x76 };
	instructions[0x77] = { "ld (hl),a", 1, load::ld_r };
	instructions[0x78] = { "ld a,b",    1, load::ld_r };
	instructions[0x79] = { "ld a,c",    1, load::ld_r };
	instructions[0x7A] = { "ld a,d",    1, load::ld_r };
	instructions[0x7B] = { "ld a,e",    1, load::ld_r };
	instructions[0x7C] = { "ld a,h",    1, load::ld_r };
	instructions[0x7D] = { "ld a,l",    1, load::ld_r };
	instructions[0x7E] = { "ld a,(hl)", 1, load::ld_r };
	instructions[0x7F] = { "ld a,a",    1, load::ld_r };

	// ќпкоды 0x80 Ч 0x87: ADD A, r
	instructions[0x80] = { "add a,b",    1, arithmetic::add_0x80 };
	instructions[0x81] = { "add a,c",    1, arithmetic::add_0x81 };
	instructions[0x82] = { "add a,d",    1, arithmetic::add_0x82 };
	instructions[0x83] = { "add a,e",    1, arithmetic::add_0x83 };
	instructions[0x84] = { "add a,h",    1, arithmetic::add_0x84 };
	instructions[0x85] = { "add a,l",    1, arithmetic::add_0x85 };
	instructions[0x86] = { "add a,(hl)", 1, arithmetic::add_0x86 };
	instructions[0x87] = { "add a,a",    1, arithmetic::add_0x87 };

	// ќпкоды 0x88 Ч 0x8F: ADC A, r
	instructions[0x88] = { "adc a,b",    1, arithmetic::adc_0x88 };
	instructions[0x89] = { "adc a,c",    1, arithmetic::adc_0x89 };
	instructions[0x8A] = { "adc a,d",    1, arithmetic::adc_0x8a };
	instructions[0x8B] = { "adc a,e",    1, arithmetic::adc_0x8b };
	instructions[0x8C] = { "adc a,h",    1, arithmetic::adc_0x8c };
	instructions[0x8D] = { "adc a,l",    1, arithmetic::adc_0x8d };
	instructions[0x8E] = { "adc a,(hl)", 1, arithmetic::adc_0x8e };
	instructions[0x8F] = { "adc a,a",    1, arithmetic::adc_0x8f };

	// ќпкоды 0x90 Ч 0x95: SUB A, r
	instructions[0x90] = { "sub b", 1, arithmetic::sub_0x90 };
	instructions[0x91] = { "sub c", 1, arithmetic::sub_0x91 };
	instructions[0x92] = { "sub d", 1, arithmetic::sub_0x92 };
	instructions[0x93] = { "sub e", 1, arithmetic::sub_0x93 };
	instructions[0x94] = { "sub h", 1, arithmetic::sub_0x94 };
	instructions[0x95] = { "sub l", 1, arithmetic::sub_0x95 };

	instructions[0x96] = { "sub (hl)", 1, arithmetic::sub_0x96 };
	instructions[0x97] = { "sub a",    1, arithmetic::sub_0x97 };

	// ќпкоды 0x98 Ч 0x9F: SBC A, r
	instructions[0x98] = { "sbc a,b",  1, arithmetic::sbc_0x98 };
	instructions[0x99] = { "sbc a,c",  1, arithmetic::sbc_0x99 };
	instructions[0x9A] = { "sbc a,d",  1, arithmetic::sbc_0x9a };
	instructions[0x9B] = { "sbc a,e",  1, arithmetic::sbc_0x9b };
	instructions[0x9C] = { "sbc a,h",  1, arithmetic::sbc_0x9c };
	instructions[0x9D] = { "sbc a,l",  1, arithmetic::sbc_0x9d };
	instructions[0x9E] = { "sbc a,(hl)", 1, arithmetic::sbc_0x9e };
	instructions[0x9F] = { "sbc a,a",  1, arithmetic::sbc_0x9f };

	// ќпкоды 0xA0 Ч 0xA7: AND r
	instructions[0xA0] = { "and b",    1, logical::and_0xa0 };
	instructions[0xA1] = { "and c",    1, logical::and_0xa1 };
	instructions[0xA2] = { "and d",    1, logical::and_0xa2 };
	instructions[0xA3] = { "and e",    1, logical::and_0xa3 };
	instructions[0xA4] = { "and h",    1, logical::and_0xa4 };
	instructions[0xA5] = { "and l",    1, logical::and_0xa5 };
	instructions[0xA6] = { "and (hl)", 1, logical::and_0xa6 };
	instructions[0xA7] = { "and a",    1, logical::and_0xa7 };

	// ќпкоды 0xA8 Ч 0xAF: XOR r
	instructions[0xA8] = { "xor b",    1, logical::xor_0xa8 };
	instructions[0xA9] = { "xor c",    1, logical::xor_0xa9 };
	instructions[0xAA] = { "xor d",    1, logical::xor_0xaa };
	instructions[0xAB] = { "xor e",    1, logical::xor_0xab };
	instructions[0xAC] = { "xor h",    1, logical::xor_0xac };
	instructions[0xAD] = { "xor l",    1, logical::xor_0xad };
	instructions[0xAE] = { "xor (hl)", 1, logical::xor_0xae };
	instructions[0xAF] = { "xor a",    1, logical::xor_0xaf };

	// ќпкоды 0xB0 Ч 0xB7: OR r
	instructions[0xB0] = { "or b",     1, logical::or_0xb0 };
	instructions[0xB1] = { "or c",     1, logical::or_0xb1 };
	instructions[0xB2] = { "or d",     1, logical::or_0xb2 };
	instructions[0xB3] = { "or e",     1, logical::or_0xb3 };
	instructions[0xB4] = { "or h",     1, logical::or_0xb4 };
	instructions[0xB5] = { "or l",     1, logical::or_0xb5 };
	instructions[0xB6] = { "or (hl)",  1, logical::or_0xb6 };
	instructions[0xB7] = { "or a",     1, logical::or_0xb7 };

	// ќпкоды 0xB8 Ч 0xBF: CP r
	instructions[0xB8] = { "cp b",     1, logical::cp_0xb8 };
	instructions[0xB9] = { "cp c",     1, logical::cp_0xb9 };
	instructions[0xBA] = { "cp d",     1, logical::cp_0xba };
	instructions[0xBB] = { "cp e",     1, logical::cp_0xbb };
	instructions[0xBC] = { "cp h",     1, logical::cp_0xbc };
	instructions[0xBD] = { "cp l",     1, logical::cp_0xbd };
	instructions[0xBE] = { "cp (hl)",  1, logical::cp_0xbe };
	instructions[0xBF] = { "cp a",     1, logical::cp_0xbf };

	// ќпкоды 0xC0 Ч 0xC7
	instructions[0xC0] = { "ret nz",      1, flow::ret_0xc0 };
	instructions[0xC1] = { "pop bc",      1, flow::pop_0xc1 };
	instructions[0xC2] = { "jp nz,a16",   3, flow::jp_0xc2 };
	instructions[0xC3] = { "jp a16",      3, flow::jp_0xc3 };
	instructions[0xC4] = { "call nz,a16", 3, flow::call_0xc4 };
	instructions[0xC5] = { "push bc",     1, flow::push_0xc5 };
	instructions[0xC6] = { "add a,d8",    2, arithmetic::add_0xc6 };
	instructions[0xC7] = { "rst 00h",     1, flow::rst_0xc7 };


	// 0xC8 Ц 0xCF
	instructions[0xC8] = { "ret z",       1, flow::ret_0xc8 };
	instructions[0xC9] = { "ret",         1, flow::ret_0xc9 };
	instructions[0xCA] = { "jp z,a16",    3, flow::jp_0xca };
	instructions[0xCB] = { "prefix cb",   1, flow::nop };  // особый случай
	instructions[0xCC] = { "call z,a16",  3, flow::call_0xcc };
	instructions[0xCD] = { "call a16",    3, flow::call_0xcd };
	instructions[0xCE] = { "adc a,d8",    2, arithmetic::adc_0xce };
	instructions[0xCF] = { "rst 08h",     1, flow::rst_0xcf };

	// 0xD0 Ц 0xD7
	instructions[0xD0] = { "ret nc",      1, flow::ret_0xd0 };
	instructions[0xD1] = { "pop de",      1, flow::pop_0xd1 };
	instructions[0xD2] = { "jp nc,a16",   3, flow::jp_0xd2 };
	instructions[0xD3] = { "nop (undoc)", 1, flow::nop }; // часто используетс€ заглушка
	instructions[0xD4] = { "call nc,a16", 3, flow::call_0xd4 };
	instructions[0xD5] = { "push de",     1, flow::push_0xd5 };
	instructions[0xD6] = { "sub d8",      2, arithmetic::sub_0xd6 };
	instructions[0xD7] = { "rst 10h",     1, flow::rst_0xd7 };

	// 0xD8 Ц 0xDF
	instructions[0xD8] = { "ret c",       1, flow::ret_0xd8 };
	instructions[0xD9] = { "reti",        1, flow::reti_0xd9 };
	instructions[0xDA] = { "jp c,a16",    3, flow::jp_0xda };
	instructions[0xDB] = { "nop (undoc)", 1, flow::nop };
	instructions[0xDC] = { "call c,a16",  3, flow::call_0xdc };
	instructions[0xDD] = { "nop (undoc)", 1, flow::nop };
	instructions[0xDE] = { "sbc a,d8",    2, arithmetic::sbc_0xde };
	instructions[0xDF] = { "rst 18h",     1, flow::rst_0xdf };

	// 0xE0 Ц 0xE7
	instructions[0xE0] = { "ldh (a8),a",  2, load::ldh_0xe0 };
	instructions[0xE1] = { "pop hl",      1, flow::pop_0xe1 };
	instructions[0xE2] = { "ld (c),a",    1, load::ldh_0xe2 };
	instructions[0xE3] = { "nop (undoc)", 1, flow::nop };
	instructions[0xE4] = { "nop (undoc)", 1, flow::nop };
	instructions[0xE5] = { "push hl",     1, flow::push_0xe5 };
	instructions[0xE6] = { "and d8",      2, logical::and_0xe6 };
	instructions[0xE7] = { "rst 20h",     1, flow::rst_0xe7 };

	// 0xE8 Ц 0xEF
	instructions[0xE8] = { "add sp,r8",   2, arithmetic::add_0xe8 };
	instructions[0xE9] = { "jp (hl)",     1, flow::jp_0xe9 };
	instructions[0xEA] = { "ld (a16),a",  3, load::ld_0xea };
	instructions[0xEB] = { "nop (undoc)", 1, flow::nop };
	instructions[0xEC] = { "nop (undoc)", 1, flow::nop };
	instructions[0xED] = { "nop (undoc)", 1, flow::nop };
	instructions[0xEE] = { "xor d8",      2, logical::xor_0xee };
	instructions[0xEF] = { "rst 28h",     1, flow::rst_0xef };

	// 0xF0 Ц 0xF1
	instructions[0xF0] = { "ldh a,(a8)",  2, load::ldh_0xf0 };
	instructions[0xF1] = { "pop af",      1, flow::pop_0xf1 };
	// 0xF2 Ц 0xFF
	instructions[0xF2] = { "ld a,(c)",     1, load::ldh_0xf2 };
	instructions[0xF3] = { "di",           1, flow::di_0xf3 };
	instructions[0xF4] = { "invalid",      1, flow::nop }; //- недокументированна€
	instructions[0xF5] = { "push af",      1, flow::push_0xf5 };
	instructions[0xF6] = { "or d8",        2, logical::or_0xf6 };
	instructions[0xF7] = { "rst 30h",      1, flow::rst_0xf7 };
	instructions[0xF8] = { "ld hl,sp+r8",  2, load::ld_0xf8 };
	instructions[0xF9] = { "ld sp,hl",     1, load::ld_0xf9 };
	instructions[0xFA] = { "ld a,(a16)",   3, load::ld_0xfa };
	instructions[0xFB] = { "ei",           1, flow::ei_0xfb };
	instructions[0xFC] = { "invalid",      1, flow::nop }; //- недокументированна€
	instructions[0xFD] = { "invalid",      1, flow::nop }; //- недокументированна€
	instructions[0xFE] = { "cp d8",        2, logical::cp_0xfe };
	instructions[0xFF] = { "rst 38h",      1, flow::rst_0xff };

	//===============================================
	// »ндексы 0x00 - 0x07: rlc
	pref_instructions[0x00] = { "rlc b",    1, pref::rlc_0x00 };
	pref_instructions[0x01] = { "rlc c",    1, pref::rlc_0x01 };
	pref_instructions[0x02] = { "rlc d",    1, pref::rlc_0x02 };
	pref_instructions[0x03] = { "rlc e",    1, pref::rlc_0x03 };
	pref_instructions[0x04] = { "rlc h",    1, pref::rlc_0x04 };
	pref_instructions[0x05] = { "rlc l",    1, pref::rlc_0x05 };
	pref_instructions[0x06] = { "rlc (hl)", 1, pref::rlc_0x06 };
	pref_instructions[0x07] = { "rlc a",    1, pref::rlc_0x07 };

	// »ндексы 0x08 - 0x0F: rrc
	pref_instructions[0x08] = { "rrc b",    1, pref::rrc_0x08 };
	pref_instructions[0x09] = { "rrc c",    1, pref::rrc_0x09 };
	pref_instructions[0x0A] = { "rrc d",    1, pref::rrc_0x0a };
	pref_instructions[0x0B] = { "rrc e",    1, pref::rrc_0x0b };
	pref_instructions[0x0C] = { "rrc h",    1, pref::rrc_0x0c };
	pref_instructions[0x0D] = { "rrc l",    1, pref::rrc_0x0d };
	pref_instructions[0x0E] = { "rrc (hl)", 1, pref::rrc_0x0e };
	pref_instructions[0x0F] = { "rrc a",    1, pref::rrc_0x0f };

	// »ндексы 0x10 - 0x17: rl
	pref_instructions[0x10] = { "rl b",    1, pref::rl_0x10 };
	pref_instructions[0x11] = { "rl c",    1, pref::rl_0x11 };
	pref_instructions[0x12] = { "rl d",    1, pref::rl_0x12 };
	pref_instructions[0x13] = { "rl e",    1, pref::rl_0x13 };
	pref_instructions[0x14] = { "rl h",    1, pref::rl_0x14 };
	pref_instructions[0x15] = { "rl l",    1, pref::rl_0x15 };
	pref_instructions[0x16] = { "rl (hl)", 1, pref::rl_0x16 };
	pref_instructions[0x17] = { "rl a",    1, pref::rl_0x17 };

	// »ндексы 0x18 - 0x1F: rr
	pref_instructions[0x18] = { "rr b",    1, pref::rr_0x18 };
	pref_instructions[0x19] = { "rr c",    1, pref::rr_0x19 };
	pref_instructions[0x1A] = { "rr d",    1, pref::rr_0x1a };
	pref_instructions[0x1B] = { "rr e",    1, pref::rr_0x1b };
	pref_instructions[0x1C] = { "rr h",    1, pref::rr_0x1c };
	pref_instructions[0x1D] = { "rr l",    1, pref::rr_0x1d };
	pref_instructions[0x1E] = { "rr (hl)", 1, pref::rr_0x1e };
	pref_instructions[0x1F] = { "rr a",    1, pref::rr_0x1f };

	// »ндексы 0x20 - 0x27: sla
	pref_instructions[0x20] = { "sla b",    1, pref::sla_0x20 };
	pref_instructions[0x21] = { "sla c",    1, pref::sla_0x21 };
	pref_instructions[0x22] = { "sla d",    1, pref::sla_0x22 };
	pref_instructions[0x23] = { "sla e",    1, pref::sla_0x23 };
	pref_instructions[0x24] = { "sla h",    1, pref::sla_0x24 };
	pref_instructions[0x25] = { "sla l",    1, pref::sla_0x25 };
	pref_instructions[0x26] = { "sla (hl)", 1, pref::sla_0x26 };
	pref_instructions[0x27] = { "sla a",    1, pref::sla_0x27 };

	// »ндексы 0x28 - 0x2F: sra
	pref_instructions[0x28] = { "sra b",    1, pref::sra_0x28 };
	pref_instructions[0x29] = { "sra c",    1, pref::sra_0x29 };
	pref_instructions[0x2A] = { "sra d",    1, pref::sra_0x2a };
	pref_instructions[0x2B] = { "sra e",    1, pref::sra_0x2b };
	pref_instructions[0x2C] = { "sra h",    1, pref::sra_0x2c };
	pref_instructions[0x2D] = { "sra l",    1, pref::sra_0x2d };
	pref_instructions[0x2E] = { "sra (hl)", 1, pref::sra_0x2e };
	pref_instructions[0x2F] = { "sra a",    1, pref::sra_0x2f };

	// »ндексы 0x30 - 0x31: первые 2 €чейки из группы swap
	pref_instructions[0x30] = { "swap b",   1, pref::swap_0x30 };
	pref_instructions[0x31] = { "swap c",   1, pref::swap_0x31 };
	// «аполнение €чеек с индексами 0x32 - 0x37 (остаток группы swap)
	pref_instructions[0x32] = { "swap d",    1, pref::swap_0x32 };
	pref_instructions[0x33] = { "swap e",    1, pref::swap_0x33 };
	pref_instructions[0x34] = { "swap h",    1, pref::swap_0x34 };
	pref_instructions[0x35] = { "swap l",    1, pref::swap_0x35 };
	pref_instructions[0x36] = { "swap (hl)", 1, pref::swap_0x36 };
	pref_instructions[0x37] = { "swap a",    1, pref::swap_0x37 };

	// «аполнение €чеек с индексами 0x38 - 0x3F (группа srl)
	pref_instructions[0x38] = { "srl b",    1, pref::srl_0x38 };
	pref_instructions[0x39] = { "srl c",    1, pref::srl_0x39 };
	pref_instructions[0x3A] = { "srl d",    1, pref::srl_0x3a };
	pref_instructions[0x3B] = { "srl e",    1, pref::srl_0x3b };
	pref_instructions[0x3C] = { "srl h",    1, pref::srl_0x3c };
	pref_instructions[0x3D] = { "srl l",    1, pref::srl_0x3d };
	pref_instructions[0x3E] = { "srl (hl)", 1, pref::srl_0x3e };
	pref_instructions[0x3F] = { "srl a",    1, pref::srl_0x3f };

	// «аполнение €чеек с индексами 0x40 - 0x63 (группа bit)
	// ƒл€ всех €чеек этой группы используем единый обработчик, который анализирует номер провер€емого бита,
	// а длина таких инструкций равна 2 байтам.
	for (uint16_t opcode = 0x40; opcode <= 0x7f; opcode++)
		pref_instructions[opcode] = { "bit u3,r8", 1, pref::bit_0x40_0x7f };

	for (uint16_t opcode = 0x80; opcode <= 0xbf; opcode++)
		pref_instructions[opcode] = { "res u3,r8", 1, pref::res_0x80_0xbf };

	for (uint16_t opcode = 0xc0; opcode <= 0xff; opcode++)
		pref_instructions[opcode] = { "set u3,r8", 1, pref::set_0xc0_0xff };
	}


instruction_set& instruction_set::instance()
{
	static instruction_set ins_set;
	return ins_set;
}
