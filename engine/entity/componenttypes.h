#pragma once

#undef CONCAT
#define CONCAT(a,b) a##b
#define REGISTER_COMPONENT_TYPE(name, id) static entity::ComponentRegistration CONCAT(name, _registration)(#name, id)

namespace entity
{
	void registerComponentType(const string& name, int id);
	int getComponentTypeId(const string& name);
	const string& getComponentTypeName(int id);

	struct ComponentRegistration
	{
		ComponentRegistration(const string& name, int id)
		{
			registerComponentType(name, id);
		}
	};
}