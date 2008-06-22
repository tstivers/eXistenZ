#pragma once

#include "entity/component.h"
#include "entity/interfaces.h"
#include "entity/poscomponent.h"

namespace entity
{
	struct ActorComponentDesc : public ComponentDesc
	{
		string shapesXml;
		string transform;
	};

	class ActorComponent : public Component, public PhysicsObject
	{
	public:
		// typedefs
		typedef ActorComponentDesc desc_type;
		typedef ActorComponent component_type;

		// constructor/destructor
		ActorComponent(Entity* entity, const string& name, const desc_type& desc);
		~ActorComponent();

		// Component overloads
		int getType() { return 3; }
		void acquire();
		void release();

		// methods
		void setLinearVelocity(const D3DXVECTOR3& velocity);
		void setAngularVelocity(const D3DXVECTOR3& velocity);

		// properties
		ComponentLink<PosComponent> transform;

	protected:
		// methods
		void setTransform(D3DXMATRIX& current_transform, const D3DXMATRIX& new_transform);
		void getTransform(D3DXMATRIX& new_transform, const D3DXMATRIX& current_transform);

		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();


		// members
		string m_shapesXml;
		NxActor* m_actor;
	};
}
