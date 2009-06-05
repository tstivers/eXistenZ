#pragma once

#include "component/component.h"
#include "entity/interfaces.h"

namespace component
{
	struct ControllerComponentDesc : public ComponentDesc
	{
	};

	class ControllerComponent : public Component, public IInputConsumer
	{
	public:
		// typedefs
		typedef ControllerComponentDesc desc_type;
		typedef ControllerComponent component_type;

		// constructor/destructor
		ControllerComponent(entity::Entity* entity, const string& name, const desc_type& desc);
		~ControllerComponent();

		// Component overloads
		int getType() { return CONTROLLERCOMPONENT; }
		void acquire();
		void release();

		// methods
		

		// properties
		ComponentLink<PosComponent> transform;

		// script class
		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		// methods		
		
		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members
		NxController* m_controller;
		bool m_falling;
		bool m_jumping;
		float m_fallStart;
		float m_fallHeight;
	};
}
