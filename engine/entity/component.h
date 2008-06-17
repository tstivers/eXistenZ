#pragma once

#include "script/script.h"

namespace entity
{
	class Component : public script::ScriptedObject
	{
	public:
		Component(Entity* entity, const string& name)
			: m_name(name), m_entity(entity), m_acquired(false) {}
		virtual ~Component() {}

		virtual const string& getName() { return m_name; }
		virtual const string& getTypeName() { static string typeName = "Component"; return typeName; }

		virtual Entity* getEntity() { return m_entity; }

		virtual void acquire() { m_acquired = true; }
		virtual void release() { m_acquired = false; }

	protected:
		string m_name;
		Entity* m_entity;
		bool m_acquired;
	};
}