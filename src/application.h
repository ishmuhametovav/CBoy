#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <vector>
#include<chrono>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<imgui.h>
#include<imgui_impl_glfw.h>
#include<imgui_impl_opengl3.h>
#include"CPU.h"
#include"audio/audio_engine.h"
#include"cartridge.h"
#include"debug.h"
#include"clock.h"

constexpr uint32_t frequency = 4194304;//HZ
constexpr uint32_t frequency_per_frame = frequency / 60;

class application
{
	uint32_t SCREEN_WIDTH;
	uint32_t SCREEN_HEIGHT;
	GLFWwindow* window;

	delta_timer clock;
	bool debug_mode;

	cpu& cp;
	gpu& gp;
	apu& ap;
	timer& tmr;
	input& inpt;
	memory& m;
	interrupt_registers& ir;
	oam& _oam;
	instruction_set& is;
	renderer& r;
	debug debugger;
	cartridge cart;
	audio_engine audio;

	void init_glfw();
	void init_glad();
	void init_dear_imgui();
	void loop();
	void loop_debug();
	void close_glfw();
	void close_dear_imgui();

	void draw_main_menu();

public:
	application(std::string rom_name, uint32_t SCREEN_WIDTH = 640, uint32_t SCREEN_HEIGHT = 576);
	void init();
	void start_loop();
	void close();
};