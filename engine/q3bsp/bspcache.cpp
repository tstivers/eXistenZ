/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: bspcache.cpp,v 1.3 2003/11/20 03:08:39 tstivers Exp $
//

#include "precompiled.h"
#include "q3bsp/bspcache.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bspconvert.h"
#include "render/dx.h"
#include "render/render.h"
#include "settings/settings.h"
#include "console/console.h"
#include "vfs/vfs.h"

namespace q3bsp {
	BSP* bsp;
	int debug;
	int draw;
	int convert;
	void con_list_maps(int argc, char* argv[], void* user);
};

void q3bsp::init()
{
	bsp = NULL;
	settings::addsetting("system.render.bsp.debug", settings::TYPE_INT, 0, NULL, NULL, &debug);
	settings::addsetting("system.render.bsp.bsp_path", settings::TYPE_STRING, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.bsp.draw", settings::TYPE_INT, 0, NULL, NULL, &draw);
	settings::addsetting("system.render.bsp.convert", settings::TYPE_INT, 0, NULL, NULL, &convert);
	settings::setstring("system.render.bsp.bsp_path", "/maps");
	con::addCommand("list_maps", con_list_maps);
	con::addCommand("toggle_bsp", con::toggle_int, &draw);
	debug = 0;
	draw = 1;
	convert = 0;
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

	if(convert)
		convertBSP(*bsp);

	return true;
}

void q3bsp::render()
{
	if(bsp && draw)
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