/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: meshsystemcache.cpp,v 1.1 2003/12/04 12:33:48 tstivers Exp $
//

#include "precompiled.h"
#include "mesh/meshsystemcache.h"
#include "mesh/meshsystem.h"
#include "mesh/meshload.h"

namespace mesh {

	typedef stdext::hash_map<std::string, MeshSystem*> MeshSystemCache;
	MeshSystemCache cache;
};

using namespace mesh;

MeshSystem* mesh::getMeshSystem(std::string& name)
{
	// see if it's in the cache
	MeshSystemCache::iterator it = cache.find(name);
	if(it != cache.end())
		return (*it).second;

	// nope, try to load it
	MeshSystem* mesh = loadMeshSystem(name);	
	if(mesh) {
		// found it, add it to the cache
		cache.insert(MeshSystemCache::value_type(name, mesh));
		return mesh;
	}

	return NULL;
}