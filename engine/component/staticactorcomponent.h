#pragma once

#include "component/component.h"
#include "entity/interfaces.h"
#include "component/poscomponent.h"

namespace component
{
	struct StaticActorComponentDesc : public ComponentDesc
	{
		NxShapeDesc* shape;
	};

	class StaticActorComponent : public Component, public IPhysicsObject
	{
	public:
		// typedefs
		typedef StaticActorComponentDesc desc_type;
		typedef StaticActorComponent component_type;

		// constructor/destructor
		StaticActorComponent(entity::Entity* entity, const string& name, const desc_type& desc);
		~StaticActorComponent();

		// Component overloads
		int getType() { return STATICACTORCOMPONENT; }
		void acquire();
		void release();

		// methods
		void setShapesGroup(int group);

		// properties

		// script class
		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		// methods

		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members
		NxActor* m_actor;
	};
}
