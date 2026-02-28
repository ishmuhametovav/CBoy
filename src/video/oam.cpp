#include"oam.h"

oam& oam::instance()
{
	static oam _oam;
	return _oam;
}

//возможно работает неправильно тк
//сейчас dma может считать память откуда угодно
void oam::set_dma(const uint8_t dma, cartridge& cart)
{
	//std::cout << "PPU mode: " << (int)gpu::instance().get_mode() << std::endl;

	for (uint16_t i = 0x00; i < 0xA0; i++)//160 байт
	{
		uint8_t value = read_byte((static_cast<uint16_t>(dma) << 8) | i, cart);
		//std::cout<<"[OAM DMA] Writing value 0x" << std::hex << (int)value 
		//	<< " to OAM address 0x" << std::hex << (0xFE00 + i) << std::endl;
		memory[i] = value;
	}
}