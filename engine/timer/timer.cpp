/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: timer.cpp,v 1.2 2003/12/03 07:21:39 tstivers Exp $
//

#include "precompiled.h"
#include "timer/timer.h"
#include "settings/settings.h"
#include "console/console.h"

namespace timer {
	__int64 time_hz;	
	__int64 time_ticks;
	__int64 delta_ticks;
	unsigned int time_ms;
	float delta_ms;
};

void timer::init()
{		
	QueryPerformanceFrequency((LARGE_INTEGER*)&time_hz);		
	doTick();
	doTick();

	settings::addsetting("system.time.ms", settings::TYPE_INT, settings::FLAG_READONLY, NULL, NULL, &time_ms);
	settings::addsetting("system.time.delta", settings::TYPE_FLOAT, settings::FLAG_READONLY, NULL, NULL, &delta_ms);
}

void timer::doTick()
{
	U64 last_time_ticks = time_ticks;	
	QueryPerformanceCounter((LARGE_INTEGER*)&time_ticks);	
	delta_ticks = time_ticks - last_time_ticks;	

	time_ms = (unsigned int)(time_ticks / ((float)time_hz / 1000.0f));
	delta_ms = (float)delta_ticks / ((float)time_hz / 1000.0f);
}
