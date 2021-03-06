#include "precompiled.h"
#include "game/player.h"
#include "game/physxplayer.h"
#include "game/game.h"
#include "physics/physics.h"
#include "scene/scene.h"
#include "timer/timer.h"
#include <NxPhysics.h>
#include <NxController.h>
#include <NxControllerManager.h>
#include <NxCapsuleController.h>
#include <NxBoxController.h>

namespace game
{
	class PhysXPlayerShapeHit : public NxUserControllerHitReport
	{
	public:
		NxControllerAction  onShapeHit(const NxControllerShapeHit& hit)
		{
			//if (hit.shape->getActor().getGroup() == 16) // hit the bsp
				return NX_ACTION_NONE;

			//INFO("hit %s", hit.shape->getActor().getName());
			//NxActor& actor = hit.shape->getActor();
			//actor.addForceAtPos(hit.dir * (hit.length * 1000), hit.worldNormal);
			//return NX_ACTION_PUSH;
		}

		NxControllerAction onControllerHit(const NxControllersHit& hit)
		{
			return NX_ACTION_NONE;
		}
	};

	class PhysXPlayer : public Player
	{
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

using namespace game;

PhysXPlayer::PhysXPlayer(D3DXVECTOR3& size)
		: Player(size), acquired(false), in_air(false), jumping(false), nxc(NULL)
{
}

PhysXPlayer::~PhysXPlayer()
{
	if (acquired)
		release();
}

void PhysXPlayer::acquire()
{
	if (acquired)
		return;

	ASSERT(scene::g_scene);

	NxBoxControllerDesc desc;
	desc.upDirection = NX_Y;
	desc.extents = (NxVec3)size;
	desc.stepOffset = step_up;
	//desc.callback = &shapehit;

	nxc = scene::g_scene->getPhysicsManager()->getControllerManager()->createController(scene::g_scene->getPhysicsManager()->getPhysicsScene(), desc);
	//nxc->setCollision(false);
	INFO("actor collision group = %i", nxc->getActor()->getShapes()[0]->getGroup());

	acquired = true;
}

void PhysXPlayer::release()
{
	if(!acquired)
		return;

	ASSERT(scene::g_scene);
	scene::g_scene->getPhysicsManager()->getControllerManager()->releaseController(*nxc);
	nxc = NULL;
	acquired = false;
}

bool PhysXPlayer::setPos(D3DXVECTOR3& pos)
{
	if (!Player::setPos(pos))
		return false;
	if (acquired)
		if (!nxc->setPosition(NxExtendedVec3(this->pos.x, this->pos.y, this->pos.z)))
			return false;
	return true;
}

bool PhysXPlayer::setStepUp(float step_up)
{
	if (!Player::setStepUp(step_up))
		return false;

	if (acquired && mode == MM_WALK)
		nxc->setStepOffset(this->step_up);

	return true;
}

bool PhysXPlayer::setMoveMode(t_movemode mode)
{
	if (!Player::setMoveMode(mode))
		return false;

	if (acquired)
	{		
		switch (mode)
		{
		case MM_WALK:
			INFO("setting move mode to WALK");
			nxc->setStepOffset(this->step_up);
			((NxBoxController*)nxc)->setExtents((NxVec3)size);
			break;
		default:
			INFO("setting move mode to FLY");
			nxc->setStepOffset(0.0f);
			((NxBoxController*)nxc)->setExtents(NxVec3(0, 0, 0));
			break;
		}
	}

	return true;
}

bool PhysXPlayer::setSize(D3DXVECTOR3& size)
{
	if (!Player::setSize(size))
		return false;

	if (!((NxBoxController*)nxc)->setExtents((NxVec3)size))
		return false;

	return true;
}

void PhysXPlayer::doMove(t_impulse impulse)
{
	moving[impulse] = true;
}

void PhysXPlayer::doRotation(D3DXVECTOR3& rotation)
{
	rot += rotation;
	if (rot.y > 360.0f) rot.y -= 360.0f;
	if (rot.y < 0.0f) rot.y += 360.0f;
	if (rot.x > 90.0f) rot.x = 90.0f;
	if (rot.x < -90.0f) rot.x = -90.0f;
}

void PhysXPlayer::updatePos()
{
	if (!acquired)
		return;

	D3DXVECTOR3 dis;
	switch (mode)
	{
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

	scene::g_scene->getPhysicsManager()->getControllerManager()->updateControllers();

	NxExtendedVec3 newpos = nxc->getPosition();

	//INFO("flags = %d", flags);
	if (flags & NXCC_COLLISION_DOWN)
	{
		if (!on_ground)
		{
			//if(jumping)
			//	INFO("hit ground at %f m/s, air time was %fs", -vel.y, (timer::game_ms - fall_start) / 1000.0f);
			//else
			//	INFO("hit ground at %f m/s, air time was %fs, fell %fm", -vel.y, (timer::game_ms - fall_start) / 1000.0f, fall_start_height - pos.y);
			jumping = false;
			on_ground = true;
			vel.y = 0.0f;
		}
	}
	else
	{
		if (on_ground)
		{
			on_ground = false;
			fall_start = timer::game_ms;
			fall_start_height = pos.y;
			NxU32 up_flags;
			nxc->move(NxVec3(0, step_up, 0), collisionGroups, 0.0001f, up_flags, 1.0);
		}
	}

	if (flags & NXCC_COLLISION_UP)
	{
		if (vel.y > 0.0f)
		{
			vel.y = 0.0f;
			jumping = false;
			fall_start = timer::game_ms;
		}
	}

	this->pos = D3DXVECTOR3(newpos.x, newpos.y, newpos.z);

	for (int i = 0; i < MOVE_MAX; i++)
		moving[i] = false;
}

D3DXVECTOR3 PhysXPlayer::getFlyDisplacement()
{
	fall_start = timer::game_ms;
	fall_start_height = pos.y;
	jumping = false;

	D3DXVECTOR3 dis(0.0f, 0.0f, 0.0f);

	if (moving[MOVE_UP])
		dis.y += 1.0f;

	if (moving[MOVE_DOWN])
		dis.y -= 1.0f;

	if (moving[MOVE_LEFT])
		dis.x -= 1.0f;

	if (moving[MOVE_RIGHT])
		dis.x += 1.0f;

	if (moving[MOVE_FORWARD])
		dis.z += 1.0f;

	if (moving[MOVE_BACK])
		dis.z -= 1.0f;

	D3DXMATRIX mat;
	D3DXMatrixRotationYawPitchRoll(&mat, D3DXToRadian(rot.y), D3DXToRadian(rot.x), D3DXToRadian(rot.z));
	D3DXVec3Normalize(&dis, &dis);
	D3DXVec3TransformCoord(&dis, &dis, &mat);

	return dis * timer::delta_s * speed;
}

D3DXVECTOR3 PhysXPlayer::getWalkDisplacement()
{
	D3DXVECTOR3 dis(0.0f, 0.0f, 0.0f);

	if (moving[MOVE_LEFT])
		dis.x -= 1.0f;

	if (moving[MOVE_RIGHT])
		dis.x += 1.0f;

	if (moving[MOVE_FORWARD])
		dis.z += 1.0f;

	if (moving[MOVE_BACK])
		dis.z -= 1.0f;

	static bool apogee;
	if (moving[MOVE_JUMP] && on_ground)
	{
		jumping = true;
		on_ground = false;
		fall_start = timer::game_ms;
		apogee = false;
		fall_start_height = pos.y;
	}

	D3DXMATRIX mat;
	D3DXMatrixRotationYawPitchRoll(&mat, D3DXToRadian(rot.y), 0, 0);
	D3DXVec3Normalize(&dis, &dis);
	D3DXVec3TransformCoord(&dis, &dis, &mat);
	dis *= timer::delta_s * speed;

	if (!on_ground)
	{
		float air_time = (timer::game_ms - fall_start) / 1000.0f;
		if (jumping)
		{
			float jump_velocity = sqrt(abs(jump_height * (2.0 * gravity)));
			vel.y = jump_velocity + (gravity * air_time);
			//if((vel.y <= 0.0) && !apogee)
			//{
			//	INFO("apogee at %fs, height of %f meters", air_time, pos.y - fall_start_height);
			//	apogee = true;
			//}
		}
		else
			vel.y = gravity * air_time;
		dis.y = vel.y * timer::delta_s;
	}
	else
		dis.y = -step_up;

	return dis;
}

Player* game::createPhysXPlayer(D3DXVECTOR3& size)
{
	return new PhysXPlayer(size);
}
