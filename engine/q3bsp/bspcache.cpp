/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: bspcache.cpp,v 1.2 2003/11/18 18:39:42 tstivers Exp $
//

#include "precompiled.h"
#include "q3bsp/bspcache.h"
#include "q3bsp/bleh.h"
#include "render/dx.h"
#include "render/render.h"
#include "settings/settings.h"
#include "console/console.h"
#include "vfs/vfs.h"

namespace q3bsp {
	BSP* bsp;
	int debug;
	void con_list_maps(int argc, char* argv[], void* user);
};

void q3bsp::init()
{
	bsp = NULL;
	settings::addsetting("system.render.bsp.debug", settings::TYPE_INT, 0, NULL, NULL, &debug);
	settings::addsetting("system.render.bsp.bsp_path", settings::TYPE_STRING, 0, NULL, NULL, NULL);
	settings::setstring("system.render.bsp.bsp_path", "/maps");
	con::addCommand("list_maps", con_list_maps);
	debug = 0;
}

void q3bsp::release()
{
	delete bsp;
}

bool q3bsp::loadBSP(char* filename)
{	
	delete bsp;
	bsp = new BSP();

	if(!bsp->load(filename))
	{
		LOG2("[q3bsp::loadBSP] unable to load \"%s\"", filename);
		delete bsp;
		bsp = NULL;
		return false;
	}

	return true;
}

void q3bsp::render()
{
	if(bsp)
		bsp->render();
}

void q3bsp::con_list_maps(int argc, char* argv[], void* user)
{
	vfs::file_list_t map_list;
	vfs::getFileList(map_list, settings::getstring("system.render.bsp.bsp_path"), "*.bsp");
	map_list.sort();
	LOG("Map List:");
	for(vfs::file_list_t::iterator it = map_list.begin(); it != map_list.end(); ++it)
		LOG2("  %s", (*it).c_str());
}