#include "precompiled.h"
#include "timer/timer.h"
#include "settings/settings.h"

namespace timer {
	unsigned __int64 time_hz;
	unsigned __int64 time_ticks;	
	unsigned __int64 game_ticks;
	unsigned int time_ms;
	unsigned int sim_ms;
	unsigned int game_ms;
	float delta_ms;
	float time_scale;
	unsigned int tick_ms;
	unsigned int sim_tps;
	float min_ms;
	float max_ms;
};

void timer::init()
{		
	time_ms = 0;
	sim_ms = 0;
	sim_tps = 30;
	time_scale = 1.0f;
	game_ticks = 0;
	min_ms = 0.0f;
	max_ms = FLT_MAX;

	QueryPerformanceFrequency((LARGE_INTEGER*)&time_hz);		
	doTick();
	doTick();

	settings::addsetting("system.time.ms", settings::TYPE_INT, settings::FLAG_READONLY, NULL, NULL, &game_ms);
	settings::addsetting("system.time.delta", settings::TYPE_FLOAT, settings::FLAG_READONLY, NULL, NULL, &delta_ms);
	settings::addsetting("system.time.scale", settings::TYPE_FLOAT, 0, NULL, NULL, &time_scale);
	settings::addsetting("system.time.sim_tps", settings::TYPE_INT, 0, NULL, NULL, &sim_tps);
	settings::addsetting("system.time.min_ms", settings::TYPE_FLOAT, 0, NULL, NULL, &min_ms);
	settings::addsetting("system.time.max_ms", settings::TYPE_FLOAT, 0, NULL, NULL, &max_ms);

}

void timer::doTick()
{
	U64 last_time_ticks = time_ticks;
	U64 last_game_ticks = game_ticks;

	do {
		QueryPerformanceCounter((LARGE_INTEGER*)&time_ticks);	
		U64 delta_ticks = time_ticks - last_time_ticks;	
		delta_ms = ((float)delta_ticks * time_scale) / ((float)time_hz / 1000.0f);
		game_ticks = last_game_ticks + (unsigned __int64)((float)delta_ticks * time_scale);

		game_ms = (unsigned int)(game_ticks / ((float)time_hz / 1000.0f));
		time_ms = (unsigned int)(time_ticks / ((float)time_hz / 1000.0f));
	} while(delta_ms < min_ms);
	
	if(delta_ms > max_ms)
		delta_ms = max_ms;
}


