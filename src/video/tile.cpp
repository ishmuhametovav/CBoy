#include "tile.h"
#include"./instructions/instruction.h"
#include<iostream>

void tile::read_data(uint16_t address, cartridge& cart)
{
	for (uint16_t i = address; i < address + 16; i += 2)//ошибка вначале
	{
		uint8_t lo_byte = read_byte(i, cart);
		uint8_t hi_byte = read_byte(i + 1, cart);

		for(uint8_t j = 0; j < 8; j++)
		{
			uint8_t lo_bit = (lo_byte >> (7 - j)) & 0b1;
			uint8_t hi_bit = (hi_byte >> (7 - j)) & 0b1;
			data[(i - address) >> 1][j] = (hi_bit << 1) | lo_bit;
		}
	}
}

tile_list::tile_list()
{
	data.fill({});
}

tile tile_list::get_tile(addr_method mthd, uint8_t num)
{
	tile result;

	if(mthd == UNSIGNED)
	{
		result = data[num];
	}
	else if(mthd == SIGNED)
	{//если адресаци€ знакова€ то 2 блок от 128 до 255, 3 блок от 0 до 127
		if (num >= 128 && num <= 255) result = data[num];
		else if(num >= 0 && num <= 127) result = data[num + 256];
	}

	return result;
}

void tile_list::update(uint16_t address, cartridge& cart)//address - адрес измененной €чейки
{
	uint16_t tile_num = (address - 0x8000) / 16;//вычисл€ем номер тайла
	if (tile_num < 384) //провер€ем что номер тайла в пределах
	{
		data[tile_num].read_data((0x8000 + (tile_num * 16)), cart);
	}
	else
	{
		throw std::runtime_error("Tile number out of range: ");
	}
}

tile_map::tile_map()
{
	data1.fill({});
	data2.fill({});
}

void tile_map::update(uint16_t address, uint8_t value, cartridge& cart)
{
	if(address >= 0x9800 && address <= 0x9BFF)
	{
		uint16_t col = (address - 0x9800) % 32; //колонка
		uint16_t row = (address - 0x9800) / 32; //строка
		data1[row][col] = value; //обновл€ем карту тайлов 1
	}
	else if (address >= 0x9C00 && address <= 0x9FFF)
	{
		uint16_t col = (address - 0x9C00) % 32; //колонка
		uint16_t row = (address - 0x9C00) / 32; //строка
		data2[row][col] = value; //обновл€ем карту тайлов 2
	}
}

uint8_t tile_map::get_tile_num(current_tile_map m, uint8_t x, uint8_t y)
{
	if(m == MAP_1)
	{
		return data1[y][x];
	}
	else if(m == MAP_2)
	{
		return data2[y][x];
	}
}
