#pragma once

#include "sound/sound.h"

namespace sound
{
	JSObject* CreateSoundManagerObject(SoundManager* manager);
	void DestroySoundManagerObject(SoundManager* manager);
}