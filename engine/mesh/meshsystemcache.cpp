#include "precompiled.h"
#include "mesh/meshsystemcache.h"
#include "mesh/meshsystem.h"
#include "mesh/meshload.h"

namespace mesh
{

	typedef stdext::hash_map<string, MeshSystem*> MeshSystemCache;
	MeshSystemCache cache;
};

using namespace mesh;

MeshSystem* mesh::getMeshSystem(string& name)
{
	// see if it's in the cache
	MeshSystemCache::iterator it = cache.find(name);
	if (it != cache.end())
		return it->second;

	// nope, try to load it
	MeshSystem* mesh = loadMeshSystem(name);
	if (mesh)
	{
		// found it, add it to the cache
		cache.insert(MeshSystemCache::value_type(name, mesh));
		return mesh;
	}

	LOG("failed to find meshsystem \"%s\"", name.c_str());

	return NULL;
}