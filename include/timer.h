#pragma once
#include <chrono>

class timer
{
public:
	timer(int ms) : ms(ms) { restart(); }
	void restart();
	bool time_is_up() const;

private:
	std::chrono::steady_clock::time_point start;
	int ms = 0;
};