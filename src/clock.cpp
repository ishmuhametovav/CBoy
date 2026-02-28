#include"clock.h"

delta_timer::delta_timer() : delta_time(0.0f)
{

}

void delta_timer::start()
{
	start_time = std::chrono::steady_clock::now();
}

void delta_timer::stop()
{
	end_time = std::chrono::steady_clock::now();
}

float delta_timer::calculate_delta_time()
{
	delta_time = std::chrono::duration<float>(end_time - start_time).count();
	return delta_time;
}

void delta_timer::wait_end_of_frame()
{
	while(delta_time <= 1.0f / (float)60)
	{
		end_time = std::chrono::steady_clock::now();
		calculate_delta_time();
	}
}