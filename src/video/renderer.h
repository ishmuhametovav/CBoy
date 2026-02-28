#pragma once
#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<array>
#include<cstdint>
#include<string>
#include<fstream>

struct pixel
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

class renderer
{
	uint32_t texture;
	uint32_t vbo;
	uint32_t vao;
	uint32_t shader;

	std::array<pixel, 4> palette;

	~renderer(){ }
	renderer(const renderer&) = delete;
	renderer& operator = (const renderer&) = delete;
	renderer();
public:
	static renderer& instance();
	void init(const std::string v_shader_folder, const std::string f_shader_folder);
	void draw(std::array<std::array<uint8_t, 160>, 144>& frame);
};