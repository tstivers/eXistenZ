#pragma once

#include "component/component.h"
#include "component/actorcomponent.h"
#include "entity/interfaces.h"
#include "component/poscomponent.h"

namespace component
{
	struct DynamicActorComponentDesc : public ActorComponentDesc
	{
		string shapesXml;
		string transform;
	};

	class DynamicActorComponent : public ActorComponent
	{
	public:
		// typedefs
		typedef DynamicActorComponentDesc desc_type;
		typedef DynamicActorComponent component_type;

		// constructor/destructor
		DynamicActorComponent(entity::Entity* entity, const string& name, const desc_type& desc);
		~DynamicActorComponent();

		// Component overrides
		int getType() { return DYNAMICACTORCOMPONENT; }
		void acquire();
		void release();

		// IPhysicsObject overrides
		
		// ActorComponent overrides

		// methods
		void setLinearVelocity(const D3DXVECTOR3& velocity);
		void setAngularVelocity(const D3DXVECTOR3& velocity);
		void addForce(const D3DXVECTOR3& force);
		void addForceType(const D3DXVECTOR3& force, int type);		

		// properties
		ComponentLink<PosComponent> transform;

		// script class
		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		// methods
		void setTransform(D3DXMATRIX& current_transform, const D3DXMATRIX& new_transform);
		void getTransform(D3DXMATRIX& new_transform, const D3DXMATRIX& current_transform);

		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members
		string m_shapesXml;		
	};
}
