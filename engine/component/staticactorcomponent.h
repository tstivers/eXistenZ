#pragma once

#include "component/component.h"
#include "entity/interfaces.h"
#include "component/poscomponent.h"
#include "component/actorcomponent.h"

namespace component
{
	struct StaticActorComponentDesc : public ActorComponentDesc
	{
		NxShapeDesc* shape;
	};

	class StaticActorComponent : public ActorComponent
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

		// IPhysicsObject overrides

		// methods

		// properties

		// script class
		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		// methods

		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members		
	};
}
