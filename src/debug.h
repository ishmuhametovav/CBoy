#pragma once

#include<sstream>
#include<chrono>
#include<imgui.h>
#include"CPU.h"
#include"video/GPU.h"

struct info
{
	unsigned long long instr_num;
	std::string instr;
	std::string cpu_stat;
	std::string gpu_stat;
};

class debug
{
	bool execution;
	bool logging;
	unsigned long long instr_count;
	std::vector<info> instruction_log;
	bool show_memory_map;
	std::chrono::steady_clock::time_point last_time;

	void copy_log_to_clipboard();
	float calculate_delta_time();
	void log(cartridge& cart);

	void draw_oam_menu();
	void draw_memory_map(cartridge& cart);
	void draw_cpu_menu();
	void draw_gpu_menu();

public:
	debug();
	unsigned long menu(cartridge& cart);
	inline void increment_instr_count() { instr_count++; }
	inline bool is_execution() const { return execution; }
};