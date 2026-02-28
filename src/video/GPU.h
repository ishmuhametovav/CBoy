#pragma once
#include<cstdint>
#include<vector>
#include<array>
#include<algorithm>
#include"renderer.h"
#include"./instructions/instruction.h"
#include"tile.h"
#include"sprite.h"
#include"./memory.h"

constexpr int32_t OAM = 80;
constexpr int32_t TRANSFER = 172;
constexpr int32_t HBLANK = 204;
constexpr int32_t VBLANK = 456;
constexpr uint8_t NOT_VISIBLE = 0xFF;



//у окна есть свой счетчик наподобе LY, но он увеличивается только 
//когда окно отображается
class gpu
{
	gpu();
	~gpu(){ }
	gpu(const gpu&) = delete;
	gpu& operator = (const gpu&) = delete;

	//видеопамять
	std::vector<uint8_t> vram;
	//основной контроллер дисплея
	uint8_t LCDC;
	//текущая строка
	uint8_t LY;
	//строка для сравнения
	uint8_t LYC;
	//статус LCD если PPU выключен бит 0-1 всегда 0
	uint8_t STAT;
	//координаты окна (WX = 7 WY = 0 - левый верхний угол)
	uint8_t WX, WY;
	//координаты фона
	uint8_t SCX, SCY;
	//регистр палитры фона/окна
	uint8_t BGP;
	//регистры палитр для спрайтов
	uint8_t OBP0, OBP1;
	//количество циклов
	int32_t mcycle_count;
	//предыдущие состояния линии STAT
	std::array<bool, 4> prev_stat_line;
	//список тайлов
	tile_list tile_data;
	//карты тайлов
	tile_map data_map;
	//список спрайтов
	sprite_list sprite_data;
	std::array<std::array<uint8_t, 160>, 144> frame;
	std::array<std::array<uint8_t, 160>, 144> buffer;

	void handle_stat_interrupts();
	void process_oam();
	void process_transfer();
	void process_hblank(cartridge& cart);
	void process_vblank();

	void fill_bg_line(std::array<uint8_t, 160>& line, cartridge& cart);
	void fill_wndw_line(std::array<uint8_t, 160>& line, cartridge& cart);
	void fill_sprite_line(std::array<uint8_t, 160>& line, cartridge& cart);
	inline void handle_vblank_interruption() const;
	void swap_buffers();
public:
	static gpu& instance();

	void cycle(int32_t mcycles, cartridge& cart);
	void draw();
	//геттеры и сеттеры для регистров

	uint8_t get_register(uint16_t address) const;
	void set_register(uint16_t address, uint8_t value);

	inline uint8_t get_vram(const uint16_t address) { return vram[address - 0x8000]; }
	void set_vram(const uint16_t address, const uint8_t value);

	inline uint8_t get_mode() const { return STAT & 0b11; }
	inline tile_list& get_tile_list() { return tile_data; }
	inline tile_map& get_tile_map() { return data_map; }
	inline sprite_list& get_sprite_list() { return sprite_data; }

	std::string debug();
};