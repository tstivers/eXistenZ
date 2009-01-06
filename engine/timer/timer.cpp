#include "precompiled.h"
#include "timer/timer.h"
#include "settings/settings.h"

namespace timer
{
	unsigned __int64 time_hz;
	unsigned __int64 frame_ticks;
	unsigned __int64 game_ticks;
	unsigned __int64 start_ticks;

	float time_ms;
	float game_ms;
	float delta_ms;
	double delta_s;
	float time_scale;
	float min_ms;
	float max_ms;

	__int64 Stopwatch::timer_frequency_ = 0;
	double Stopwatch::ticks_per_ms_ = 0.0;
}

REGISTER_STARTUP_FUNCTION(timer, timer::init, 10);

void timer::init()
{
	time_ms = 0.0f;
	game_ms = 0.0f;
	time_scale = 1.0f;
	min_ms = 0.0f;
	max_ms = FLT_MAX;

	QueryPerformanceFrequency((LARGE_INTEGER*)&time_hz);
	QueryPerformanceCounter((LARGE_INTEGER*)&frame_ticks);
	start_ticks = frame_ticks;
	game_ticks = frame_ticks;
	doTick();

	settings::addsetting("system.time.ms", settings::TYPE_FLOAT, settings::FLAG_READONLY, NULL, NULL, &game_ms);
	settings::addsetting("system.time.delta", settings::TYPE_FLOAT, settings::FLAG_READONLY, NULL, NULL, &delta_ms);
	settings::addsetting("system.time.scale", settings::TYPE_FLOAT, 0, NULL, NULL, &time_scale);
	settings::addsetting("system.time.min_ms", settings::TYPE_FLOAT, 0, NULL, NULL, &min_ms);
	settings::addsetting("system.time.max_ms", settings::TYPE_FLOAT, 0, NULL, NULL, &max_ms);
}

void timer::doTick()
{
	U64 last_frame_ticks = frame_ticks;
	QueryPerformanceCounter((LARGE_INTEGER*)&frame_ticks);
	U64 delta_ticks = frame_ticks - last_frame_ticks;
	delta_ms = ((double)delta_ticks * time_scale) / ((double)time_hz / 1000.0);
	delta_s = ((double)delta_ticks * time_scale) / (double)time_hz;
	game_ms += delta_ms;

	//game_ms = (double)(game_ticks - start_ticks) / ((double)time_hz / 1000.0);
	time_ms = (double)(frame_ticks - start_ticks) / ((double)time_hz / 1000.0);
	//INFO("game_ms = %f, time_ms = %f", game_ms, time_ms);

	if (delta_ms > max_ms)
		delta_ms = max_ms;
}


