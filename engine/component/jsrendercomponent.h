#pragma once

#include "component/component.h"
#include "component/poscomponent.h"
#include "entity/interfaces.h"

namespace component
{
	struct JSRenderComponentDesc : public ComponentDesc
	{
		string transformComponent;
	};

	class JSRenderComponent : public Component, public IRenderable
	{
	public:
		// typedefs
		typedef JSRenderComponentDesc desc_type;
		typedef JSRenderComponent component_type;

		// constructor/destructor
		JSRenderComponent(entity::Entity* entity, const string& name, const desc_type& desc);
		~JSRenderComponent();

		// Component overloads
		int getType() { return JSRENDERCOMPONENT; }
		void acquire();
		void release();

		// IRenderable overloads
		D3DXVECTOR3 getRenderOrigin() const;
		void render(texture::Material* lighting);

		// methods
		
		// properties
		ComponentLink<PosComponent> transform;

		// script class
		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members
	};
}
