#pragma once
#include<array>
#include"./cartridge.h"

enum addr_method { SIGNED, UNSIGNED };
enum current_tile_map { MAP_1, MAP_2 };

struct tile
{
	std::array<std::array<uint8_t, 8>, 8> data;
	void read_data(uint16_t address, cartridge& cart);
};

class tile_list
{
	std::array<tile, 384> data;
public:
	tile_list();
	tile get_tile(addr_method mthd, uint8_t num);
	void update(uint16_t address, cartridge& cart);
};

class tile_map
{
	std::array<std::array<uint8_t, 32>, 32> data1;
	std::array<std::array<uint8_t, 32>, 32> data2;
public:
	tile_map();
	void update(uint16_t address, uint8_t value, cartridge& cart);
	uint8_t get_tile_num(current_tile_map m, uint8_t x, uint8_t y);
};