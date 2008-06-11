#pragma once

namespace game
{
	typedef enum
	{
		MM_WALK,
		MM_FLY
	} t_movemode;

	typedef enum
	{
		MOVE_UP,
		MOVE_DOWN,
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_FORWARD,
		MOVE_BACK,
		MOVE_JUMP,
		MOVE_MAX
	} t_impulse;

	class Player
	{
	public:
		Player(D3DXVECTOR3& size);
		virtual ~Player();

		virtual void acquire() {};
		virtual void release() {};

		virtual D3DXVECTOR3& getPos()
		{
			return pos;
		}
		virtual D3DXVECTOR3& getRot()
		{
			return rot;
		}
		virtual D3DXVECTOR3& getVelocity()
		{
			return vel;
		}
		virtual float getSpeed()
		{
			return speed;
		}
		virtual void setSpeed(float speed)
		{
			this->speed = speed;
		}
		virtual bool getCollide()
		{
			return collide;
		}
		virtual float getStepUp()
		{
			return step_up;
		}
		virtual t_movemode getMoveMode()
		{
			return mode;
		}

		virtual bool setPos(D3DXVECTOR3& pos)
		{
			this->pos = pos;
			return true;
		}
		virtual bool setRot(D3DXVECTOR3& rot)
		{
			this->rot = rot;
			return true;
		}
		virtual bool setSize(D3DXVECTOR3& size)
		{
			this->size = size;
			return true;
		}
		virtual bool setStepUp(float step_up)
		{
			this->step_up = step_up;
			return true;
		}
		virtual bool setCollide(bool collide)
		{
			this->collide = collide;
			return true;
		}
		virtual bool setMoveMode(t_movemode mode)
		{
			this->mode = mode;
			return true;
		}

		virtual void doMove(t_impulse impulse) = 0;
		virtual void doRotation(D3DXVECTOR3& rotation) = 0;
		virtual void updatePos() = 0;
		D3DXVECTOR3 pos, rot, vel, size;
		bool collide;
		bool on_ground;
		t_movemode mode;
		float speed;
		float step_up;
		float step_down;
		float jump_height; // in meters
		float gravity;
	};

	Player* createPlayer(D3DXVECTOR3& size);
};