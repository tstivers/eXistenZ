#include "precompiled.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"

namespace scene {
};

using namespace scene;

SceneBSP::SceneBSP() 
{
	bsp = NULL;
};

SceneBSP::~SceneBSP() 
{
	// release/delete everything
};

void SceneBSP::init()
{
	// return if no bsp has been loaded
	// convert the stuff we need from this->bsp->* to this->*
	// load entities from the bsp.js here i guess (but don't acquire yet) (move to game::?)
}

void SceneBSP::acquire()
{
	if(acquired)
		return;

	// loop through clusters and create rendergroups for faces
	//		get vbuffers, ibuffers, textures
	// loop through entities and acquire everything
	acquired = true;
}

void SceneBSP::release()
{
	if(!acquired)
		return;

	// loop through clusters and delete rendergroups
	// loop through entities and release
	acquired = false;
}

void SceneBSP::reload(unsigned int flags)
{
	// release/flush/reload textures
	// release/flush/reload models for entities?
	// call ~SceneBSP() and then reload everything probably for now
}

void SceneBSP::render()
{
	// find cluster for camera
	// loop through clusters
	// if (cluster is not visible) continue;
	// loop through cluster rendergroups and render::render(rendergroup[i]);
	// loop through entities
	// if(entity->flags || FLAG_RENDER) entity->render();
}