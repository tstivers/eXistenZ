#include "precompiled.h"
#include "game/player.h"
#include "game/physxplayer.h"
#include "game/game.h"
#include "physics/physics.h"
#include "timer/timer.h"
#include <NxPhysics.h>
#include <NxController.h>
#include <NxControllerManager.h>
#include <NxCapsuleController.h>
#include <NxBoxController.h>

namespace game
{
	class PhysXPlayer : public Player {
	public:
		PhysXPlayer(D3DXVECTOR3& size);
		~PhysXPlayer();

		void acquire();
		void release();

		bool setPos(D3DXVECTOR3& pos);		
		bool setSize(D3DXVECTOR3& size);
		bool setStepUp(float step_up);

		void doMove(t_impulse impulse);
		void doRotation(D3DXVECTOR3& rotation);
		void updatePos();

		D3DXVECTOR3 getFlyDisplacement();
		D3DXVECTOR3 getWalkDisplacement();

		NxController* nxc;
		bool acquired;
		bool in_air;		
		bool moving[MOVE_MAX];
	};
}

namespace physics {
	extern NxControllerManager* gManager;
	extern NxScene* gScene;
	extern float gravity;
}


using namespace game;

PhysXPlayer::PhysXPlayer(D3DXVECTOR3& size)
: Player(size), acquired(false)
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

	if(acquired)
		nxc->setStepOffset(this->step_up / physics::scale);
		
	return true;
}

bool PhysXPlayer::setSize(D3DXVECTOR3& size)
{
	if(!Player::setSize(size))
		return false;

	// TODO: figure this out
	//if(!nxc->setExtents(this->size / physics::scale))
	//	return false;

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

	nxc->move((NxVec3&)dis, collisionGroups, 0.0001f, flags);

	physics::gManager->updateControllers();
	
	NxExtendedVec3 newpos = nxc->getFilteredPosition();

	this->pos = D3DXVECTOR3(newpos.x, newpos.y, newpos.z) * physics::scale;

	for(int i = 0; i < MOVE_MAX; i++)
		moving[i] = false;
}

D3DXVECTOR3 PhysXPlayer::getFlyDisplacement()
{
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

	return dis * (timer::delta_ms / 1000.0f) * speed;
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

	D3DXMATRIX mat;	
	D3DXMatrixRotationYawPitchRoll(&mat, rot.x * (D3DX_PI / 180.0f), 0, 0);
	D3DXVec3Normalize(&dis, &dis);
	D3DXVec3TransformCoord(&dis, &dis, &mat);
	dis *= (timer::delta_ms / 1000.0f) * speed;
	dis.y += physics::gravity * (timer::delta_ms / 1000.0f);

	return dis;
}

Player* game::createPhysXPlayer(D3DXVECTOR3& size)
{
	return new PhysXPlayer(size);
}