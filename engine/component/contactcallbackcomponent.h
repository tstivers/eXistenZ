#pragma once

#include "component/component.h"
#include "entity/interfaces.h"

namespace component
{
	struct ContactCallbackEventArgs
	{		
		Component* otherComponent;
		D3DXVECTOR3 contactForce;

		ContactCallbackEventArgs(Component* otherComponent, D3DXVECTOR3& contactForce)
			: otherComponent(otherComponent), contactForce(contactForce)
		{}
	};

	typedef function<void(Component*, const ContactCallbackEventArgs&)> ContactCallbackFunction;

	struct ContactCallbackComponentDesc : public ComponentDesc
	{
		ContactCallbackFunction callback;

		ContactCallbackComponentDesc()
			: callback(NULL)
		{}
	};

	class ContactCallbackComponent : public Component
	{
	public:
		// typedefs
		typedef ContactCallbackComponentDesc desc_type;
		typedef ContactCallbackComponent component_type;

		// constructor/destructor
		ContactCallbackComponent(entity::Entity* entity, const string& name, const desc_type& desc);
		~ContactCallbackComponent();

		// Component overloads
		int getType() { return CONTACTCALLBACKCOMPONENT; }
		void acquire();
		void release();

		// methods
		void onContact(Component* sender, const ContactCallbackEventArgs& args);

		// properties
		ContactCallbackFunction getCallback() { return m_callback; }
		void setCallback(ContactCallbackFunction callback) { m_callback = callback; }

		// script class
		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		// methods		
		
		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members		
		ContactCallbackFunction m_callback;
		bool m_started;
	};
}
