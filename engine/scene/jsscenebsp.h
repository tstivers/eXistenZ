#pragma once

#include "scene/scenebsp.h"

namespace jsscenebsp
{
	JSObject* CreateSceneBSPObject(scene::SceneBSP* scene);
	void DestroySceneBSPObject(scene::SceneBSP* scene);
}