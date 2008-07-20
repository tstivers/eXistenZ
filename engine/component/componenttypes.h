#pragma once

#undef CONCAT
#define CONCAT(a,b) a##b
#define REGISTER_COMPONENT_TYPE(name, id) static component::ComponentRegistration CONCAT(name, _registration)(#name, id)

#define ACTORCOMPONENT		3
#define MESHCOMPONENT		2
#define POSCOMPONENT		1
#define JSRENDERCOMPONENT	4

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