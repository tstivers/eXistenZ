/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

namespace mesh {

	class Mesh;
	class MeshSystem;

	Mesh* loadMesh(const std::string& name);
	MeshSystem* loadMeshSystem(const std::string& name);
}