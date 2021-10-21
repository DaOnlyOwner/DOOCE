#pragma once
#include <chrono>
#include "definitions.h"
#include <stdexcept>

class timer
{
public:
	timer(float milli) : ms(milli) { if (milli < 0) throw std::runtime_error("Time Span was below zero"); restart(); action_now(); }
	void restart();
	bool time_is_up() const;
	void action_now();
	// This function returns the quotient of the time it took for the last action (action_now) and the passed in timespan.
	float action_took_time_of_span();
	i64 ms_left() const;
	float sec_left() const;
	float min_left() const;
	void stop();
	bool is_running() const;

private:
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point action_tp;
	bool running = false;
	u64 ms = 0;
};