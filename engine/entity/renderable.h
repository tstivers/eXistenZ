#pragma once

#include "texture/material.h"

namespace entity
{
	class Renderable
	{
	public:
		virtual D3DXVECTOR3 getRenderOrigin() const = 0;
		virtual void render(texture::Material* lighting) = 0;
	};
}