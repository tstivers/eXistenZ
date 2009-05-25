#pragma once

#include "entity/interfaces.h"

namespace entity
{
	class EntityManager;
	class Entity;
};

namespace sound
{
	class SoundManager;
}

namespace physics
{
	class PhysicsManager;
}

namespace q3shader
{
	class Q3ShaderCache;
}

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
		typedef set<IRenderable*> renderables_list;
		renderables_list m_renderables;

		// cameras
		typedef set<ICamera*> camera_list;
		camera_list m_cameras;
		ICamera* m_current_camera;
	
		virtual void addCamera(ICamera* camera)
		{
			ASSERT(m_cameras.find(camera) == m_cameras.end());
			m_cameras.insert(camera);
		}

		virtual void removeCamera(ICamera* camera)
		{
			ASSERT(m_cameras.find(camera) != m_cameras.end());
			if(m_current_camera == camera)
				m_current_camera = NULL;
			m_cameras.erase(camera);
		}

		virtual void setCurrentCamera(ICamera* camera)
		{
			ASSERT(m_cameras.find(camera) != m_cameras.end());
			m_current_camera = camera;
		}

		virtual ICamera* getCurrentCamera()
		{
			return m_current_camera;
		}

		// construction
		Scene();
		virtual ~Scene();

		// loading/init
		static shared_ptr<Scene> load(const string& name, SCENE_TYPE type = ST_AUTO);
		virtual void init() = 0;
		virtual void acquire() = 0;
		virtual void release() = 0;
		virtual void reload(unsigned int flags = 0) = 0;

		virtual void doTick();

		// render crap
		//TODO: virtual render(render::Camera& cam, render::Frustrum& frust);
		virtual void render() = 0;

		// entity crap
		//TODO: virtual addEntity(string name, int type,
		virtual void addEntity(entity::Entity* entity) = 0;
		// virtual void updateEntity(const Entity* entity);
		virtual void removeEntity(entity::Entity* entity) = 0;
		virtual void addRenderable(IRenderable* renderable)
		{
			m_renderables.insert(renderable);
		}

		virtual void removeRenderable(IRenderable* renderable)
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

		physics::PhysicsManager* getPhysicsManager() { return m_physicsManager.get(); }

		shared_ptr<entity::EntityManager> m_entityManager;
		shared_ptr<sound::SoundManager> m_soundManager;
		shared_ptr<physics::PhysicsManager> m_physicsManager;
	//	shared_ptr<q3shader::Q3ShaderCache> m_q3shaderCache;
	};

	extern shared_ptr<Scene> g_scene;
}