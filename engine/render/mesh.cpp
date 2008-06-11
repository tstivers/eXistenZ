/////////////////////////////////////////////////////////////////////////////
// render.cpp
// rendering system implementation
// $Id$
//

#include "precompiled.h"
#include "render/render.h"

namespace render
{
}

using namespace render;

Mesh::Mesh()
{
	ZeroMemory(this, sizeof(Mesh));
	bbox.min = D3DXVECTOR3(BIGFLOAT, BIGFLOAT, BIGFLOAT);
	bbox.max = D3DXVECTOR3(-BIGFLOAT, -BIGFLOAT, -BIGFLOAT);
}