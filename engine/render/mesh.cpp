/////////////////////////////////////////////////////////////////////////////
// render.cpp
// rendering system implementation
// $Id: mesh.cpp,v 1.1 2003/11/24 00:16:13 tstivers Exp $
//

#include "precompiled.h"
#include "render/render.h"

namespace render {
}

using namespace render;

Mesh::Mesh()
{
	ZeroMemory(this, sizeof(Mesh));
	for(int i = 0; i < 3; i++) {
		bounds[0][i] = BIGFLOAT;
		bounds[1][i] = -BIGFLOAT;
	}
}