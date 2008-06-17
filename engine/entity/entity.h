#pragma once

#include "script/script.h"

namespace entity
{
	class Component;
	struct ComponentDesc;
	class EntityManager;

	class Entity : public script::ScriptedObject
	{
	public:
		// typedefs

		// constructor/destructor
		Entity(EntityManager* manager, const string& name);
		virtual ~Entity();

		// common entity functions
		virtual const string& getName() { return m_name; }
		virtual EntityManager* getManager() { return m_manager; }

		// component functions
		virtual void addComponent(const string& name, shared_ptr<Component> component);
		virtual Component* getComponent(const string& name);
		virtual void removeComponent(const string& name);

		// component shortcuts
		virtual Component* getPositionComponent() { return m_position; } // shortcut for getComponent("pos")

		// acquire/release methods
		virtual void acquire();
		virtual void release();

	protected:
		// typedefs
		typedef map<string, shared_ptr<Component>> component_map;

		// script functions
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members
		string m_name;
		EntityManager* m_manager;
		component_map m_components;
		Component* m_position;
		bool m_acquired;
	};
}