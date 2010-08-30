#include "precompiled.h"
#include "sound/sound.h"
#include "sound/jssoundmanager.h"
#include "vfs/vfs.h"
#include "game/game.h"
#include "game/player.h"

using namespace sound;

namespace sound
{
	static void init();
	static FMOD_RESULT __stdcall OpenCallback(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata);
	static FMOD_RESULT __stdcall CloseCallback(void *handle, void *userdata);
	static FMOD_RESULT __stdcall ReadCallback(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata);
	static FMOD_RESULT __stdcall SeekCallback(void *handle, unsigned int pos, void *userdata);
};

static FMOD_RESULT __stdcall sound::OpenCallback(const char *name, int unicode, unsigned int *filesize, void **handle, void **userdata)
{
	vfs::File f = vfs::getFile(name);
	if(!f)
		return FMOD_ERR_FILE_NOTFOUND;

	*filesize = f->getSize();

	*handle = new vfs::File(f);
	return FMOD_OK;
}

static FMOD_RESULT __stdcall sound::CloseCallback(void *handle, void *userdata)
{
	vfs::File* f = (vfs::File*)handle;
	delete f;
	return FMOD_OK;
}

static FMOD_RESULT __stdcall sound::ReadCallback(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
{
	vfs::File* f = (vfs::File*)handle;
	*bytesread  = (*f)->read(buffer, sizebytes);
	if(*bytesread < sizebytes)
		return FMOD_ERR_FILE_EOF;
	else
		return FMOD_OK;
}

static FMOD_RESULT __stdcall sound::SeekCallback(void *handle, unsigned int pos, void *userdata)
{
	vfs::File* f = (vfs::File*)handle;
	(*f)->seek(pos, SEEK_SET);
	return FMOD_OK;
}


REGISTER_STARTUP_FUNCTION(sound, sound::init, 20);

void sound::init()
{
	//settings.addsetting()
}

SoundManager::SoundManager(scene::Scene* scene)
: m_scene(scene)
{
	FMOD_RESULT result;

	result = FMOD::System_Create(&m_system);
	if(result != FMOD_OK)
	{
		INFO("ERROR: failed to create sound system (%d)", result);
		return;
	}
	
	result = m_system->init(100, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
	{
		INFO("ERROR: failed to initialize sound system (%d)", result);
		return;
	}

	result = m_system->setFileSystem(
		OpenCallback,
		CloseCallback,
		ReadCallback,
		SeekCallback,
		NULL,
		NULL,
		-1);

	if (result != FMOD_OK)
	{
		INFO("ERROR: failed to set filesystem callbacks on sound system (%d)", result);
		return;
	}

	initScriptObject();
}

SoundManager::~SoundManager()
{
	m_sounds.clear();

	m_system->release();

	if(m_scriptObject)
		destroyScriptObject();
}

void SoundManager::doTick()
{
	D3DXMATRIX m;
	D3DXVECTOR3 rot = game::player->getRot();
	D3DXVECTOR3 pos = game::player->getPos();
	D3DXVECTOR3 up(0, 1, 0);
	D3DXMatrixRotationYawPitchRoll(&m, D3DXToRadian(rot.y), D3DXToRadian(rot.x), D3DXToRadian(rot.z));
	rot = D3DXVECTOR3(0, 0, 1);
	D3DXVec3TransformCoord(&rot, &rot, &m);
	m_system->set3DListenerAttributes(0, (FMOD_VECTOR*)&pos, NULL, (FMOD_VECTOR*)&rot, (FMOD_VECTOR*)&up);
	if(m_system)
		m_system->update();
}

Sound* SoundManager::createSound(const string& name)
{
	if(m_sounds.find(name) != m_sounds.end())
	{
		INFO("WARNING: tried to create duplicate sound \"%s\"", name.c_str());
		return m_sounds.find(name)->second.get();
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
	else // try to create the sound
		return createSound(name);
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

FMOD::Channel* SoundManager::playSound(const string& name, float volume)
{
	Sound* sound = getSound(name);
	if(!sound)
	{
		INFO("WARNING: tried to play nonexistant sound \"%s\"", name.c_str());
		return NULL;
	}

	FMOD::Channel* channel;
	FMOD_RESULT result = m_system->playSound(FMOD_CHANNEL_FREE, sound->getSound(), true, &channel);
	if(result != FMOD_OK)
	{
		INFO("WARNING: failed to play sound \"%s\"", name.c_str());
		return NULL;
	}

	channel->setMode(FMOD_3D_HEADRELATIVE);
	D3DXVECTOR3 pos(0, 0, 0);
	channel->set3DAttributes((FMOD_VECTOR*)&pos, NULL);
	channel->setVolume(volume);
	channel->setPaused(false);

	return channel;
}

FMOD::Channel* SoundManager::playSound3d(const string& name, const D3DXVECTOR3& pos, float volume)
{
	Sound* sound = getSound(name);
	if(!sound)
	{
		INFO("WARNING: tried to play nonexistant sound \"%s\"", name.c_str());
		return NULL;
	}

	FMOD::Channel* channel;
	FMOD_RESULT result = m_system->playSound(FMOD_CHANNEL_FREE, sound->getSound(), true, &channel);
	if(result != FMOD_OK)
	{
		INFO("WARNING: failed to play sound \"%s\"", name.c_str());
		return NULL;
	}

	channel->set3DMinMaxDistance(5.0f, 1000.0f);
	channel->setVolume(volume);
	channel->set3DAttributes((FMOD_VECTOR*)&pos, NULL);
	channel->setPaused(false);

	return channel;
}


JSObject* SoundManager::createScriptObject()
{
	return CreateSoundManagerObject(this);
}

void SoundManager::destroyScriptObject()
{
	DestroySoundManagerObject(this);
	m_scriptObject = NULL;
}

void SoundManager::initScriptObject()
{
	m_scriptObject = createScriptObject();
}