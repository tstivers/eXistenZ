#include "precompiled.h"
#include "component/jsrendercomponent.h"
#include "component/jscomponent.h"
#include "entity/entitymanager.h"

using namespace component;

REGISTER_COMPONENT_TYPE(JSRenderComponent, JSRENDERCOMPONENT);

#pragma warning(disable: 4355) // disable warning for using 'this' as an initializer

JSRenderComponent::JSRenderComponent(entity::Entity* entity, const string& name, const desc_type& desc)
: Component(entity, name, desc), transform(this)
{
	transform = desc.transformComponent;
}

JSRenderComponent::~JSRenderComponent()
{
	if(m_acquired)
		release();

	if(m_scriptObject)
		destroyScriptObject();
}

void JSRenderComponent::acquire()
{
	if(m_acquired)
		return;

	if(!transform)
	{
		INFO("WARNING: unable to acquire transform for \"%s.%s\"",
			m_entity->getName().c_str(), m_name.c_str());
		return;
	}

	m_entity->getManager()->getScene()->addRenderable(this);
	Component::acquire();
}

void JSRenderComponent::release()
{
	Component::release();
	transform.release();
	m_entity->getManager()->getScene()->removeRenderable(this);
}

D3DXVECTOR3 JSRenderComponent::getRenderOrigin() const
{
	ASSERT(transform);
	return transform->getPos();
}

void JSRenderComponent::render(texture::Material* lighting)
{
	ASSERT(m_acquired);
	ASSERT(transform);
}

JSObject* JSRenderComponent::createScriptObject()
{
	return jscomponent::createComponentScriptObject(this);
}

void JSRenderComponent::destroyScriptObject()
{
	jscomponent::destroyComponentScriptObject(this);
	m_scriptObject = NULL;
}

