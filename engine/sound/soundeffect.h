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

	protected:
		// script functions
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members
		string m_name;
		SoundManager* m_manager;
		bool m_acquired;
	};
}