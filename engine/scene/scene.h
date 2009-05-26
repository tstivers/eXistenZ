#pragma once

#include "entity/interfaces.h"

namespace scene
{
	enum SCENE_TYPE
	{
		ST_BSP,
		ST_TREE,
		ST_PORTAL
	};

	class Scene : public script::ScriptedObject
	{
	public:
		// construction
		Scene(const string& name);
		virtual ~Scene();

		// TODO: replace with RenderableManager?
		virtual void addRenderable(IRenderable* renderable)
		{
			m_renderables.insert(renderable);
		}

		virtual void removeRenderable(IRenderable* renderable)
		{
			m_renderables.erase(renderable);
		}

		// TODO: replace with CameraManager?
		virtual void addCamera(ICamera* camera)
		{
			ASSERT(m_cameras.find(camera) == m_cameras.end());
			m_cameras.insert(camera);
		}

		virtual void removeCamera(ICamera* camera)
		{
			ASSERT(m_cameras.find(camera) != m_cameras.end());
			if(m_currentCamera == camera)
				m_currentCamera = NULL;
			m_cameras.erase(camera);
		}

		// properties
		physics::PhysicsManager* getPhysicsManager() { return m_physicsManager.get(); }
		entity::EntityManager* getEntitymanager() { return m_entityManager.get(); }
		sound::SoundManager* getSoundManager() { return m_soundManager.get(); }
		const string& getName() { return m_name; }
		virtual void setCurrentCamera(ICamera* camera)
		{
			ASSERT(m_cameras.find(camera) != m_cameras.end());
			m_currentCamera = camera;
		}

		virtual ICamera* getCurrentCamera()
		{
			return m_currentCamera;
		}

		// methods
		static shared_ptr<Scene> load(const string& filename);
		virtual void acquire() = 0;
		virtual void release() = 0;
		virtual void doTick();		
		virtual void render() = 0;

		// for js interface
		static ScriptClass m_scriptClass;

	protected:
		// ScriptedObject overrides
		JSObject* createScriptObject() = 0;
		void destroyScriptObject() = 0;		
		
		typedef set<IRenderable*> RenderableList;
		RenderableList m_renderables;

		typedef set<ICamera*> CameraList;
		CameraList m_cameras;
		ICamera* m_currentCamera;

		string m_name;
		int m_type;
		bool m_acquired;

		shared_ptr<entity::EntityManager> m_entityManager;
		shared_ptr<sound::SoundManager> m_soundManager;
		shared_ptr<physics::PhysicsManager> m_physicsManager;
	};

	extern shared_ptr<Scene> g_scene;
}