#include "precompiled.h"
#include "sound/sound.h"
#include "sound/jssoundmanager.h"

namespace sound
{
	static void init();
};

using namespace sound;

REGISTER_STARTUP_FUNCTION(sound, sound::init, 20);

void sound::init()
{
	//settings.addsetting()
}

SoundManager::SoundManager(scene::Scene* scene)
: m_scene(scene)
{
	initScriptObject();
}

SoundManager::~SoundManager()
{

}

Sound* SoundManager::createSound(const string& name)
{
	if(getSound(name))
	{
		INFO("WARNING: tried to create duplicate sound \"%s\"", name.c_str());
		return NULL;
	}

	Sound* sound = new Sound(this, name);

	m_sounds.insert(sound_map::value_type(name, shared_ptr<Sound>(sound)));
	return sound;
}

Sound* SoundManager::getSound(const string& name)
{
	sound_map::iterator it = m_sounds.find(name);
	if(it != m_sounds.end())
		return it->second.get();
	else
		return NULL;
}

int SoundManager::getSoundCount()
{
	return m_sounds.size();
}

void SoundManager::removeSound(const string& name)
{
	m_sounds.erase(name);
}

// used for alternate sound types or sounds owned by other managers
void SoundManager::addSound(shared_ptr<Sound> sound)
{
	ASSERT(getSound(sound->getName()) == NULL);
	m_sounds.insert(sound_map::value_type(sound->getName(), sound));
}

JSObject* SoundManager::createScriptObject()
{
	//return jssound::createSoundManager(this);
	return NULL;
}

void SoundManager::destroyScriptObject()
{
	//jssound::destroySoundManager(this);
}

void SoundManager::initScriptObject()
{
	//m_scriptObject = createScriptObject();
}