/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: meshload.cpp,v 1.1 2003/12/04 12:33:48 tstivers Exp $
//

#include "precompiled.h"
#include "mesh/meshload.h"
#include "mesh/textloader.h"

namespace mesh {
};

using namespace mesh;

Mesh* mesh::loadMesh(const std::string& name)
{
	std::string filename;
	Mesh* mesh;

	// try to load a text mesh;
	mesh = loadTextMesh(name + EXTENSION_TEXTMESH);
	if(mesh)
		return mesh;

	// TODO: other formats here
	return NULL;
}

MeshSystem* mesh::loadMeshSystem(const std::string& name)
{
	std::string filename;
	MeshSystem* meshsys;

	// try to load a text mesh;
	meshsys = loadTextMeshSystem(name + EXTENSION_TEXTMESHSYSTEM);
	if(meshsys)
		return meshsys;

	// TODO: other formats here
	return NULL;
}