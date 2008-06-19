#pragma once

#include "script/script.h"
#include "entity/componenttypes.h"

namespace entity
{
	class Entity;

	struct ComponentDesc
	{
	};

	class Component : public script::ScriptedObject
	{
	public:
		typedef ComponentDesc desc_type;

		Component(Entity* entity, const string& name, const desc_type& desc)
			: m_name(name), m_entity(entity), m_acquired(false) {}
		virtual ~Component() {}

		virtual const string& getName() { return m_name; }
		virtual int getType() { return 0; }
		virtual const string& getTypeName() { return getComponentTypeName(getType()); }

		virtual Entity* getEntity() { return m_entity; }

		virtual void acquire() { m_acquired = true; }
		virtual void release() { m_acquired = false; }

	protected:
		string m_name;
		Entity* m_entity;
		bool m_acquired;
	};
}