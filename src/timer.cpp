#include "timer.h"

timer::timer() : counter(0), TIMA(0), TMA(0), TAC(0xF8), pending_overflow(false)
{ }

void timer::handle_tima(int32_t cycles, uint32_t old_counter)
{
    interrupt_registers& ir = interrupt_registers::instance();
    //если таймер не включен то выходим
    if (!(TAC & 0b100)) return;
    
    while (cycles)
    {
        old_counter ++;

        if (pending_overflow)//но это должно идти на следующий машинный цикл а не следующий такт! 
        {
            ir.set_if(ir.get_if() | 0b00000100);
            TIMA = TMA;
            pending_overflow = false;
        }

        if (check_tick(old_counter, old_counter - 1))
        {
            TIMA++;
            //вызываем прерывание при переполнении tima
            if (TIMA > 0xFF) pending_overflow = true;
        }
        cycles--;
    }
    
}

timer& timer::instance()
{
    static timer t;
    return t;
}

void timer::cycle(int32_t m_cycles)
{
    uint32_t old_counter = counter;//значение счетчика до включения
    uint32_t cycles = m_cycles * 4;//количество тиков
    
    counter += cycles;
    counter &= 0xFFFF;//сброс счетчика

    handle_tima(cycles, old_counter);

}

bool timer::check_tick(uint32_t new_counter, uint32_t old_counter)//случился ли такт
{
    if ((TAC & 0b11) == 0)//256 mcycles - тоесть 4096 Hz
    {
        if ((old_counter & 512) && !(new_counter & 512))
            return true;
    }
    else if ((TAC & 0b11) == 1)//4 mcycles - тоесть 262144 Hz
    {
        if ((old_counter & 8) && !(new_counter & 8))
            return true;
    }
    else if ((TAC & 0b11) == 2)//16 mcycles - тоесть 65536 Hz
    {
        if ((old_counter & 32) && !(new_counter & 32))
            return true;
    }
    else if ((TAC & 0b11) == 3)//64 mcycles - тоесть 16384 Hz
    {
        if ((old_counter & 128) && !(new_counter & 128))
            return true;
    }
    return false;
}

