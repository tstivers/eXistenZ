#pragma once

namespace eXistenZ
{
	enum GameState
	{
		GS_RUNNING,
		GS_INITIALIZING,
		GS_PAUSED,
		GS_SHUTDOWN
	};

	class Game : public script::ScriptedObject
	{
	public:
		Game(const string& commandLine);
		~Game();

		// methods
		void createServices();
		void run();
		void destroyServices();

		// static methods
		template<typename T>
		static void registerServiceHandler(function<ServiceBase*(void)> createFunction);
		
		template<typename T>
		ServiceBase* getService() { return m_services[&typeid(T)]; }

		GameState getState() { return m_gameState; }

		scene::Scene* getScene() { return m_scene.get(); }
		script::ScriptEngine* getScriptEngine() { return m_scriptEngine.get(); }

	protected:
		// methods
		void wireAppWindow();

		// fields
		GameState m_gameState;

		unique_ptr<scene::Scene> m_scene; // TODO: SceneManager
		unique_ptr<InputManager> m_inputManager;
		unique_ptr<AppWindow> m_appWindow;
		unique_ptr<game::Player> m_player; // TODO: componentize
		//unique_ptr<VFSManager> m_vfsManager;
		unique_ptr<script::ScriptEngine> m_scriptEngine;
		unique_ptr<SettingsManager> m_settingsManager;
		//unique_ptr<TextureManager> m_textureManager;
		//unique_ptr<SoundManager> m_soundManager;
		//unique_ptr<PhysicsManager> m_physicsManager;
		//unique_ptr<EventManager> m_eventManager;
		//unique_ptr<TimeManager> m_timeManager;
		//unique_ptr<AssetManager> m_assetManager; // templated universal loading/caching/streaming mechanism

		static map<type_info, function<ServiceBase*(void)>> s_serviceHandlers;
		ptr_map<const type_info*, ServiceBase*> m_services;
	};

	extern unique_ptr<Game> g_game;
}