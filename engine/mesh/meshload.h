/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: meshload.h,v 1.1 2003/12/04 12:33:48 tstivers Exp $
//

#pragma once

namespace mesh {

	class Mesh;
	class MeshSystem;

	Mesh* loadMesh(const std::string& name);
	MeshSystem* loadMeshSystem(const std::string& name);
}