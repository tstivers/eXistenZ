#include "precompiled.h"
#include "entity/entity.h"
#include "component/contactcallbackcomponent.h"
#include "component/jscomponent.h"

using namespace component;

REGISTER_COMPONENT_TYPE(ContactCallbackComponent, CONTACTCALLBACKCOMPONENT);

ContactCallbackComponent::ContactCallbackComponent(entity::Entity* entity, const string& name, const desc_type& desc)
: Component(entity, name, desc)
{
	m_callback = desc.callback;
}

ContactCallbackComponent::~ContactCallbackComponent()
{
	if(m_acquired)
		release();

	if(m_scriptObject)
		destroyScriptObject();
}

void ContactCallbackComponent::acquire()
{
	if(m_acquired)
		return;

	Component::acquire();
}

void ContactCallbackComponent::release()
{
	Component::release();
}

void ContactCallbackComponent::onContact(Component* sender, const ContactCallbackEventArgs& args)
{
	if(!m_acquired)
		return;

	if(m_callback)
		m_callback(sender, args);
}

JSObject* ContactCallbackComponent::createScriptObject()
{
	return jscomponent::createComponentScriptObject(this);
}

void ContactCallbackComponent::destroyScriptObject()
{
	jscomponent::destroyComponentScriptObject(this);
	m_scriptObject = NULL;
}