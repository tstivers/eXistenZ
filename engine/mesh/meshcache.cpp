#include "precompiled.h"
#include "mesh/meshcache.h"
#include "mesh/mesh.h"
#include "mesh/meshload.h"
#include "console/console.h"

namespace mesh {

	typedef stdext::hash_map<std::string, Mesh*> MeshCache;
	MeshCache cache;
};

using namespace mesh;

Mesh* mesh::getMesh(std::string& name)
{
	// see if it's in the cache
	MeshCache::iterator it = cache.find(name);
	if(it != cache.end())
		return (*it).second;

	// nope, try to load it
	Mesh* mesh = loadMesh(name);	
	if(mesh) {
		// found it, add it to the cache
		cache.insert(MeshCache::value_type(name, mesh));
		return mesh;
	}

	LOG2("[mesh::getMesh] failed to find mesh \"%s\"", name.c_str());

	return NULL;
}