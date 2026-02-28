#include "cartridge.h"

mbc1::mbc1() : RAMG(0b00000000), BANK1(0b00000001), BANK2(0b00000000), MODE(0b00000000)
{
}

/*uint8_t mbc1::get_rom_value(const uint16_t address, const std::vector<uint8_t>& rom) const
{
    uint16_t bank_size = 16 * 1024; //один rom bank для mbc1 - 16 kib

	if(address >= 0x0000 && address <= 0x3FFF)
	{
        if (MODE == 0b00000000) return rom[address];
        else if (MODE == 0b00000001) return rom[(BANK2 << 5) * bank_size + address];
	}
    else if (address >= 0x4000 && address <= 0x7FFF)
    {
        uint16_t offset = 0x4000;
        return rom[(BANK2 << 5 | BANK1) * bank_size + (address - offset)];
    }
    return 0xFF;
    //throw std::runtime_error("incorrect address");
}*/

uint8_t mbc1::get_rom_value(const uint16_t address, const std::vector<uint8_t>& rom) const 
{
    const uint32_t bank_size = 16 * 1024;
    const uint32_t max_banks = rom.size() / bank_size;
    if (address <= 0x3FFF) 
    {
        if (MODE == 0) return rom[address]; // Фиксированный банк 0
        else 
        {
            // Режим 1: применяем BANK2 к области 0000–3FFF
            uint32_t bank = (BANK2 << 5) % max_banks;
            return rom[bank * bank_size + address];
        }
    }
    else if (address <= 0x7FFF) 
    {
        // Область 4000–7FFF: комбинированный номер банка
        uint32_t bank = ((BANK2 << 5) | BANK1) % max_banks;
        return rom[bank * bank_size + (address - 0x4000)];
    }
    return 0xFF;
}

void mbc1::set_register(const uint16_t address, const uint8_t value)
{
    if(address >= 0x0000 && address <= 0x1FFF)
    {
        RAMG = value & 0b1111;//если записан 0xA то нужно включить SRAM
    }
    else if(address >= 0x2000 && address <= 0x3FFF)
    {
        /*if (value == 0) BANK1 = 0b00000001;//bank1 не может принимать 0
        else BANK1 = value & 0b11111;*/
        uint8_t new_bank = value & 0x1F;       // берём только младшие 5 бит
        if (new_bank == 0) new_bank = 1;                     // банк 0 -> 1
        BANK1 = new_bank;
    }
    else if(address >= 0x4000 && address <= 0x5FFF)
    {
        BANK2 = value & 0b11;
    }
    else if(address >= 0x6000 && address <= 0x7FFF)
    {
        MODE = value & 0b1;
    }
    else throw std::runtime_error("incorrect address");
}

uint8_t mbc1::get_ram_value(const uint16_t address, const std::vector<uint8_t>& ram) const
{
    if (address < 0xA000 || address > 0xBFFF) throw std::runtime_error("incorrect address");

    if(RAMG == 0xA)//если доступ в память разрешен
    {
        uint16_t offset = 0xA000;

        if (MODE == 0b00000001) 
        {
            //bank2 -номер банка, <<13 - умножение на 8192 байта
            uint16_t current_address = ((BANK2 & 0b11) << 13) + (address - offset);
            return ram[current_address]; 
        }
        else return ram[address - offset];
    }
    else
    {
        return 0xFF;
    }
}

void mbc1::set_ram_value(const uint16_t address, const uint8_t value, std::vector<uint8_t>& ram) const
{
    if (address < 0xA000 || address > 0xBFFF) throw std::runtime_error("incorrect address");

    if (RAMG == 0xA)//если доступ в память разрешен
    {
        uint16_t offset = 0xA000;

        if (MODE == 0b00000001)
        {
            //bank2 -номер банка, <<13 - умножение на 8192 байта
            uint16_t current_address = ((BANK2 & 0b11) << 13) + (address - offset);
            ram[current_address] = value;
        }
        else ram[address - offset] = value;
    }//если в память нет доступа запись ни к чему не приведет
}

cartridge::cartridge(const std::string path)
{
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) throw std::runtime_error("no such file");

    const std::streamsize size = ifs.tellg();

    ifs.seekg(0, std::ios::beg);

    rom.resize(size);
    ifs.read(reinterpret_cast<char*>(rom.data()), size);

    if (!ifs) throw std::runtime_error("error during file reading");

    ifs.close();

    uint8_t control_sum = 0;

    for (uint16_t i = 0x0134; i <= 0x014C; i++)
        control_sum = control_sum - rom[i] - 1;

    if (control_sum != rom[0x14D]) throw std::runtime_error("cartridge is damaged");

    uint8_t cartridge_type = rom[0x0147];

    if (cartridge_type == 0x01 || cartridge_type == 0x02 || cartridge_type == 0x03)
    {
        chip = new mbc1();

        if (rom[0x0149] == 0x00) ram.resize(0);
        if (rom[0x0149] == 0x02) ram.resize(8 * 1024);
        if (rom[0x0149] == 0x03) ram.resize(32 * 1024);
        if (rom[0x0149] == 0x04) ram.resize(128 * 1024);
        if (rom[0x0149] == 0x05) ram.resize(64 * 1024);
    }
	else if (cartridge_type == 0x00)
    {
        chip = new mbc0();
		ram.resize(0); // MBC0 не поддерживает RAM
    }
    else throw std::runtime_error("unsupported cartridge type");
}

uint8_t cartridge::get_rom_value(const uint16_t address) const
{
    return chip->get_rom_value(address, rom);
}

void cartridge::set_mbc_register(const uint16_t address, const uint8_t value)
{
    chip->set_register(address, value);
}

uint8_t cartridge::get_ram_value(const uint16_t address) const
{
    return chip->get_ram_value(address, ram);
}

void cartridge::set_ram_value(const uint16_t address, const uint8_t value)
{
    chip->set_ram_value(address, value, ram);
}

uint8_t mbc0::get_rom_value(const uint16_t address, const std::vector<uint8_t>& rom) const
{
	size_t size = rom.size();
    if (address >= size) return 0xFF;
	return rom[address];
}

void mbc0::set_register(const uint16_t address, const uint8_t value)
{
}

uint8_t mbc0::get_ram_value(const uint16_t address, const std::vector<uint8_t>& ram) const
{
	return 0xFF; // MBC0 не поддерживает RAM, всегда возвращаем 0xFF
}

void mbc0::set_ram_value(const uint16_t address, const uint8_t value, std::vector<uint8_t>& ram) const
{
}
