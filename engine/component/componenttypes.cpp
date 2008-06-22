#include "precompiled.h"
#include "component/componenttypes.h"
#include "common/wrapped_global.h"

namespace component
{
	typedef boost::bimap<string, int> type_map;
	wrapped_global<type_map> component_types;
}


void component::registerComponentType(const string& name, int id)
{
	ASSERT(component_types->left.find(name) == component_types->left.end());
	ASSERT(component_types->right.find(id) == component_types->right.end());
	component_types->insert(type_map::value_type(name, id));
}

int component::getComponentTypeId(const string& name)
{
	return component_types->left.at(name);
}


const string& component::getComponentTypeName(int id)
{
	return component_types->right.at(id);
}

