#pragma once

namespace timer
{
	void init(void);
	void release(void);
	void doTick();

	extern float time_ms;
	extern float game_ms;
	extern float delta_ms;
	extern double delta_s;

	extern float time_scale;
};