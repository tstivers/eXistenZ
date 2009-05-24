#include "precompiled.h"
#include "component/cameracomponent.h"
#include "component/jscomponent.h"
#include "entity/entitymanager.h"

using namespace component;

REGISTER_COMPONENT_TYPE(CameraComponent, CAMERACOMPONENT);

#pragma warning(disable: 4355) // disable warning for using 'this' as an initializer

CameraComponent::CameraComponent(entity::Entity* entity, const string& name, const desc_type& desc)
: Component(entity, name, desc), transform(this)
{
	transform = desc.transform;
}

CameraComponent::~CameraComponent()
{
	if(m_acquired)
		release();

	if(m_scriptObject)
		destroyScriptObject();
}

void CameraComponent::acquire()
{
	if(m_acquired)
		return;

	m_entity->getManager()->getScene()->addCamera(this);

	Component::acquire();
}

void CameraComponent::release()
{
	Component::release();
	m_entity->getManager()->getScene()->removeCamera(this);
	transform.release();
}

JSObject* CameraComponent::createScriptObject()
{
	return jscomponent::createComponentScriptObject(this);
}

void CameraComponent::destroyScriptObject()
{
	jscomponent::destroyComponentScriptObject(this);
	m_scriptObject = NULL;
}