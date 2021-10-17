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

void timer::action_now()
{
	action_tp = std::chrono::steady_clock::now();
}

float timer::action_took_time_of_span()
{
	auto span = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - action_tp).count();
	return span / (float)ms;
}

i64 timer::ms_left() const
{
	auto span = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
	return ms - span;
}

float timer::sec_left() const
{
	return ms_left() / 1000.f;
}

float timer::min_left() const
{
	return sec_left() / 60.f;
}

void timer::stop()
{
	auto span = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
	ms -= span;
}






