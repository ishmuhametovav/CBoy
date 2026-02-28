#pragma once
#include<string>
#include<cstdint>
#include<vector>
#include<fstream>

class mbc
{
public:
	virtual uint8_t get_rom_value(const uint16_t address, const std::vector<uint8_t>& rom) const = 0;
	virtual void set_register(const uint16_t address, const uint8_t value) = 0;
	virtual uint8_t get_ram_value(const uint16_t address, const std::vector<uint8_t>& ram) const = 0;
	virtual void set_ram_value(const uint16_t address, const uint8_t value, std::vector<uint8_t>& ram) const = 0;
};

class mbc1 : public mbc
{
	uint8_t RAMG;
	uint8_t BANK1;
	uint8_t BANK2;
	uint8_t MODE;
public:
	mbc1();
	uint8_t get_rom_value(const uint16_t address, const std::vector<uint8_t>& rom) const override;
	void set_register(const uint16_t address, const uint8_t value) override;
	uint8_t get_ram_value(const uint16_t address, const std::vector<uint8_t>& ram) const override;
	void set_ram_value(const uint16_t address, const uint8_t value, std::vector<uint8_t>& ram) const override;
};

class mbc0 : public mbc
{
public:
	mbc0() = default;
	uint8_t get_rom_value(const uint16_t address, const std::vector<uint8_t>& rom) const override;
	void set_register(const uint16_t address, const uint8_t value) override;
	uint8_t get_ram_value(const uint16_t address, const std::vector<uint8_t>& ram) const override;
	void set_ram_value(const uint16_t address, const uint8_t value, std::vector<uint8_t>& ram) const override;
};

class cartridge
{
	mbc* chip;
	std::vector<uint8_t> rom;
	std::vector<uint8_t> ram;
public:
	cartridge(const std::string path);
	uint8_t get_rom_value(const uint16_t address) const;
	void set_mbc_register(const uint16_t address, const uint8_t value);
	uint8_t get_ram_value(const uint16_t address) const;
	void set_ram_value(const uint16_t address, const uint8_t value);
};