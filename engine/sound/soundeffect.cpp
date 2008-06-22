#include "precompiled.h"
#include "sound/sound.h"

using namespace sound;

Sound::Sound(SoundManager* manager, const string& name)
: m_manager(manager), m_name(name), m_scriptObject(NULL), m_sound(NULL)
{
	FMOD_RESULT result = m_manager->m_system->createSound(m_name.c_str(), FMOD_3D, NULL, &m_sound);
	if(result != FMOD_OK)
	{
		INFO("WARNING: failed to create sound \"%s\" (%d)", m_name.c_str(), result);
		return;
	}
}

Sound::~Sound()
{
	if(m_sound)
		m_sound->release();
}

