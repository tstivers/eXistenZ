/////////////////////////////////////////////////////////////////////////////
// dx.h
// rendering system directx functions interface
// $Id: timer.h,v 1.2 2004/07/09 16:04:56 tstivers Exp $
//

#pragma once

namespace timer {
	void init(void);
	void release(void);

	void doTick();

	extern unsigned int time_ms;
	extern unsigned int sim_ms;
	extern unsigned int game_ms;
	extern float delta_ms;

	extern float time_scale;
	extern unsigned int tick_ms;
};