/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: player.cpp 66 2007-03-12 03:37:50Z tstivers $
//

#include "precompiled.h"
#include "game/player.h"
#include "game/bspplayer.h"
#include "game/physxplayer.h"

namespace game {

}

using namespace game;

Player::Player(D3DXVECTOR3& size)
: size(size), pos(D3DXVECTOR3(0, 0, 0)), rot(D3DXVECTOR3(0, 0, 0)), vel(D3DXVECTOR3(0, 0, 0)),
mode(MM_FLY), collide(false)
{
}

Player::~Player()
{
}

Player* game::createPlayer(D3DXVECTOR3& size)
{
	return createPhysXPlayer(size);
}