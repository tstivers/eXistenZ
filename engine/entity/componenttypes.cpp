#include "precompiled.h"
#include "entity/componenttypes.h"
#include "common/wrapped_global.h"

namespace entity
{
	typedef boost::bimap<string, int> type_map;
	wrapped_global<type_map> component_types;
}


void entity::registerComponentType(const string& name, int id)
{
	ASSERT(component_types->left.find(name) == component_types->left.end());
	ASSERT(component_types->right.find(id) == component_types->right.end());
	component_types->insert(type_map::value_type(name, id));
}

int entity::getComponentTypeId(const string& name)
{
	return component_types->left.at(name);
}


const string& entity::getComponentTypeName(int id)
{
	return component_types->right.at(id);
}

