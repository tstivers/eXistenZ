#pragma once

#include "component/component.h"
#include "entity/interfaces.h"
#include "component/poscomponent.h"

namespace component
{
	struct ActorComponentDesc : public ComponentDesc
	{		
	};

	class ActorComponent : public Component, public IPhysicsObject
	{
	public:
		// typedefs
		typedef ActorComponentDesc desc_type;
		typedef ActorComponent component_type;

		// constructor/destructor
		ActorComponent(entity::Entity* entity, const string& name, const desc_type& desc);
		~ActorComponent();

		// Component overrides
		int getType() { return ACTORCOMPONENT; }
		void acquire();
		void release();

		// IPhysicsObject overrides
		NxActor* getActor() const { return m_actor; }
		void updateTransform() {};

		// methods
		virtual void setShapesGroup(int group);

		// properties
		
		// script class
		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		// methods

		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members
		string m_shapesXml;
		NxActor* m_actor;
	};
}
