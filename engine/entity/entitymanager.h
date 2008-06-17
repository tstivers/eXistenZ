#pragma once

#include "script/script.h"
#include "entity/entity.h"

namespace entity
{
	class EntityManager : public script::ScriptedObject
	{
	public:
		// typedefs

		// constructor/destructor
		EntityManager();
		virtual ~EntityManager();

		// entity functions
		Entity* createEntity(const string& name);
		virtual Entity* getEntity(const string& name);
		virtual void removeEntity(const string& name);

	protected:
		// typedefs
		typedef map<string, shared_ptr<Entity>> entity_map;

		// entity functions
		virtual void addEntity(shared_ptr<Entity> entity);

		// script functions
		JSObject* createScriptObject();
		void destroyScriptObject();
		void initScriptObject();

		// members
		entity_map m_entities;
	};
}