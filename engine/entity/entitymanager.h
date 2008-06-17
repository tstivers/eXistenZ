#pragma once

#include "script/script.h"
#include "entity/entity.h"
#include "scene/scene.h"
#include "entity/jsentitymanager.h"

namespace entity
{
	class EntityManager : public script::ScriptedObject
	{
	protected:
		// typedefs
		typedef map<string, shared_ptr<Entity>> entity_map;

	public:
		// typedefs
		typedef entity_map::const_iterator iterator;

		// constructor/destructor
		EntityManager(scene::Scene* scene);
		virtual ~EntityManager();

		// parent functions
		virtual scene::Scene* getScene() { return m_scene; }

		// entity functions
		Entity* createEntity(const string& name);
		virtual Entity* getEntity(const string& name);
		virtual Entity* getEntity(int index);
		virtual int getEntityCount();
		virtual void removeEntity(const string& name);
		virtual void addEntity(shared_ptr<Entity> entity);

		// iteration functions
		virtual iterator begin() { return m_entities.begin(); }
		virtual iterator end() { return m_entities.end(); }
	protected:
		// script functions
		JSObject* createScriptObject();
		void destroyScriptObject();
		void initScriptObject();

		// members
		entity_map m_entities;
		scene::Scene* m_scene;
	};
}