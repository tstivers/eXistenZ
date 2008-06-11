#include "precompiled.h"
#include "game/player.h"
#include "game/bspplayer.h"
#include "game/game.h"
#include "console/console.h"
#include "timer/timer.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bspcollide.h"
#include "q3bsp/bspcache.h"

#define MIN_VELOCITY (0.01f)
#define MIN_MOVE (0.001f)
#define CLAMP_NEG(x) (x < 0 ? 0 : x)
#define CLAMP_POS(x) (x > 0 ? 0 : x)
#define TEXNAME (collider.fraction != 1.0f ? q3bsp::bsp->textures[collider.brush_contents]->name : "none")
#define LOGPOS(a,b) a.x, a.y, a.z, b.x, b.y, b.z
#define LOGCOL(label, start, end) FRAMEDO(con::log(con::FLAG_DEBUG, "  %-12s [%1.2f] (%2.2f, %2.2f, %2.2f) - (%2.2f, %2.2f, %2.2f) : %s[%i]", label, collider.fraction, LOGPOS(start, end), TEXNAME, collider.brush_id))
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))
#define MAX_SLIDES 5

namespace game
{
	class BSPPlayer : public Player
	{
	public:
		BSPPlayer(D3DXVECTOR3& size);
		~BSPPlayer();
		void doMove(t_impulse impulse);
		void doRotation(D3DXVECTOR3& rotation);
		void do_accel(float x, float y, float z);
		void do_jump();

		void updatePos();
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

		float velocity;
		bool in_jump;
		bool in_air;
		bool in_step;
		float step_interpol;
		float step_start_height;
		float step_end_height;
		D3DXVECTOR3 actual_pos;
		void do_step_interpol();
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
};

using namespace game;

BSPPlayer::BSPPlayer(D3DXVECTOR3& size)
		: Player(size)
{
	actual_pos = D3DXVECTOR3(0, 0, 0);
	size = D3DXVECTOR3(10, 35, 10);
	in_jump = false;
	in_air = false;
	in_step = false;
	down = D3DXVECTOR3(0, -1, 0);
}

BSPPlayer::~BSPPlayer()
{
}

void BSPPlayer::doRotation(D3DXVECTOR3& rotation)
{
	rot += rotation;
	if (rot.x > 360.0f) rot.x -= 360.0f;
	if (rot.x < 0.0f) rot.x += 360.0f;
	if (rot.y > 90.0f) rot.y = 90.0f;
	if (rot.y < -90.0f) rot.y = -90.0f;
}

void BSPPlayer::doMove(t_impulse impulse)
{
	moving[impulse] = true;
}

// notes on wall moving:
// create rotation matrix that points down
// multiply rotation vector by matrix for mouse rotation
// multiply velocity vector by matrix for position
// when doing move, do this:
//	check down and get the normal of what you started on
//  move to destination
//	if you collide with something
//		try to step up onto it like normal
//		if you couldn't step up
//			new down becomes opposite of collider.hitnormal
//			slide if you have movement left
//	check down by step_down
//  if you collide with something
//		new down becomes opposite of collider.hitnormal
//		new position becomes collider.collision
//  if you don't collide with anything
//		rotate "down" matrix 45deg times movement vector
//		check down by move_down again
//		if you collide with something
//			new down becomes collider.hitnormal
//			new position becomes collider.collision
//		if not, you're in the air and new down is gravity

void BSPPlayer::updatePos()
{
	D3DXVECTOR3 start_pos = pos;

	move_left = 1.0f;
	move_slides = 0;

	process_moves();
	D3DXVECTOR3 end_pos = pos;
	D3DXVECTOR3 move_delta = end_pos - start_pos;
	move_start = start_pos;
	move_pos = start_pos;
	move_dest = end_pos;
	num_touched_brushes = 0;

	if (!game::noclip && q3bsp::bsp)
	{
		while (move_left > 0)
			do_move();

		pos = move_pos;

		if (!in_air)
			in_air = do_stepDown();

		if (in_air)
			in_air = do_falling();

		if (in_step)
			do_step_interpol();
		else
			actual_pos = pos;
	}
	else
		actual_pos = pos;
}

void BSPPlayer::do_step_interpol()
{
	step_interpol += game::climb_interpol * (float)timer::delta_ms / (1000.0f / 600.f);
	if (step_interpol > 1.0f)
	{
		step_interpol = 1.0f;
		in_step = false;
	}

	D3DXVECTOR3 step_start = pos;
	D3DXVECTOR3 step_end = pos;
	step_start.y = step_start_height;
	step_end.y = step_end.y;
	D3DXVec3Lerp(&actual_pos, &step_start, &step_end, step_interpol);
	//actual_pos = pos;
}

void BSPPlayer::do_move()
{
	q3bsp::collider_t collider;
	D3DXVECTOR3 start_pos = move_pos;
	D3DXVECTOR3 start_dest = move_dest;
	collided_brush = 0;
	collided_normal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	FRAMEDO(LOG2("[do_move()] (%2.4f left)", move_left));

	q3bsp::bsp->collide(move_pos, move_dest, size, collider, move_left);
	move_pos = collider.collision;
	move_left -= collider.fraction;
	collided_brush = collider.brush_id;
	collided_normal = collider.normal;

	if (move_left > 0)
		do_stepUp();

	if (move_left > 0)
		do_slide();

	// make sure we accomplished something
	if ((start_pos == move_pos) && (start_dest == move_dest))
	{
		FRAMEDO(LOG("[do_move()] wedged, exiting..."));
		move_left = 0;
	} // let slide take care of this?
}

void BSPPlayer::do_stepUp()
{
	D3DXVECTOR3 step_start = move_pos;
	D3DXVECTOR3 step_end = move_dest;

	step_start.y += game::step_up;
	step_end.y += game::step_up;

	q3bsp::collider_t collider;

	// move up till we hit something
	q3bsp::bsp->collide(move_pos, step_start, size, collider);
	if (!(collider.fraction > 0)) // couldn't move up at all
		return;

	step_start = collider.collision;
	step_end.y = step_start.y;

	// try to move to our elevated dest
	q3bsp::bsp->collide(step_start, step_end, size, collider, move_left);
	if (!(collider.fraction > 0)) // stepping up didn't let us move, if it's a different brush then we try to slide
	{
		if (collider.brush_id != collided_brush)
		{
			// we collided with a different brush, so we'll try to slide
			collider.normal.y = 0;
			D3DXVec3Normalize(&collider.normal, &collider.normal);
			D3DXVECTOR3 move_delta = step_end - step_start;
			D3DXVECTOR3 slide = move_delta - collider.normal * D3DXVec3Dot(&collider.normal, &move_delta);
			step_end = step_start + slide;
			q3bsp::bsp->collide(step_start, step_end, size, collider, move_left);
		}
		else
			return;
	}

	FRAMEDO(LOG("[do_stepUp()] went up a step"));
	// we moved, update position and move_left
	move_pos = collider.collision;
	move_left -= collider.fraction;
	collided_brush = collider.brush_id;
	collided_normal = collider.normal;

	// stick to the ground wherever our move put us
	step_start = move_pos;
	step_end = move_pos;
	step_end.y -= game::step_up;
	q3bsp::bsp->collide(step_start, step_end, size, collider);

	// update our position
	move_pos = collider.collision;

	// start step interpolation if we ended up at a diff height than we started at
	if (in_step)
		step_start_height = actual_pos.y;
	else
		step_start_height = move_start.y;
	step_interpol = 0.0f;
	in_step = true;
	step_end_height = move_pos.y;
}

void BSPPlayer::do_slide()
{
	// avoid infinite recursion
	if (num_touched_brushes + 1 > MAX_SLIDES)
	{
		LOG("[BSPPlayer::do_slide] WARNING: hit MAX_SLIDES");
		return;
	}

	// make sure we haven't hit it before
	int i;
	for (i = 0; (i < num_touched_brushes) && (touched_brushes[i] != collided_brush); i++);
	if (i != num_touched_brushes)
		return;
	touched_brushes[num_touched_brushes++] = collided_brush;

	// get the normal and calc our new destination
	collided_normal.y = 0;
	D3DXVECTOR3 move_delta = move_dest - move_pos;
	D3DXVECTOR3 slide = move_delta - (collided_normal * D3DXVec3Dot(&collided_normal, &move_delta));

	// todo: test destination's next collision to see if it is back towards our original vector
	// move_left = 0 if it is (so very very tired)
	q3bsp::collider_t collider;
	q3bsp::bsp->collide(move_pos, move_pos + slide, size, collider);
	if (collider.fraction < 1.0f)
	{
		collider.normal.y = 0;
		D3DXVECTOR3 slide2 = slide - (collider.normal * D3DXVec3Dot(&collider.normal, &slide));
		float dot2 = D3DXVec3Dot(&move_delta, &slide2);
		if (dot2 < 0) // angle is opposite the way we started out moving
		{
			move_pos = collider.collision;
			move_left = 0.0;
			return;
		}
	}

	// update our new destination
	move_dest = move_pos + slide;
}

bool BSPPlayer::do_stepDown()
{
	D3DXVECTOR3 start_pos = pos;
	D3DXVECTOR3 end_pos = pos;
	end_pos.y -= game::step_up;
	q3bsp::collider_t collider;

	q3bsp::bsp->collide(start_pos, end_pos, size, collider);
	if (collider.fraction == 1.0f)  // falling
	{
		return true;
	}

	pos = collider.collision;

	if (in_step)
		step_start_height = actual_pos.y;
	else
		step_start_height = pos.y;

	step_interpol = 0.0f;
	in_step = true;
	step_end_height = pos.y;

	return false;
}

bool BSPPlayer::do_falling()
{
	D3DXVECTOR3 start_pos = pos;
	D3DXVECTOR3 end_pos = pos;
	end_pos.y += vel.y * ((float)timer::delta_ms / (1000.0f / 600.0f));
	vel.y -= gravity * ((float)timer::delta_ms / (1000.0f / 600.0f));
	q3bsp::collider_t collider;

	in_step = false;

	q3bsp::bsp->collide(start_pos, end_pos, size, collider);
	pos = collider.collision;

	if (collider.fraction < 1.0f)  // hit the ground
	{
		vel.y = 0;
		return false;
	}

	return true;
}

void BSPPlayer::process_moves()
{
	D3DXVECTOR3 acceleration_vect(0, 0, 0);

	if (moving[MOVE_UP])
	{
		pos.y += player_speed * ((float)timer::delta_ms / (1000.0f / 600.0f));
	}

	if (moving[MOVE_DOWN])
	{
		pos.y -= player_speed * ((float)timer::delta_ms / (1000.0f / 600.0f));
	}

	if (moving[MOVE_LEFT])
	{
		acceleration_vect = makeAccelVect(rot.x - 90);
	}

	if (moving[MOVE_RIGHT])
	{
		acceleration_vect = makeAccelVect(rot.x + 90);
	}

	if (moving[MOVE_FORWARD])
	{
		acceleration_vect = makeAccelVect(rot.x);
	}

	if (moving[MOVE_BACK])
	{
		acceleration_vect = makeAccelVect(rot.x + 180);
	}

	if (moving[MOVE_FORWARD] && moving[MOVE_LEFT])
	{
		acceleration_vect = makeAccelVect(rot.x - 45);
	}

	if (moving[MOVE_FORWARD] && moving[MOVE_RIGHT])
	{
		acceleration_vect = makeAccelVect(rot.x + 45);
	}

	if (moving[MOVE_BACK] && moving[MOVE_LEFT])
	{
		acceleration_vect = makeAccelVect(rot.x - 180 + 45);
	}

	if (moving[MOVE_BACK] && moving[MOVE_RIGHT])
	{
		acceleration_vect = makeAccelVect(rot.x - 180 - 45);
	}

	if (moving[MOVE_JUMP])
	{
		vel.y = game::jump_vel;
		in_jump = true;
		in_air = true;
	}

	D3DXVec3Normalize(&acceleration_vect, & acceleration_vect);

	if (!acceleration_vect.x && !acceleration_vect.z)
	{
		float tmp_y = vel.y;
		D3DXVECTOR3 stopped(0, 0, 0);
		vel.y = 0;
		D3DXVec3Lerp(&vel, &vel, &stopped, MIN(1.0f, game::std_friction * ((float)timer::delta_ms / (1000.0f / 600.f))));
		vel.y = tmp_y;
		if ((vel.x > -MIN_VELOCITY) && (vel.x < MIN_VELOCITY))
			vel.x = 0;
		if ((vel.z > -MIN_VELOCITY) && (vel.z < MIN_VELOCITY))
			vel.z = 0;
	}
	else
	{
		float tmp_y = vel.y;
		vel.y = 0;
		acceleration_vect *= player_speed;
		D3DXVec3Lerp(&vel, &vel, &acceleration_vect, MIN(1.0f, game::player_accel * ((float)timer::delta_ms / (1000.0f / 600.f))));
		vel.y = tmp_y;
	}

	// do the actual movement
	pos.x += vel.x * ((float)timer::delta_ms / (1000.0f / 600.f));
	pos.z += vel.z * ((float)timer::delta_ms / (1000.0f / 600.f));

	// clear the move flags
	for (int i = 0; i < MOVE_MAX; i++)
		moving[i] = false;
}

D3DXVECTOR3 BSPPlayer::makeAccelVect(const float x_rot)
{
	return D3DXVECTOR3(
			   (float)sin(x_rot * (D3DX_PI / 180.0f)),
			   0,
			   (float)cos(x_rot * (D3DX_PI / 180.0f)));
}

Player* game::createBSPPlayer(D3DXVECTOR3& size)
{
	return new BSPPlayer(size);
}