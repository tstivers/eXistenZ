#pragma once

#include "script/script.h"

namespace component
{
	class Component;
}

namespace entity
{
	class EntityManager;

	enum ENTITY_FLAGS
	{
		E_STATIC		= (1<<0),
		E_DYNAMIC		= (1<<1),
		E_PROJECTILE	= (1<<2),
		E_MESH			= (1<<3)
	};

	class Entity : public script::ScriptedObject
	{
	protected:
		// typedefs
		typedef map<string, shared_ptr<component::Component>> component_map;

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

		virtual void addComponent(const string& name, shared_ptr<component::Component> component);
		virtual component::Component* getComponent(const string& name);
		template<typename T>
		T* getComponent(const string& name)
		{
			return dynamic_cast<T*>(getComponent(name));
		}
		virtual void removeComponent(const string& name);
		virtual int getComponentCount() { return m_components.size(); }

		// component shortcuts
		virtual component::Component* getPositionComponent() { return m_position; } // shortcut for getComponent("pos")

		// entity flags
		virtual void setFlags(int flags) { m_flags = flags; }
		virtual int getFlags() { return m_flags; }

		// acquire/release methods
		virtual void acquire();
		virtual void release();

		// component iterators
		virtual component_iterator begin() { return m_components.begin(); }
		virtual component_iterator end() { return m_components.end(); }

		// script class
		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		// script functions
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members
		string m_name;
		EntityManager* m_manager;
		component_map m_components;
		component::Component* m_position;
		bool m_acquired;
		int m_flags;
	};
}