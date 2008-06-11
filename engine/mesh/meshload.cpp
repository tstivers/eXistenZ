#include "precompiled.h"
#include "mesh/meshload.h"
#include "mesh/textloader.h"
#include "mesh/mesh.h"
#include "mesh/meshsystem.h"

namespace mesh
{
	const static string EXTENSION_TEXTMESH = ".tm";
	const static string EXTENSION_TEXTMESHSYSTEM = ".tms";
};

using namespace mesh;

Mesh* mesh::loadMesh(const string& name)
{
	string filename;
	Mesh* mesh;

	// try to load a text mesh;
	mesh = loadTextMesh(name + EXTENSION_TEXTMESH);
	if (mesh)
	{
		mesh->name = name;
		return mesh;
	}

	// TODO: other formats here
	return NULL;
}

MeshSystem* mesh::loadMeshSystem(const string& name)
{
	string filename;
	MeshSystem* meshsys;

	// try to load a text mesh;
	meshsys = loadTextMeshSystem(name + EXTENSION_TEXTMESHSYSTEM);
	if (meshsys)
	{
		meshsys->name = name;
		return meshsys;
	}

	// TODO: other formats here
	return NULL;
}