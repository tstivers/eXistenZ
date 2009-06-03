#include "precompiled.h"
#include "entity/entity.h"
#include "entity/jsentity.h"
#include "component/component.h"

namespace entity
{
}

using namespace entity;

Entity::Entity(EntityManager* manager, const string& name)
: m_manager(manager), m_name(name), m_position(NULL), m_acquired(false), m_flags(0)
{
}

Entity::~Entity()
{
	if(m_acquired)
		release();

	// clear out our components so that things get destroyed in the right order
	m_components.clear();

	if(m_scriptObject)
		destroyScriptObject();
}

void Entity::addComponent(const string& name, shared_ptr<component::Component> component)
{
	ASSERT(getComponent(name) == NULL);
	m_components.insert(component_map::value_type(name, component));

	if(name == "pos")
		m_position = component.get();
}

component::Component* Entity::getComponent(const string& name)
{
	component_map::iterator it = m_components.find(name);
	if(it != m_components.end())
		return it->second.get();
	else
		return NULL;
}

void Entity::removeComponent(const string& name)
{
	component_map::iterator it = m_components.find(name);
	if(it == m_components.end())
		return;

	m_manager->addToFreeList((*it).second);
	it->second->release();
	m_components.erase(it);

	if(name == "pos")
		m_position = NULL;
}

void Entity::acquire()
{
	for(component_map::iterator it = m_components.begin(); it != m_components.end(); ++it)
		it->second->acquire();
	m_acquired = true;
}

void Entity::release()
{
	for(component_map::iterator it = m_components.begin(); it != m_components.end(); ++it)
		it->second->release();
	m_acquired = false;
}

JSObject* Entity::createScriptObject()
{
	return jsentity::createEntityObject(this);
}

void Entity::destroyScriptObject()
{
	jsentity::destroyEntityObject(this);
	m_scriptObject = NULL;
}
