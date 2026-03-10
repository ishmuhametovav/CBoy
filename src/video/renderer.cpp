#include "renderer.h"
#include<iostream>

renderer::renderer() : texture(0), vao(0), vbo(0), shader(0)
{
    palette[0] = {224, 248, 208};//white
    palette[1] = {136, 192, 112};//light gray
    palette[2] = {52, 104, 86};//dark gray
    palette[3] = {8 , 24, 32};//dark
}

renderer& renderer::instance()
{
    static renderer r;
    return r;
}

void renderer::init(const std::string v_shader_folder, const std::string f_shader_folder)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 160, 144, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);//пока nullptr для инициализации
    //glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    /*float vertices[]{0.0f,0.0f,0.0f, 0.0f,1.0f,
                      0.0f,1.0f,0.0f, 0.0f,0.0f,
                      1.0f,0.0f,0.0f, 1.0f,1.0f,

                      1.0f,0.0f,0.0f, 1.0f,1.0f,
                      1.0f,1.0f,0.0f, 1.0f,0.0f,
                      0.0f,1.0f,0.0f, 0.0f,0.0f
    };*/

    float vertices[]{ 0.0f,0.0f,0.0f, 0.0f,0.0f,
                  0.0f,1.0f,0.0f, 0.0f,1.0f,
                  1.0f,0.0f,0.0f, 1.0f,0.0f,

                  1.0f,0.0f,0.0f, 1.0f,0.0f,
                  1.0f,1.0f,0.0f, 1.0f,1.0f,
                  0.0f,1.0f,0.0f, 0.0f,1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)12);
    glBindVertexArray(0);

    std::string v_shader_s, f_shader_s;
    {
        std::ifstream ifs1(v_shader_folder);
        while (!ifs1.eof())
        {
            std::string buffer;
            std::getline(ifs1, buffer);
            v_shader_s = v_shader_s + buffer + "\r\n";
        }
        ifs1.close();

        std::ifstream ifs2(f_shader_folder);
        while (!ifs2.eof())
        {
            std::string buffer;
            std::getline(ifs2, buffer);
            f_shader_s = f_shader_s + buffer + "\r\n";
        }
        ifs2.close();
    }

    const char* v_shader_source = v_shader_s.c_str();
    const char* f_shader_source = f_shader_s.c_str();
    int ok;

    uint32_t v_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v_shader, 1, &v_shader_source, nullptr);
    glCompileShader(v_shader);
    glGetShaderiv(v_shader, GL_COMPILE_STATUS, &ok);
    if(!ok)
    {
        char log[512];
        glGetShaderInfoLog(v_shader, 512, nullptr, log);
        std::cerr << log << std::endl;
    }

    uint32_t f_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f_shader, 1, &f_shader_source, nullptr);
    glCompileShader(f_shader);
    glGetShaderiv(f_shader, GL_COMPILE_STATUS, &ok);
    if(!ok)
    {
        char log[512];
        glGetShaderInfoLog(f_shader, 512, nullptr, log);
        std::cerr << log << std::endl;
    }
    shader = glCreateProgram();
    glAttachShader(shader, v_shader);
    glAttachShader(shader, f_shader);
    glLinkProgram(shader);

    glGetProgramiv(shader, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetProgramInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << log << std::endl;
    }
    glDeleteShader(v_shader);
    glDeleteShader(f_shader);
}

void renderer::draw(std::array<std::array<uint8_t, 160>, 144>& frame)
{
    std::array <pixel, 160 * 144> rgb_frame;
    for(uint16_t i = 0; i < 144; i++)
    {
        for(uint16_t j = 0; j < 160; j++)
        {
            uint8_t base_color = frame[i][j];
            rgb_frame[i * 160 + j].r = palette[base_color].r;
            rgb_frame[i * 160 + j].g = palette[base_color].g;
            rgb_frame[i * 160 + j].b = palette[base_color].b;
        }
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 160, 144, GL_RGB, GL_UNSIGNED_BYTE, rgb_frame.data());
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(vao);
    glUseProgram(shader);

    glm::mat4 model{ 1.0f };
    model = glm::scale(model, glm::vec3(160.0f, 144.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, 
        glm::value_ptr(glm::ortho(0.0f, 160.0f, 144.0f, 0.0f, -1.0f, 1.0f)));
    glUniform1i(glGetUniformLocation(shader, "screen_texture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
