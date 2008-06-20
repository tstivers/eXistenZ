#pragma once

#include "script/script.h"

namespace entity
{
	class Component;
	struct ComponentDesc;
	class EntityManager;

	class Entity : public script::ScriptedObject
	{
	protected:
		// typedefs
		typedef map<string, shared_ptr<Component>> component_map;

	public:
		// typedefs
		typedef component_map::const_iterator component_iterator;

		// constructor/destructor
		Entity(EntityManager* manager, const string& name);
		virtual ~Entity();

		// common entity functions
		virtual const string& getName() { return m_name; }
		virtual EntityManager* getManager() { return m_manager; }

		// component functions
		template<typename T>
		T* createComponent(const string& name, const typename T::desc_type& desc, T** component)
		{
			if(getComponent(name) != NULL)
			{
				INFO("WARNING: tried to create duplicate component \"%s\" on entity \"%s\"", name.c_str(), this->m_name.c_str());
				return NULL;
			}

			shared_ptr<T> ptr(new T(this, name, desc));
			addComponent(name, ptr);
			*component = ptr.get();
			return ptr.get();
		}

		virtual void addComponent(const string& name, shared_ptr<Component> component);
		virtual Component* getComponent(const string& name);
		template<typename T>
		T* getComponent(const string& name)
		{
			return dynamic_cast<T*>(getComponent(name));
		}
		virtual void removeComponent(const string& name);
		virtual int getComponentCount() { return m_components.size(); }

		// component shortcuts
		virtual Component* getPositionComponent() { return m_position; } // shortcut for getComponent("pos")

		// acquire/release methods
		virtual void acquire();
		virtual void release();

		// component iterators
		virtual component_iterator begin() { return m_components.begin(); }
		virtual component_iterator end() { return m_components.end(); }

	protected:
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