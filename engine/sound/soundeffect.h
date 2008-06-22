#pragma once

#include "sound/sound.h"
#include "script/script.h"


namespace sound
{
	class Sound : public script::ScriptedObject
	{
	public:
		Sound(SoundManager* manager, const string& name);
		virtual ~Sound();

		const string& getName() { return m_name; }
		SoundManager* getManager() { return m_manager; }
		FMOD::Sound* getSound() { return m_sound; }

		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		// script functions
		JSObject* createScriptObject() { return NULL; }
		void destroyScriptObject() {}

		// members
		string m_name;
		JSObject* m_scriptObject;
		SoundManager* m_manager;
		FMOD::Sound* m_sound;
	};
}