#include<GLFW/glfw3.h>
#include"memory.h"
#include<array>

class input
{
	input(const input&) = delete;
	input& operator = (const input&) = delete;
	~input(){ }

	input();

	uint8_t P1;//управляющий регистр joypad
	uint8_t ssab_cond;//состояние ssab кнопок
	uint8_t d_pad_cond;//состояние крестовины

	void interruption();

public:
	static input& instance();
	std::array<bool, 8> keys;

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void cycle();
	uint8_t get_p1() const;
	void set_p1(const uint8_t value);
};