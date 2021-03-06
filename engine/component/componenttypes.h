#pragma once

#undef CONCAT
#define CONCAT(a,b) a##b
#define REGISTER_COMPONENT_TYPE(name, id) static component::ComponentRegistration CONCAT(name, _registration)(#name, id)

#define ACTORCOMPONENT				3
#define MESHCOMPONENT				2
#define POSCOMPONENT				1
#define JSRENDERCOMPONENT			4
#define STATICACTORCOMPONENT		5
#define CAMERACOMPONENT				6
#define DYNAMICACTORCOMPONENT		7
#define TIMERCOMPONENT				8
#define CONTACTCALLBACKCOMPONENT	9

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