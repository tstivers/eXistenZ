#pragma once

#include "entity/interfaces.h"

namespace entity
{
	class EntityManager;
	class Entity;
};

namespace scene
{
	enum SCENE_TYPE
	{
		ST_BSP,
		ST_TREE,
		ST_PORTAL,
		ST_AUTO
	};

	class Scene
	{
	public:
		// TODO: replace with renderablemanager
		typedef set<entity::Renderable*> renderables_list;
		renderables_list m_renderables;

		// construction
		Scene();
		virtual ~Scene();

		// loading/init
		static Scene* load(const string& name, SCENE_TYPE type = ST_AUTO);
		virtual void init() = 0;
		virtual void acquire() = 0;
		virtual void release() = 0;
		virtual void reload(unsigned int flags = 0) = 0;

		// render crap
		//TODO: virtual render(render::Camera& cam, render::Frustrum& frust);
		virtual void render() = 0;

		// entity crap
		//TODO: virtual addEntity(string name, int type,
		virtual void addEntity(entity::Entity* entity) = 0;
		// virtual void updateEntity(const Entity* entity);
		virtual void removeEntity(entity::Entity* entity) = 0;
		virtual void addRenderable(entity::Renderable* renderable)
		{
			m_renderables.insert(renderable);
		}

		virtual void removeRenderable(entity::Renderable* renderable)
		{
			m_renderables.erase(renderable);
		}

		// movement/collision crap (or let game:: handle movement and collide:: handle collision?)
		// maybe collision:: looks like addCollider(Collider& col) and there are ColliderMesh, ColliderBox(trigger), ColliderBSP, etc.

		// lots of other stuff i'm forgetting: sky

		string name;
		int type;
		bool acquired;
		bool initialized;

		shared_ptr<entity::EntityManager> m_entityManager;
	};

	void init();

	extern int optimize_bsp;
};