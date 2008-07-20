#include "precompiled.h"
#include "component/actorcomponent.h"
#include "component/jscomponent.h"
#include "physics/physics.h"

using namespace component;

REGISTER_COMPONENT_TYPE(ActorComponent, 3);

#pragma warning(disable: 4355) // disable warning for using 'this' as an initializer

ActorComponent::ActorComponent(entity::Entity* entity, const string& name, const desc_type& desc)
: Component(entity, name, desc), m_shapesXml(desc.shapesXml), transform(this)
{
	transform = desc.transform;
}

ActorComponent::~ActorComponent()
{
	if(m_acquired)
		release();

	if(m_scriptObject)
		destroyScriptObject();
}

void ActorComponent::acquire()
{
	Component::acquire();

	NxActorDesc actordesc;
	NxBodyDesc bodydesc;
	actordesc.body = &bodydesc;
	D3DXMatrixToNxMat34(&(transform->getTransform()), &actordesc.globalPose);
	actordesc.userData = dynamic_cast<IPhysicsObject*>(this);
	actordesc.name = m_name.c_str();
	actordesc.density = 10.0;
	if(!m_shapesXml.empty())
	{
		physics::ShapeEntry shapes = physics::getShapeEntry(m_shapesXml);
		if(!shapes)
		{
			INFO("WARNING: unable to acquire shapes \"%s\" for \"%s.%s\"", 
				m_shapesXml.c_str(),
				m_entity->getName().c_str(), m_name.c_str());
		}
		else
		{
			for(physics::ShapeList::const_iterator it = shapes->begin(); it != shapes->end(); ++it)
				actordesc.shapes.pushBack(&**it);
		}
	}

	m_actor = physics::gScene->createActor(actordesc);
	if(!m_actor)
	{
		INFO("WARNING: unable to create actor for \"%s.%s\"", 
			m_entity->getName().c_str(), m_name.c_str());
		m_acquired = false;
		return;
	}

	m_actor->userData = this;

	transform->setSetFunction(bind(&ActorComponent::setTransform, this, _1, _2));
	transform->setGetFunction(bind(&ActorComponent::getTransform, this, _1, _2));
}

void ActorComponent::release()
{
	Component::release();
	transform->setSetFunction(NULL);
	transform->setGetFunction(NULL);
	transform.release();
	physics::gScene->releaseActor(*m_actor);
	m_actor = NULL;
}

void ActorComponent::setLinearVelocity(const D3DXVECTOR3& velocity)
{
	if(!m_acquired || !m_actor)
	{
		INFO("WARNING: tried to set linear velocity on unacquired/invalid ActorComponent \"%s.%s\"", 
			m_entity->getName().c_str(), m_name.c_str());
		return;
	}

	m_actor->setLinearVelocity((NxVec3)velocity);
}

void ActorComponent::setAngularVelocity(const D3DXVECTOR3& velocity)
{
	if(!m_acquired || !m_actor)
	{
		INFO("WARNING: tried to set angular velocity on unacquired/invalid ActorComponent \"%s.%s\"", 
			m_entity->getName().c_str(), m_name.c_str());
		return;
	}

	m_actor->setAngularVelocity((NxVec3)velocity);
}

void ActorComponent::addForce(const D3DXVECTOR3& force)
{
	if(!m_acquired || !m_actor)
	{
		INFO("WARNING: tried to set angular velocity on unacquired/invalid ActorComponent \"%s.%s\"", 
			m_entity->getName().c_str(), m_name.c_str());
		return;
	}

	m_actor->addForce((NxVec3)force);
}

void ActorComponent::addForceType(const D3DXVECTOR3& force, int type)
{
	if(!m_acquired || !m_actor)
	{
		INFO("WARNING: tried to set angular velocity on unacquired/invalid ActorComponent \"%s.%s\"", 
			m_entity->getName().c_str(), m_name.c_str());
		return;
	}

	m_actor->addForce((NxVec3)force, (NxForceMode)type);
}

void ActorComponent::setShapesGroup(int group)
{
	NxShape * const * shape = m_actor->getShapes();
	for(int i = m_actor->getNbShapes() - 1; i >= 0; --i)
	{
		shape[i]->setGroup(group);
		shape++;
	}
}

void ActorComponent::setTransform(D3DXMATRIX& current_transform, const D3DXMATRIX& new_transform)
{
	NxMat34 m;
	D3DXMatrixToNxMat34(&new_transform, &m);
	m_actor->setGlobalPose(m);
	current_transform = new_transform;
}

void ActorComponent::getTransform(D3DXMATRIX& new_transform, const D3DXMATRIX& current_transform)
{
	NxMat34ToD3DXMatrix(&m_actor->getGlobalPose(), &new_transform);
}

JSObject* component::ActorComponent::createScriptObject()
{
	return jscomponent::createComponentScriptObject(this);
}

void component::ActorComponent::destroyScriptObject()
{
	jscomponent::destroyComponentScriptObject(this);
	m_scriptObject = NULL;
}