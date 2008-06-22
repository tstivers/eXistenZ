#pragma once

#include "entity/entity.h"
#include "component/component.h"
#include "component/poscomponent.h"
#include "entity/interfaces.h"
#include "mesh/mesh.h"

namespace component
{
	struct MeshComponentDesc : public ComponentDesc
	{
		string mesh;
		string transformComponent;
	};

	class MeshComponent : public Component, public Renderable
	{
	public:
		// typedefs
		typedef MeshComponentDesc desc_type;
		
		// constructor/destructor
		MeshComponent(entity::Entity* entity, const string& name, const desc_type& desc);
		~MeshComponent();

		// component overloads
		int getType() { return 2; }
		void acquire();
		void release();

		// renderable overloads
		D3DXVECTOR3 getRenderOrigin() const;
		void render(texture::Material* lighting);

		// methods

		// properties
		ComponentLink<PosComponent> transform;

		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		// script functions
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members
		string m_meshName;
		mesh::Mesh* m_mesh;
	};
}