#include "precompiled.h"
#include "component/staticactorcomponent.h"
#include "component/jscomponent.h"
#include "physics/physics.h"

using namespace component;

REGISTER_COMPONENT_TYPE(StaticActorComponent, STATICACTORCOMPONENT);

#pragma warning(disable: 4355) // disable warning for using 'this' as an initializer

StaticActorComponent::StaticActorComponent(entity::Entity* entity, const string& name, const desc_type& desc)
: ActorComponent(entity, name, desc)
{
	NxActorDesc actor_desc;
	actor_desc.shapes.push_back(desc.shape);
	m_actor = m_entity->getScene()->getPhysicsManager()->getPhysicsScene()->createActor(actor_desc);
	
	if(!m_actor)
	{
		INFO("WARNING: unable to create static actor for \"%s.%s\"", 
			m_entity->getName().c_str(), m_name.c_str());
		m_acquired = false;
		return;
	}

	m_actor->userData = this;
}

StaticActorComponent::~StaticActorComponent()
{
	if(m_acquired)
		release();

	if(m_scriptObject)
		destroyScriptObject();
}

void StaticActorComponent::acquire()
{
	ActorComponent::acquire();
}

void StaticActorComponent::release()
{
	if(!m_acquired)
		return;	
	ActorComponent::release();
}


JSObject* component::StaticActorComponent::createScriptObject()
{
	return jscomponent::createComponentScriptObject(this);
}

void component::StaticActorComponent::destroyScriptObject()
{
	jscomponent::destroyComponentScriptObject(this);
	m_scriptObject = NULL;
}