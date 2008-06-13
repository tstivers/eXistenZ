#include "precompiled.h"
#include "physics/shapecache.h"
#include "physics/xmlloader.h"

namespace physics
{
	typedef map<string, ShapeEntry> entry_map_type;
	entry_map_type entry_map;
}

using namespace physics;

ShapeEntry physics::getShapeEntry(const string& name)
{
	entry_map_type::iterator it = entry_map.find(name);
	if (it == entry_map.end())
	{
		ShapeEntry entry = loadDynamicsXML(name);
		if(!entry)
		{
			INFO("ERROR: unable to load shape data from %s", name.c_str());
			return entry;
		}

		entry_map[name] = entry;
		return entry;
	}

	return it->second;
}