#pragma once

#undef CONCAT
#define CONCAT(a,b) a##b
#define REGISTER_COMPONENT_TYPE(name, id) static component::ComponentRegistration CONCAT(name, _registration)(#name, id)

namespace component
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