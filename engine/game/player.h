/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

#define MAX_SLIDES 5

namespace game {
	enum {
		MM_WALK,
		MM_FLY
	};

	enum {
		MOVE_UP,
		MOVE_DOWN,
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_FORWARD,
		MOVE_BACK,
		MOVE_JUMP,
		MOVE_MAX
	};

	class Player {
	public:
		Player();
		~Player();
		void do_move(int direction);
		void do_rotation(float x, float y, float z);
		void do_accel(float x, float y, float z);
		void do_jump();

		void update_pos();
		void process_moves();
		void do_move();
		void do_stepUp();
		void do_stepDn();
		void do_slide();
		bool do_stepDown();
		bool do_falling();
		void move_vect(float &x, float &z, float vect);
		D3DXVECTOR3 makeAccelVect(const float x_rot);
		float capSpeed(const float vector, const float velocity);

		D3DXVECTOR3 pos;
		D3DXVECTOR3 rot;
		D3DXVECTOR3 vel;
		float velocity;
		D3DXVECTOR3 size;
		bool in_jump;
		bool in_air;
		bool in_step;
		float step_interpol;
		float step_start_height;
		float step_end_height;
		D3DXVECTOR3 actual_pos;
		void do_step_interpol();
		int move_mode;
		bool moving[MOVE_MAX];
		bool prev_moving[MOVE_MAX];
		D3DXVECTOR3 move_start;
		D3DXVECTOR3 move_pos;
		D3DXVECTOR3 move_dest;
		D3DXVECTOR3	down;
		float move_left;
		int move_slides;
		int touched_brushes[MAX_SLIDES];
		int num_touched_brushes;
		int collided_brush;
		D3DXVECTOR3 collided_normal;
	};

	extern Player player;
};