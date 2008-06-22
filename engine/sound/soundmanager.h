#pragma once

#include "script/script.h"
#include "scene/scene.h"

namespace sound
{
	class Sound;

	class SoundManager : public script::ScriptedObject
	{
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

		// sound functions
		Sound* createSound(const string& name);
		virtual Sound* getSound(const string& name);
		virtual int getSoundCount();
		virtual void removeSound(const string& name);
		virtual void addSound(shared_ptr<Sound> sound);
		virtual int getSoundList(vector<string>& names);

		// iteration functions
		virtual iterator begin() { return m_sounds.begin(); }
		virtual iterator end() { return m_sounds.end(); }

		// play functions

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