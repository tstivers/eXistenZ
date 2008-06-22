#pragma once

#include "texture/material.h"
#include <NxPhysics.h>

class Renderable
{
public:
	virtual D3DXVECTOR3 getRenderOrigin() const = 0;
	virtual void render(texture::Material* lighting) = 0;
};

class PhysicsObject
{
public:
	virtual NxActor* getActor() { return NULL; }
	virtual void updateTransform() {}
};
