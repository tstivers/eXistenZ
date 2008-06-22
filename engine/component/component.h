#pragma once

#include "component/componenttypes.h"

namespace entity
{
	class Entity;
}

namespace component
{
	class Component;

	template<typename T>
	class ComponentLink
	{
	public:
		typedef T link_target_type;

		ComponentLink(Component* parent)
		{
			m_parent = parent;
		}

		inline bool operator=(const string& name)
		{
			m_name = name;
			m_component = NULL;
			return operator->();
		}

		inline bool operator=(T* component)
		{
			if(component)
				m_name = component->getName();
			else
				m_name.clear();

			m_component = component;
			return operator->();
		}

		inline T* operator->() const
		{
			if(!m_component && !m_name.empty())
				m_component = m_parent->getEntity()->getComponent<T>(m_name);
			return m_component;
		}

		inline const string& getName() const
		{
			return m_name;
		}

		inline void release()
		{
			m_component = NULL;
		}

		inline operator T* () const
		{
			return operator->();
		}

		template<typename U>
		inline bool operator==(const ComponentLink<U> &other)
		{
			return this->getName() == other.getName();
		}

	protected:

		string m_name;
		mutable T* m_component;
		Component* m_parent;
	};

	struct ComponentDesc
	{
	};

	class Component : public script::ScriptedObject
	{
	public:
		typedef ComponentDesc desc_type;

		Component(entity::Entity* entity, const string& name, const desc_type& desc)
			: m_name(name), m_entity(entity), m_acquired(false) {}
		virtual ~Component() {}

		const string& getName() { return m_name; }
		virtual int getType() = 0;
		const string& getTypeName() { return getComponentTypeName(getType()); }

		virtual entity::Entity* getEntity() { return m_entity; }

		virtual void acquire() { m_acquired = true; }
		virtual void release() { m_acquired = false; }

		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		string m_name;
		entity::Entity* m_entity;
		bool m_acquired;
	};
}