#include "precompiled.h"
#include "game/player.h"
#include "game/physxplayer.h"
#include "game/game.h"
#include "physics/physics.h"
#include "console/console.h"
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

		void setPos(D3DXVECTOR3& pos);

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
	desc.extents = NxVec3(0.5, 1.6, 0.5);
	desc.stepOffset = 0.5;

	nxc = physics::gManager->createController(physics::gScene, desc);
	//nxc->setCollision(false);

	acquired = true;
}

void PhysXPlayer::setPos(D3DXVECTOR3& pos)
{
	Player::setPos(pos);
	nxc->setPosition(NxExtendedVec3(pos.x / physics::scale, pos.y / physics::scale, pos.z / physics::scale));
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

	D3DXVECTOR3 normal_dis;
	D3DXVec3Normalize(&normal_dis, &dis);

	D3DXMATRIX mat;	
	D3DXMatrixRotationYawPitchRoll(&mat, rot.x * (D3DX_PI / 180.0f), rot.y * (D3DX_PI / 180.0f), rot.z * (D3DX_PI / 180.0f));

	D3DXVECTOR3 rotated_dis;
	D3DXVec3TransformCoord(&rotated_dis, &normal_dis, &mat);

	return rotated_dis * (timer::delta_ms / 1000.0f) * game::player_speed * physics::scale;
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

	D3DXVECTOR3 normal_dis;
	D3DXVec3Normalize(&normal_dis, &dis);

	D3DXMATRIX mat;	
	D3DXMatrixRotationYawPitchRoll(&mat, rot.x * (D3DX_PI / 180.0f), 0, 0);

	D3DXVECTOR3 rotated_dis;
	D3DXVec3TransformCoord(&rotated_dis, &normal_dis, &mat);
	rotated_dis *= (timer::delta_ms / 1000.0f) * game::player_speed * physics::scale;
	rotated_dis.y += physics::gravity;

	return rotated_dis;
}

Player* game::createPhysXPlayer(D3DXVECTOR3& size)
{
	return new PhysXPlayer(size);
}