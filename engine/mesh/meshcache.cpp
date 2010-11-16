#include "precompiled.h"
#include "mesh/meshcache.h"
#include "mesh/mesh.h"
#include "mesh/meshload.h"

namespace mesh
{
	typedef stdext::hash_map<string, Mesh*> MeshCache;
	MeshCache cache;
}

using namespace mesh;

Mesh* mesh::getMesh(string& name)
{
	// see if it's in the cache
	auto it = cache.find(name);
	if (it != cache.end())
		return it->second;

	// nope, try to load it
	auto mesh = loadMesh(name);
	if (mesh)
		return mesh;

	LOG("failed to find mesh \"%s\"", name.c_str());

	return NULL;
}

bool mesh::addMesh( Mesh* mesh )
{
	if(cache.find(mesh->name) != cache.end())
		return false;

	cache.insert(MeshCache::value_type(mesh->name, mesh));
	INFO("cached mesh: \"%s\"", mesh->name.c_str());
	return true;
}