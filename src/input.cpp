#include "input.h"
#include<iostream>

input::input() : P1(0x30), ssab_cond(0x0F), d_pad_cond(0x0F)
{
    keys.fill(false);
}

input& input::instance()
{
    static input inpt;
    return inpt;
}

void input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    input& i = input::instance();

    if (key == GLFW_KEY_UP)
    {
        if(action == GLFW_PRESS) i.keys[0] = true;
        else if (action == GLFW_RELEASE) i.keys[0] = false;
    }
        //i.d_pad_cond = action == GLFW_PRESS ? i.d_pad_cond & 0b1011 : i.d_pad_cond | 0b0100;

    if (key == GLFW_KEY_DOWN) 
    { 
        if (action == GLFW_PRESS) i.keys[1] = true;
        else if (action == GLFW_RELEASE) i.keys[1] = false;
    }
        //i.d_pad_cond = action == GLFW_PRESS ? i.d_pad_cond & 0b0111 : i.d_pad_cond | 0b1000;

    if (key == GLFW_KEY_LEFT) 
    { 
        if (action == GLFW_PRESS) i.keys[2] = true;
        else if (action == GLFW_RELEASE) i.keys[2] = false;
    }
        //i.d_pad_cond = action == GLFW_PRESS ? i.d_pad_cond & 0b1101 : i.d_pad_cond | 0b0010;

    if (key == GLFW_KEY_RIGHT) 
    { 
        if (action == GLFW_PRESS) i.keys[3] = true;
        else if (action == GLFW_RELEASE) i.keys[3] = false;
    }
        //i.d_pad_cond = action == GLFW_PRESS ? i.d_pad_cond & 0b1110 : i.d_pad_cond | 0b0001;

    if (key == GLFW_KEY_Z) 
    { 
        if (action == GLFW_PRESS) i.keys[4] = true;
        else if (action == GLFW_RELEASE) i.keys[4] = false;
    }//A 
        //i.ssab_cond = action == GLFW_PRESS ? i.ssab_cond & 0b1110 : i.ssab_cond | 0b0001;

    if (key == GLFW_KEY_X) 
    { 
        if (action == GLFW_PRESS) i.keys[5] = true;
        else if (action == GLFW_RELEASE) i.keys[5] = false;
    }//B
        //i.ssab_cond = action == GLFW_PRESS ? i.ssab_cond & 0b1101 : i.ssab_cond | 0b0010;

    if (key == GLFW_KEY_SPACE) 
    { 
        if (action == GLFW_PRESS) i.keys[6] = true;
        else if (action == GLFW_RELEASE) i.keys[6] = false;
    }//START
        //i.ssab_cond = action == GLFW_PRESS ? i.ssab_cond & 0b0111 : i.ssab_cond | 0b1000;

    if (key == GLFW_KEY_ESCAPE) 
    { 
        if (action == GLFW_PRESS) i.keys[7] = true;
        else if (action == GLFW_RELEASE) i.keys[7] = false;
    }//SELECT
        //i.ssab_cond = action == GLFW_PRESS ? i.ssab_cond & 0b1011 : i.ssab_cond | 0b0100;
}

void input::interruption()
{
    interrupt_registers& ir = interrupt_registers::instance();

    if (!(P1 & 0b00100000) && ((ssab_cond & 0x0F) != 0x0F)) 
    {
        //std::cout << "Interruption was call from ssab" << std::endl;
        ir.set_if(ir.get_if() | 0b10000);
    }
    if (!(P1 & 0b00010000) && ((d_pad_cond & 0x0F) != 0x0F)) 
    {
        //std::cout << "Interruption was call from d_pad" << std::endl;
        ir.set_if(ir.get_if() | 0b10000);
    }
}

void input::cycle()
{
    d_pad_cond = keys[0] ? d_pad_cond & 0b1011 : d_pad_cond | 0b0100;

    d_pad_cond = keys[1] ? d_pad_cond & 0b0111 : d_pad_cond | 0b1000;

    d_pad_cond = keys[2] ? d_pad_cond & 0b1101 : d_pad_cond | 0b0010;

    d_pad_cond = keys[3] ? d_pad_cond & 0b1110 : d_pad_cond | 0b0001;

    ssab_cond = keys[4] ? ssab_cond & 0b1110 : ssab_cond | 0b0001;

    ssab_cond = keys[5] ? ssab_cond & 0b1101 : ssab_cond | 0b0010;

    ssab_cond = keys[6] ? ssab_cond & 0b0111 : ssab_cond | 0b1000;

    ssab_cond = keys[7] ? ssab_cond & 0b1011 : ssab_cond | 0b0100;

    interruption();
}

uint8_t input::get_p1() const
{
    if (!(P1 & 0b100000)) return (P1 & 0b110000) | (ssab_cond & 0b1111);
    else if (!(P1 & 0b010000)) return (P1 & 0b110000) | (d_pad_cond & 0b1111);

    else return (P1 & 0xF0) | 0x0F;
}

void input::set_p1(const uint8_t value)
{
    P1 = value & 0b110000;
}
