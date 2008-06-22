#pragma once

#include "script/script.h"
#include "scene/scene.h"

namespace sound
{
	class Sound;

	class SoundManager : public script::ScriptedObject
	{
		friend class Sound;

	protected:
		// typedefs
		typedef map<string, shared_ptr<Sound>> sound_map;

	public:
		// typedefs
		typedef sound_map::const_iterator iterator;

		// constructor/destructor
		SoundManager(scene::Scene* scene);
		virtual ~SoundManager();

		// parent functions
		virtual scene::Scene* getScene() { return m_scene; }

		// frame functions
		virtual void doTick();

		// sound functions
		Sound* createSound(const string& name);
		virtual Sound* getSound(const string& name);
		virtual int getSoundCount();
		virtual void removeSound(const string& name);
		virtual void addSound(shared_ptr<Sound> sound);
		FMOD::Channel* playSound(const string& name, float volume = 1.0f);
		FMOD::Channel* playSound3d(const string& name, const D3DXVECTOR3& pos, float volume = 1.0f);

		// iteration functions
		virtual iterator begin() { return m_sounds.begin(); }
		virtual iterator end() { return m_sounds.end(); }

		// play functions

		static ScriptClass m_scriptClass;
	protected:
		// script functions
		JSObject* createScriptObject();
		void destroyScriptObject();
		void initScriptObject();

		// members
		sound_map m_sounds;
		scene::Scene* m_scene;
		FMOD::System* m_system;
	};
}