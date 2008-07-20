#pragma once

#include "texture/material.h"
#include <NxPhysics.h>

class IRenderable
{
public:
	// used to get origin for lighting calcs in the bsp renderpath
	virtual D3DXVECTOR3 getRenderOrigin() const = 0;
	virtual void render(texture::Material* lighting) = 0;
};

class IPhysicsObject
{
public:
	virtual NxActor* getActor() { return NULL; }
	virtual void updateTransform() {}
};
