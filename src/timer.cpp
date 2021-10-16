#include "timer.h"

void timer::restart()
{
	start = std::chrono::steady_clock::now();
}

bool timer::time_is_up() const
{
	auto span = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
	return span > ms;
}
