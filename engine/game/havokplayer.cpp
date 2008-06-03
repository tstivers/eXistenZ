#include "precompiled.h"
#include "game/player.h"
#include "game/havokplayer.h"
#include "game/game.h"
#include "physics/physics.h"
#include "timer/timer.h"

namespace game
{
	class HavokPlayer : public Player {
	public:
		HavokPlayer(D3DXVECTOR3& size);
		~HavokPlayer();

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

		bool acquired;
		bool in_air;
		bool jumping;
		bool moving[MOVE_MAX];
		float fall_start;
		float fall_start_height;

		// havok junk
		hkpShape* m_standShape;
		hkpShape* m_crouchShape;
		hkpSimpleShapePhantom* m_phantom;
		hkpCharacterProxy* m_characterProxy;
		hkpCharacterContext* m_characterContext;
	};
}

using namespace game;

HavokPlayer::HavokPlayer(D3DXVECTOR3& size)
: Player(size), acquired(false), in_air(false), jumping(false)
{	
}

HavokPlayer::~HavokPlayer()
{
	if(acquired)
		release();
}

void HavokPlayer::acquire()
{
	if(acquired)
		return;

	return;

	//
	//	Create a character proxy object
	//
	{
		// Construct a shape

		hkVector4 vertexA(0,0, 0.4f);
		hkVector4 vertexB(0,0,-0.4f);		

		// Create a capsule to represent the character standing
		m_standShape = new hkpCapsuleShape(vertexA, vertexB, .6f);

		// Create a capsule to represent the character crouching
		// Note that we create the smaller capsule with the base at the same position as the larger capsule.
		// This means we can simply swap the shapes without having to reposition the character proxy,
		// and if the character is standing on the ground, it will still be on the ground.
		vertexA.setZero4();
		m_crouchShape = new hkpCapsuleShape(vertexA, vertexB, .6f);


		// Construct a Shape Phantom
		m_phantom = new hkpSimpleShapePhantom( m_standShape, hkTransform::getIdentity(), hkpGroupFilter::calcFilterInfo(0,2) );

		// Add the phantom to the world
		physics::getWorld()->addPhantom(m_phantom);
		m_phantom->removeReference();

		// Construct a character proxy
		hkpCharacterProxyCinfo cpci;
		cpci.m_position.set(-9.1f, 35, .4f);
		cpci.m_staticFriction = 0.0f;
		cpci.m_dynamicFriction = 1.0f;
		cpci.m_up.setNeg4( physics::getWorld()->getGravity() );
		cpci.m_up.normalize3();	
		cpci.m_userPlanes = 4;
		cpci.m_maxSlope = HK_REAL_PI / 3.f;

		cpci.m_shapePhantom = m_phantom;
		m_characterProxy = new hkpCharacterProxy( cpci );
	}

	//
	// Create the Character state machine and context
	//
	{
		hkpCharacterState* state;
		hkpCharacterStateManager* manager = new hkpCharacterStateManager();

		state = new hkpCharacterStateOnGround();
		manager->registerState( state,	HK_CHARACTER_ON_GROUND);
		state->removeReference();

		state = new hkpCharacterStateInAir();
		manager->registerState( state,	HK_CHARACTER_IN_AIR);
		state->removeReference();

		state = new hkpCharacterStateJumping();
		manager->registerState( state,	HK_CHARACTER_JUMPING);
		state->removeReference();

		state = new hkpCharacterStateClimbing();
		manager->registerState( state,	HK_CHARACTER_CLIMBING);
		state->removeReference();

		m_characterContext = new hkpCharacterContext(manager, HK_CHARACTER_ON_GROUND);
		manager->removeReference();
	}

	acquired = true;
}

bool HavokPlayer::setPos(D3DXVECTOR3& pos)
{
	if(!Player::setPos(pos))
		return false;
	
	return true;
}

bool HavokPlayer::setStepUp(float step_up)
{
	if(!Player::setStepUp(step_up))
		return false;

	return true;
}

bool HavokPlayer::setMoveMode( t_movemode mode )
{
	if(!Player::setMoveMode(mode))
		return false;

	return true;
}

bool HavokPlayer::setSize(D3DXVECTOR3& size)
{
	if(!Player::setSize(size))
		return false;

	return true;
}

void HavokPlayer::release()
{
	acquired = false;
}

void HavokPlayer::doMove(t_impulse impulse)
{
	moving[impulse] = true;
}

void HavokPlayer::doRotation(D3DXVECTOR3& rotation)
{
	rot += rotation;	
	if(rot.x > 360.0f) rot.x -= 360.0f;
	if(rot.x < 0.0f) rot.x += 360.0f;
	if(rot.y > 90.0f) rot.y = 90.0f;
	if(rot.y < -90.0f) rot.y = -90.0f;
}

void HavokPlayer::updatePos()
{
	if(!acquired)
		return;
}

D3DXVECTOR3 HavokPlayer::getFlyDisplacement()
{
	D3DXVECTOR3 dis(0.0f, 0.0f, 0.0f);

	return dis;
}

D3DXVECTOR3 HavokPlayer::getWalkDisplacement()
{
	D3DXVECTOR3 dis(0.0f, 0.0f, 0.0f);

	return dis;
}

Player* game::createHavokPlayer(D3DXVECTOR3& size)
{
	return new HavokPlayer(size);
}