#pragma once

#include<chrono>

class delta_timer
{
	std::chrono::steady_clock::time_point start_time;
	std::chrono::steady_clock::time_point end_time;
	float delta_time;
public:
	delta_timer();
	void start();
	void stop();
	float calculate_delta_time();
	void wait_end_of_frame();
};
