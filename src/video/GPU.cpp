#include "GPU.h"

/*gpu::gpu() : LCDC(0x91), STAT(0x85), SCY(0x0), SCX(0x0), LY(0x0), LYC(0x0), //--оригинал 
WX(0x0), WY(0x0), BGP(0xFC), OBP0(0xFF), OBP1(0xFF), mcycle_count(0x0)
{
	vram.resize(8 * 1024);

	for(uint16_t i = 0; i < frame.size(); i++)
		frame[i].fill(0);

	prev_stat_line.fill(false);
}*/

gpu::gpu() : LCDC(0x91), STAT(0x81), SCY(0x0), SCX(0x0), LY(0x90), LYC(0x0),
WX(0x0), WY(0x0), BGP(0xFC), OBP0(0xFF), OBP1(0xFF), mcycle_count(0x0)
{
	vram.resize(8 * 1024);

	for (uint16_t i = 0; i < frame.size(); i++)
		frame[i].fill(0);

	prev_stat_line.fill(false);
}

void gpu::process_oam()
{
	if(mcycle_count >= OAM / 4)
	{
		//std::cout << "[OAM SCAN] LY = " << (int)LY << std::endl;
		//переключение в transfer
		STAT = (STAT & 0b11111100) | 0x3;
		mcycle_count -= (OAM / 4);
	}
}

void gpu::process_transfer()
{
	if(mcycle_count >= TRANSFER / 4)
	{
		//переключаем ppu в mode 0
		STAT = STAT & 0b11111100;
		mcycle_count -= (TRANSFER / 4);
	}
}

void gpu::process_hblank(cartridge& cart)
{
	if (mcycle_count >= HBLANK / 4)
	{
		//выборка спрайтов================================

		//if (LY == 80) __debugbreak();//проверка отрисовки объектов

		fill_bg_line(buffer[LY], cart);
		fill_wndw_line(buffer[LY], cart);
		fill_sprite_line(buffer[LY], cart);

		LY++;

		//переключаем ppu в mode 1
		if(LY > 143) 
			STAT = (STAT & 0b11111100) | 0x1;//LY = 144, PPU_MODE = 1 => ложное срабатывание VBLANK
		//переключаем ppu в mode 2
		else STAT = (STAT & 0b11111100) | 0x2;

		mcycle_count -= (HBLANK / 4);
	}
}

//тут небольшая ошибка в коде
void gpu::process_vblank()
{
	if(mcycle_count >= VBLANK / 4)
	{
		handle_vblank_interruption();

		LY++;//LY теперь выше if условия

		if (LY > 153)
		{
			LY = 0;//отрисовка с первой строки
			//готовим кадр
			swap_buffers();
			STAT = (STAT & 0b11111100) | 0x2;
		}

		mcycle_count -= (VBLANK / 4);
	}
}

void gpu::fill_bg_line(std::array<uint8_t, 160>& line, cartridge& cart)
{
	line.fill(0);

	if (LCDC & 0b1)//если фон и окно включены
	{

		uint16_t tiles_needed = (160 + (SCX & 0x7) + 7) / 8;
		uint16_t start_col = (SCX / 8);
		uint16_t row = ((SCY + LY) & 0xFF) / 8;


		current_tile_map tm = LCDC & 0b1000 ? MAP_2 : MAP_1;
		addr_method mthd = LCDC & 0b10000 ? UNSIGNED : SIGNED;
		uint16_t tile_row = (SCY + LY) & 7;//какая строка тайла

		for (uint16_t i = 0; i < tiles_needed; i++)
		{
			uint16_t col = (start_col + i) % 32;
			uint8_t bg_tile_num = data_map.get_tile_num(tm, col, row);
			tile bg_tile = tile_data.get_tile(mthd, bg_tile_num);

			for (uint16_t j = 0; j < 8; j++)
			{
				uint8_t color = bg_tile.data[tile_row][j];
				color = BGP >> (color * 2) & 0b11;

				int16_t x = (i * 8) + j - (SCX & 0x7);
				if (x >= 0 && x < 160) line[x] = color;
			}
		}
	}
}

void gpu::fill_wndw_line(std::array<uint8_t, 160>& line, cartridge& cart)
{
	if ((LCDC & 0b1) && (LCDC & 0b100000)) 
	{
		//координаты (0;0) окна
		int16_t w_start_pos_x = WX - 7;
		int16_t w_start_pos_y = WY;

		if (w_start_pos_x >= 160 || LY < w_start_pos_y) return;

		uint16_t row = (LY - w_start_pos_y) / 8;

		//сколько тайлов отобразится на экран
		int16_t first_x = 0 > w_start_pos_x ? 0 : w_start_pos_x;
		int16_t last_x = 159 < (w_start_pos_x + 256 - 1) ? 159 : (w_start_pos_x + 256 - 1);
		uint16_t tiles_needed = (last_x - first_x + 8) / 8;

		current_tile_map tm = LCDC & 0b1000000 ? MAP_2 : MAP_1;
		addr_method mthd = LCDC & 0b10000 ? UNSIGNED : SIGNED;
		uint16_t tile_row = (LY - w_start_pos_y) & 7;//какая строка тайла

		for (uint16_t i = 0; i < tiles_needed; i++)
		{
			uint8_t w_tile_num = data_map.get_tile_num(tm, i, row);
			tile w_tile = tile_data.get_tile(mthd, w_tile_num);
			for (uint16_t j = 0; j < 8; j++)
			{
				uint8_t color = w_tile.data[tile_row][j];
				color = BGP >> (color * 2) & 0b11;

				int16_t x = (i * 8) + j + w_start_pos_x;
				if (x >= 0 && x < 160) line[x] = color;
			}
		}
	}
}

void gpu::fill_sprite_line(std::array<uint8_t, 160>& line, cartridge& cart)
{
	if (!(LCDC & 0b10)) return;

	std::array<sprite, 10> sprites;
	uint16_t i_sp = 0;

	//размерность спрайтов
	uint16_t height = LCDC & 0b100 ? 16 : 8;

	for (uint16_t i = 0; i < 40; i++)
	{
		sprite sp = sprite_data.get_sorted_sprite(i);

		int16_t y0 = (int16_t)sp.Y - 16;

		if (LY >= y0 && LY < ( y0 + height))//линия на спрайте
		{
			sprites[i_sp++] = sp;
		}
		if (i_sp == 10) break;
	}
	//std::sort(sprites.begin(), sprites.begin() + i_sp, [](sprite& a, sprite& b) { if (a.X != b.X) return a.X < b.X;
		//																	else return a.num < b.num; });
	
	//====================================================================

	for(int8_t i = i_sp - 1; i >= 0; i--)
	{
		if (sprites[i].flags >> 7) continue;
		//расчет строки спрайта с учетом Yflip
		uint8_t row_in_sprite = sprites[i].flags >> 6 ? 
			( height == 8 ? 7 - (LY - (sprites[i].Y - 16) ) 
				: 15 - (LY - (sprites[i].Y - 16)) )
			: (LY - (sprites[i].Y - 16));

		uint16_t tile_row = row_in_sprite & 0x7;

		//расчет id тайла при разной размерности
		uint16_t tile_id = height == 8 ? sprites[i].pattern :
			(row_in_sprite < 8 ? sprites[i].pattern & 0xFE : sprites[i].pattern | 0x01);

		tile spr_tile = tile_data.get_tile(UNSIGNED, tile_id);
		
		for(int j = 0; j < 8; j++)
		{
			//если цвет 0 то спрайт прозрачный
			uint8_t color = spr_tile.data[tile_row][j];

			if (color != 0) 
			{
				color = sprites[i].flags & 0x10
					? OBP1 >> (color * 2) & 0b11
					: OBP0 >> (color * 2) & 0b11;

				int16_t base_x = (int16_t)sprites[i].X - 8;
				int16_t x = (sprites[i].flags >> 5) & 0b1 ? base_x + (7 - j) : base_x + j;

			if (x >= 0 && x < 160)
				line[x] = color;
			}
		}
	}
}

inline void gpu::handle_vblank_interruption() const
{
	interrupt_registers& ir = interrupt_registers::instance();
	if (LY == 144) 
	{
		//устанавливаем флаг VBLANK
		ir.set_if(ir.get_if() | 0b1);
	}
}

void gpu::swap_buffers()
{
	frame = buffer;
}

gpu& gpu::instance()
{
	static gpu gp;
	return gp;
}

void gpu::cycle(int32_t mcycles, cartridge& cart)
{
	//включен ли LCD
	if (!(LCDC & 0b10000000)) 
	{
		LY = 0;
		mcycle_count = 0;
		STAT = (STAT & ~0b11) | 1;
		return; 
	}

	mcycle_count += mcycles;

	uint8_t ppu_mode = STAT & 0b11;

	//OAM сканирование
	if (ppu_mode == 2)
	{
		//std::cout << "oam scanning" << std::endl;
		process_oam();
	}
	//отрисовка 
	else if (ppu_mode == 3)
	{
		//std::cout << "transfer" << std::endl;
		process_transfer();
	}
	//HBLANK
	else if (ppu_mode == 0)
	{
		//std::cout << "hblank processing" << std::endl;
		process_hblank(cart);
	}
	//VBLANK
	else if (ppu_mode == 1)
	{
		//std::cout << "vblank processing" << std::endl;
		process_vblank();
	}

	handle_stat_interrupts();
}

void gpu::draw()
{
	renderer::instance().draw(frame);
}

uint8_t gpu::get_register(uint16_t address) const
{

	if (address == 0xFF40) return LCDC;
	else if (address == 0xFF41) return STAT;
	else if (address == 0xFF42) return SCY;
	else if (address == 0xFF43) return SCX;
	else if (address == 0xFF44) return LY;
	else if (address == 0xFF45) return LYC;
	else if (address == 0xFF46) return 0xFF;//чтение DMA возвращает 0xFF
	else if (address == 0xFF47) return BGP;
	else if (address == 0xFF48) return OBP0;
	else if (address == 0xFF49) return OBP1;
	else if (address == 0xFF4A) return WY;
	else if (address == 0xFF4B) return WX;

	return 0xFF;
}

void gpu::set_register(uint16_t address, uint8_t value)
{
	if (address == 0xFF40) LCDC = value;
	else if (address == 0xFF41) STAT = (STAT & 0b00000111) | (value & 0b1111000);
	else if (address == 0xFF42) SCY = value;
	else if (address == 0xFF43) SCX = value;
	else if (address == 0xFF44) 
	{ 
		//std::cout << "обнуляем LY" << std::endl;
		LY = 0; 
	}
	else if (address == 0xFF45) LYC = value;
	//обработаем случай DMA во внешнем обработчике
	else if (address == 0xFF47) BGP = value;
	else if (address == 0xFF48) OBP0 = value;
	else if (address == 0xFF49) OBP1 = value;
	else if (address == 0xFF4A) WY = value;
	else if (address == 0xFF4B) WX = value;

}

void gpu::set_vram(const uint16_t address, const uint8_t value)
{
	vram[address - 0x8000] = value;
}

std::string gpu::debug()
{
	std::stringstream log;
	log << "LCDC: " << std::hex << (unsigned)LCDC << " LY: " << (unsigned)LY
		<< " LYC: " << (unsigned)LYC << " STAT: " << (unsigned)STAT;

	return log.str();
}

void gpu::handle_stat_interrupts()
{
	uint8_t ppu_mode = STAT & 0b11;
	interrupt_registers& ir = interrupt_registers::instance();

	std::array<bool, 4> new_stat_line;
	new_stat_line[0] = (STAT & 0b1000) && (ppu_mode == 0); // HBLANK
	new_stat_line[1] = (STAT & 0b10000) && (ppu_mode == 1); // VBLANK
	new_stat_line[2] = (STAT & 0b100000) && (ppu_mode == 2); // OAM
	new_stat_line[3] = (STAT & 0b1000000) && (LYC == LY); // LYC

	//прерывание LYC 
	if (LY == LYC) STAT = STAT | 0b100;
	else STAT = STAT & 0b11111011;

	for(int i = 0; i < 4; i++)
	{
		if (new_stat_line[i] && !prev_stat_line[i]) 
		{
			//std::cout << "STAT interrupt: " << i << std::endl;
			ir.set_if(ir.get_if() | 0b10);
		}
		prev_stat_line[i] = new_stat_line[i];
	}

	//прерывание при mode 1 (VBLANK) - выполняется много раз за кадр
	//if (ppu_mode == 1 && LY == 144) 
		//ir.set_if(ir.get_if() | 0b1);
}

