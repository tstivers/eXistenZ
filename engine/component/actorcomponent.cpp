#include "precompiled.h"
#include "component/actorcomponent.h"
#include "component/jscomponent.h"
#include "physics/physics.h"

using namespace component;

REGISTER_COMPONENT_TYPE(ActorComponent, ACTORCOMPONENT);

#pragma warning(disable: 4355) // disable warning for using 'this' as an initializer

ActorComponent::ActorComponent(entity::Entity* entity, const string& name, const desc_type& desc)
: Component(entity, name, desc), m_actor(NULL), contactCallback(this)
{	
}

ActorComponent::~ActorComponent()
{
	if(m_acquired)
		release();
}

void ActorComponent::acquire()
{
	Component::acquire();
}

void ActorComponent::release()
{	
	if(!m_acquired)
		return;

	m_entity->getScene()->getPhysicsManager()->getPhysicsScene()->releaseActor(*m_actor);
	m_actor = NULL;
	Component::release();
}

void ActorComponent::setShapesGroup(int group)
{
	if(!m_acquired)
		return;
	
	ASSERT(m_actor);

	NxShape * const * shape = m_actor->getShapes();
	for(int i = m_actor->getNbShapes() - 1; i >= 0; --i)
	{
		shape[i]->setGroup(group);
		shape++;
	}
}

void ActorComponent::setContactReportFlags(int flags)
{
	if(!m_acquired)
		return;

	ASSERT(m_actor);

	m_actor->setContactReportFlags(flags);
}