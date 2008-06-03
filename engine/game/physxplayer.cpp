#include "precompiled.h"
#include "game/player.h"
#include "game/physxplayer.h"
#include "game/game.h"
#include "physics/physics.h"
#include "timer/timer.h"

namespace game
{
	class PhysXPlayerShapeHit : public NxUserControllerHitReport
	{
	public:
		NxControllerAction  onShapeHit(const NxControllerShapeHit& hit)
		{
			if(hit.shape->getActor().getGroup() == 16) // hit the bsp
				return NX_ACTION_NONE;

			INFO("hit %s", hit.shape->getActor().getName());
			NxActor& actor = hit.shape->getActor();
			actor.addForceAtPos(hit.dir * (hit.length * 1000), hit.worldNormal);
			return NX_ACTION_PUSH;
		}

		NxControllerAction onControllerHit(const NxControllersHit& hit)
		{
			return NX_ACTION_NONE;
		}
	};

	class PhysXPlayer : public Player {
	public:
		PhysXPlayer(D3DXVECTOR3& size);
		~PhysXPlayer();

		void acquire();
		void release();

		bool setPos(D3DXVECTOR3& pos);		
		bool setSize(D3DXVECTOR3& size);
		bool setStepUp(float step_up);
		bool setMoveMode(t_movemode mode);

		void doMove(t_impulse impulse);
		void doRotation(D3DXVECTOR3& rotation);
		void updatePos();

		D3DXVECTOR3 getFlyDisplacement();
		D3DXVECTOR3 getWalkDisplacement();

		NxController* nxc;
		bool acquired;
		bool in_air;
		bool jumping;
		bool moving[MOVE_MAX];
		float fall_start;
		float fall_start_height;
		PhysXPlayerShapeHit shapehit;
	};


}

namespace physics {
	extern NxControllerManager* gManager;
	extern NxScene* gScene;
	extern float gravity;
}


using namespace game;

PhysXPlayer::PhysXPlayer(D3DXVECTOR3& size)
: Player(size), acquired(false), in_air(false), jumping(false), nxc(NULL)
{	
}

PhysXPlayer::~PhysXPlayer()
{
	if(acquired)
		release();
}

void PhysXPlayer::acquire()
{
	if(acquired)
		return;

	NxBoxControllerDesc desc;	
	desc.upDirection = NX_Y;
	desc.extents = (NxVec3)size;
	desc.stepOffset = step_up / physics::scale;
	desc.callback = &shapehit;

	nxc = physics::gManager->createController(physics::gScene, desc);
	//nxc->setCollision(false);

	acquired = true;
}

bool PhysXPlayer::setPos(D3DXVECTOR3& pos)
{
	if(!Player::setPos(pos))
		return false;
	if(acquired)
		if(!nxc->setPosition(NxExtendedVec3(this->pos.x / physics::scale, this->pos.y / physics::scale, this->pos.z / physics::scale)))
			return false;		
	return true;
}

bool PhysXPlayer::setStepUp(float step_up)
{
	if(!Player::setStepUp(step_up))
		return false;

	if(acquired && mode == MM_WALK)
		nxc->setStepOffset(this->step_up / physics::scale);
		
	return true;
}

bool PhysXPlayer::setMoveMode( t_movemode mode )
{
	if(!Player::setMoveMode(mode))
		return false;

	if(acquired)
	{
		INFO("setting move movde");
		switch(mode)
		{
		case MM_WALK:
			nxc->setStepOffset(this->step_up / physics::scale);
			((NxBoxController*)nxc)->setExtents((NxVec3)(size / physics::scale));
			break;
		default:
			nxc->setStepOffset(0.0f);
			((NxBoxController*)nxc)->setExtents(NxVec3(0,0,0));
			break;
		}
	}

	return true;
}

bool PhysXPlayer::setSize(D3DXVECTOR3& size)
{
	if(!Player::setSize(size))
		return false;

	if(!((NxBoxController*)nxc)->setExtents((NxVec3)(size / physics::scale)))
		return false;

	return true;
}

void PhysXPlayer::release()
{
	physics::gManager->releaseController(*nxc);
	nxc = NULL;
	acquired = false;
}

void PhysXPlayer::doMove(t_impulse impulse)
{
	moving[impulse] = true;
}

void PhysXPlayer::doRotation(D3DXVECTOR3& rotation)
{
	rot += rotation;	
	if(rot.x > 360.0f) rot.x -= 360.0f;
	if(rot.x < 0.0f) rot.x += 360.0f;
	if(rot.y > 90.0f) rot.y = 90.0f;
	if(rot.y < -90.0f) rot.y = -90.0f;
}

void PhysXPlayer::updatePos()
{
	if(!acquired)
		return;

	D3DXVECTOR3 dis;
	switch(mode) {
		case MM_FLY:
			dis = getFlyDisplacement();
			break;
		case MM_WALK:
			dis = getWalkDisplacement();
			break;
		default:
			break;
	}

	NxU32 flags = 0;
	NxU32 collisionGroups = game::noclip ? 0xffff : 0;

	nxc->move((NxVec3&)dis, collisionGroups, 0.0001f, flags, 1.0);

	physics::gManager->updateControllers();
	
	NxExtendedVec3 newpos = nxc->getPosition();

	//INFO("flags = %d", flags);
	if(flags & NXCC_COLLISION_DOWN)
	{
		if(!on_ground)
		{
			if(jumping)
				INFO("hit ground at %f m/s, air time was %fs", -vel.y, (timer::game_ms - fall_start) / 1000.0f);
			else
				INFO("hit ground at %f m/s, air time was %fs, fell %fm", -vel.y, (timer::game_ms - fall_start) / 1000.0f, fall_start_height - pos.y);
			jumping = false;
			on_ground = true;
			vel.y = 0.0f;
		}
	}
	else
	{
		if(on_ground)
		{
			on_ground = false;
			fall_start = timer::game_ms;
			fall_start_height = pos.y;
			NxU32 up_flags;
			nxc->move(NxVec3(0, step_up, 0), collisionGroups, 0.0001f, up_flags, 1.0);
		}
	}

	if(flags & NXCC_COLLISION_UP)
	{
		if(vel.y > 0.0f)
		{
			vel.y = 0.0f;
			jumping = false;
			fall_start = timer::game_ms;
		}
	}
	
	this->pos = D3DXVECTOR3(newpos.x, newpos.y, newpos.z) * physics::scale;

	for(int i = 0; i < MOVE_MAX; i++)
		moving[i] = false;
}

D3DXVECTOR3 PhysXPlayer::getFlyDisplacement()
{
	fall_start = timer::game_ms;
	fall_start_height = pos.y;
	jumping = false;

	D3DXVECTOR3 dis(0.0f, 0.0f, 0.0f);
	
	if(moving[MOVE_UP])
		dis.y += 1.0f;
	
	if(moving[MOVE_DOWN])
		dis.y -= 1.0f;

	if(moving[MOVE_LEFT])
		dis.x -= 1.0f;

	if(moving[MOVE_RIGHT])
		dis.x += 1.0f;

	if(moving[MOVE_FORWARD])
		dis.z += 1.0f;

	if(moving[MOVE_BACK])
		dis.z -= 1.0f;

	D3DXMATRIX mat;	
	D3DXMatrixRotationYawPitchRoll(&mat, rot.x * (D3DX_PI / 180.0f), rot.y * (D3DX_PI / 180.0f), rot.z * (D3DX_PI / 180.0f));
	D3DXVec3Normalize(&dis, &dis);
	D3DXVec3TransformCoord(&dis, &dis, &mat);

	return dis * timer::delta_s * speed;
}

D3DXVECTOR3 PhysXPlayer::getWalkDisplacement()
{
	D3DXVECTOR3 dis(0.0f, 0.0f, 0.0f);

	if(moving[MOVE_LEFT])
		dis.x -= 1.0f;

	if(moving[MOVE_RIGHT])
		dis.x += 1.0f;

	if(moving[MOVE_FORWARD])
		dis.z += 1.0f;

	if(moving[MOVE_BACK])
		dis.z -= 1.0f;

	static bool apogee;
	if(moving[MOVE_JUMP] && on_ground)
	{
		jumping = true;
		on_ground = false;
		fall_start = timer::game_ms;
		apogee = false;
		fall_start_height = pos.y;
		INFO("left ground at %f m/s", jump_velocity);
	}

	D3DXMATRIX mat;	
	D3DXMatrixRotationYawPitchRoll(&mat, rot.x * (D3DX_PI / 180.0f), 0, 0);
	D3DXVec3Normalize(&dis, &dis);
	D3DXVec3TransformCoord(&dis, &dis, &mat);
	dis *= timer::delta_s * speed;

	if(!on_ground)
	{
		float air_time = (timer::game_ms - fall_start) / 1000.0f;
		if(jumping)
		{
			vel.y = jump_velocity + (physics::gravity * air_time);
			if((vel.y <= 0.0) && !apogee)
			{
				INFO("apogee at %fs, height of %f meters", air_time, pos.y - fall_start_height);
				apogee = true;
			}
		}
		else
			vel.y = physics::gravity * air_time;
		dis.y = vel.y * timer::delta_s;
	} else
		dis.y = -step_up;

	return dis;
}

Player* game::createPhysXPlayer(D3DXVECTOR3& size)
{
	return new PhysXPlayer(size);
}