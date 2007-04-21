#pragma once

namespace game {
	typedef enum {
		MM_WALK,
		MM_FLY
	} t_movemode;

	typedef enum {
		MOVE_UP,
		MOVE_DOWN,
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_FORWARD,
		MOVE_BACK,
		MOVE_JUMP,
		MOVE_MAX
	} t_impulse;

	class Player {
	public:
		Player(D3DXVECTOR3& size);
		virtual ~Player();

		virtual void acquire() {};
		virtual void release() {};
		
		virtual D3DXVECTOR3& getPos() { return pos; };
		virtual D3DXVECTOR3& getRot() { return rot; };
		virtual D3DXVECTOR3& getVelocity() { return vel; };
		virtual bool getCollide() { return collide; }
		virtual t_movemode getMoveMode() { return mode; }
		
		virtual void setPos(D3DXVECTOR3& pos) { this->pos = pos; };
		virtual void setRot(D3DXVECTOR3& rot) { this->rot = rot; };
		virtual void setCollide(bool collide) { this->collide = collide; }
		virtual void setMoveMode(t_movemode mode) { this->mode = mode; }

		virtual void doMove(t_impulse impulse) = 0;
		virtual void doRotation(D3DXVECTOR3& rotation) = 0;
		virtual void updatePos() = 0;
		D3DXVECTOR3 pos, rot, vel, size;
		bool collide;
		t_movemode mode;
	};

	Player* createPlayer(D3DXVECTOR3& size);
};