#pragma once
#include<array>
#include<algorithm>
//спрайты хранятся в таблице данных тайлов
//(с 0x8000) но обладают беззнаковой нумерацией
struct sprite
{
	uint8_t Y;
	uint8_t X;
	//номер шаблона($8000–8FFF, unsigned)
	uint8_t pattern;
	//7 - приоритет 6 - отображение по вертикали
	//5 - отображение по горизонтали 4 - номер палитры(1 - obj1pal, 0 - obj0pal)
	uint8_t flags;
	//номер в памяти
	uint16_t num;
	void read_data(uint16_t address, cartridge& cart);
	inline bool operator < (const sprite& other) const { if (X != other.X) return X < other.X; return num < other.num; }
};

class sprite_list
{
	std::array<sprite, 40> raw;
	std::array<sprite*, 40> sorted;
	void sort();
	void rebalance(sprite* spr);
public:
	sprite_list();
	void dma_write(cartridge& cart);
	void update(uint16_t address, uint8_t value);
	inline sprite get_sprite(uint8_t spr_num) const { return raw[spr_num]; }
	inline sprite get_sorted_sprite(uint8_t spr_num) const { return *sorted[spr_num]; }
};