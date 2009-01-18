#include "precompiled.h"
#include "component/staticactorcomponent.h"
#include "component/jscomponent.h"
#include "physics/physics.h"

using namespace component;

REGISTER_COMPONENT_TYPE(StaticActorComponent, STATICACTORCOMPONENT);

#pragma warning(disable: 4355) // disable warning for using 'this' as an initializer

StaticActorComponent::StaticActorComponent(entity::Entity* entity, const string& name, const desc_type& desc)
: Component(entity, name, desc)
{
	
	NxActorDesc actor_desc;
	actor_desc.shapes.push_back(desc.shape);
	m_actor = physics::gScene->createActor(actor_desc);
	
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
	Component::acquire();
}

void StaticActorComponent::release()
{
	Component::release();
	physics::gScene->releaseActor(*m_actor);
	m_actor = NULL;
}

void StaticActorComponent::setShapesGroup(int group)
{
	NxShape * const * shape = m_actor->getShapes();
	for(int i = m_actor->getNbShapes() - 1; i >= 0; --i)
	{
		shape[i]->setGroup(group);
		shape++;
	}
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