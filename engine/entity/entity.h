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
		template<typename T>
		T* createComponent(const typename T::desc_type& desc);
		virtual Component* getComponent(const string& name);
		virtual void removeComponent(const string& name);

		// component shortcuts
		virtual Component* getPositionComponent(); // shortcut for getComponent("pos")

		// acquire/release methods
		virtual void acquire();
		virtual void release();

	protected:

		// members
		string m_name;
		EntityManager* m_manager;
	};
}