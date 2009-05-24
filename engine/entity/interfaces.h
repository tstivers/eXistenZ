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

class IPhysicsObject // not used?
{
public:
	virtual NxActor* getActor() const = 0;
	virtual void updateTransform() = 0;
};

class ICamera
{
public:
};