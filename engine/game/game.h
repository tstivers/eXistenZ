/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: game.h,v 1.2 2004/07/09 16:04:56 tstivers Exp $
//

#pragma once

namespace game {
	void init(void);
	void doTick(void);
	void release(void);

	bool startMap(char* name);

	enum {
		STATE_LOADLEVEL,
		STATE_RUN,
		STATE_EXIT
	};

	extern int game_state;
	extern float gravity;
	extern float std_friction;
	extern float air_friction;
	extern float player_accel;
	extern float player_speed;
	extern float jump_vel;
	extern float mouse_sens_x;
	extern float mouse_sens_y;
	extern float step_up;
	extern float climb_interpol;
	extern int noclip;	

	// console commands
	void con_map(char* cmd, char* cmdline, void* user);
};