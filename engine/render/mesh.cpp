/////////////////////////////////////////////////////////////////////////////
// render.cpp
// rendering system implementation
// $Id: mesh.cpp,v 1.2 2003/11/25 22:57:23 tstivers Exp $
//

#include "precompiled.h"
#include "render/render.h"

namespace render {
}

using namespace render;

Mesh::Mesh()
{
	ZeroMemory(this, sizeof(Mesh));
	bbox.min = D3DXVECTOR3(BIGFLOAT, BIGFLOAT, BIGFLOAT);
	bbox.max = D3DXVECTOR3(-BIGFLOAT, -BIGFLOAT, -BIGFLOAT);
}