#pragma once

#include "component/component.h"
#include "entity/interfaces.h"
#include "component/poscomponent.h"

namespace component
{
	struct CameraComponentDesc : public ComponentDesc
	{
		string transform;
	};

	class CameraComponent : public Component, public ICamera
	{
	public:
		// typedefs
		typedef CameraComponentDesc desc_type;
		typedef CameraComponent component_type;

		// constructor/destructor
		CameraComponent(entity::Entity* entity, const string& name, const desc_type& desc);
		~CameraComponent();

		// Component overloads
		int getType() { return CAMERACOMPONENT; }
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
	};
}
