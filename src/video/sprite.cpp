#include"./instructions/instruction.h"
#include"sprite.h"
void sprite::read_data(uint16_t address, cartridge& cart)
{
	this->Y = read_byte(address, cart);
	this->X = read_byte(address + 1, cart);
	this->pattern = read_byte(address + 2, cart);
	this->flags = read_byte(address + 3, cart);
	this->num = (address - 0xFE00) / 4;
}

void sprite_list::sort()
{
	std::sort(sorted.begin(), sorted.end(), [](sprite* a, sprite* b) { return a < b; });
}

void sprite_list::rebalance(sprite* spr)
{
	uint8_t pos = 0;
	while (pos < 40 && sorted[pos] != spr) pos++;

	while (pos + 1 < 40 && sorted[pos + 1] < sorted[pos])
	{
		std::swap(sorted[pos], sorted[pos + 1]);
		pos++;
	}
	while (pos > 0 && sorted[pos] < sorted[pos - 1])
	{
		std::swap(sorted[pos], sorted[pos - 1]);
		pos--;
	}
}

sprite_list::sprite_list()
{
	raw.fill({});
	for(uint8_t i = 0; i < 40; i++)
		sorted[i] = &raw[i];
}

void sprite_list::dma_write(cartridge& cart)
{
	for (uint16_t address = 0xFE00; address <= 0xFE9F; address += 4)
		raw[(address - 0xFE00) >> 2].read_data(address, cart);

	sort();
}

void sprite_list::update(uint16_t address, uint8_t value)
{
	uint8_t sprite_num = (address - 0xFE00) / 4;
	uint8_t byte_to_update = address - (0xFE00 + (sprite_num * 4));
	sprite* spr = &raw[sprite_num];
	if (byte_to_update == 0) spr->Y = value;
	if (byte_to_update == 2) spr->pattern = value;
	if (byte_to_update == 3) spr->flags = value;

	if(byte_to_update == 1)//если X координата обновилась
	{
		spr->X = value;

		rebalance(spr);
	}
}
