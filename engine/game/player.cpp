/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id$
//

#include "precompiled.h"
#include "game/player.h"
#include "game/bspplayer.h"
#include "game/physxplayer.h"

namespace game {

}

using namespace game;

Player::Player(D3DXVECTOR3& size)
: size(size)
{	
}

Player::~Player()
{
}

Player* game::createPlayer(D3DXVECTOR3& size)
{
	return createBSPPlayer(size);
}