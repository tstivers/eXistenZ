/////////////////////////////////////////////////////////////////////////////
// main.cpp
// contains the program entry point as well as all globals
// $Id: main.cpp,v 1.6 2003/12/23 04:51:58 tstivers Exp $
//
#include "precompiled.h"
#include "client/main.h"
#include "console/console.h"
#include "console/jsconsole.h"
#include "script/script.h"
#include "script/jsscript.h"
#include "settings/settings.h"
#include "settings/jssettings.h"
#include "vfs/vfs.h"
#include "vfs/file.h"
#include "client/appwindow.h"
#include "render/render.h"
#include "interface/interface.h"
#include "timer/timer.h"
#include "q3bsp/bleh.h"
#include "q3shader/q3shadercache.h"
#include "input/input.h"
#include "input/jsinput.h"
#include "game/game.h"
#include "game/jsgame.h"
#include "texture/texturecache.h"
#include "texture/jstexture.h"
#include "shader/shadercache.h"
#include "mesh/meshcache.h"
#include "mesh/meshsystemcache.h"
#include "scene/scene.h"
#include "entity/jsentity.h"

//extern unsigned long  _build_num;

// hack
ScriptEngine gScriptEngine;
HINSTANCE gHInstance;
int gActive = 0;

void addSystemSettings();
int mainloop();

//////////////////////////////////////////////////////////
// Client Entry Point
//
int WINAPI
WinMain(HINSTANCE hinst, HINSTANCE hinst_prev, LPSTR cmdline, int cmdshow)
{
	int exitcode = 0;
	gHInstance = hinst;

	//LOG2("\n\n-------------- eXistenZ client build %i starting --------------\n", _build_num);

	// initialize essention system services
	jsscript::init();
	timer::init();
	con::init();
	jscon::init();
	settings::init();
	jssettings::init();
	vfs::init();
	appwindow::init();
	render::init();
	ui::init();
	game::init();
	jsgame::init();
	texture::init();
	jstexture::init();
	jsinput::init();
	q3bsp::init();
	shader::init();
	scene::init();
	jsentity::init();

	// add some generic system settings
	addSystemSettings();

	// set vfs root so we can load the main config file
	vfs::setRoot(settings::getstring("system.env.exepath"));
	
	// load and execute the config script
	VFile* file = vfs::getFile("config.js");
	if(file){
		gScriptEngine.RunScript(file);
		file->close();
	} else LOG("[eXistenZ] unable to open \"config.js\"");

	// execute command line
	con::processCmd(cmdline);

	// open main window and set up interface	
	appwindow::createWindow(hinst);
	appwindow::showWindow(true);
	
	input::init();
	texture::acquire(); // hack
	input::acquire();
	render::start();
	shader::acquire();	

	// enter main loop and do fun things
	mainloop();

	// shut down the system
	appwindow::showWindow(false);
	appwindow::release();
	//vfs::release();
	//jssettings::release();
	settings::release();
	//jscon::release();
	//con::release();
	
	//LOG2("\n----------- eXistenZ client build %i shutting down -----------", _build_num);
	
	return exitcode;
}

void addSystemSettings()
{
	char buf[512];
	char* bufptr;

	settings::addsetting("system.env.exepath", settings::TYPE_STRING, settings::FLAG_READONLY, NULL, NULL, NULL);
	settings::addsetting("system.env.exefilename", settings::TYPE_STRING, settings::FLAG_READONLY, NULL, NULL, NULL);
	settings::addsetting("system.env.cwd", settings::TYPE_STRING, settings::FLAG_READONLY, NULL, NULL, NULL);
	
	GetModuleFileName(NULL, buf, 512);
	if(bufptr = strrchr(buf, '\\')) {
		*bufptr = 0;
		bufptr++;
	}

	settings::setstring("system.env.exepath", buf);
	settings::setstring("system.env.exefilename", bufptr);
	
	GetCurrentDirectory(512, buf);
	settings::setstring("system.env.cwd", buf);
	settings::addsetting("system.env.build", settings::TYPE_INT, settings::FLAG_READONLY, NULL, NULL, NULL);
	//settings::setint("system.env.build", _build_num);

	// placeholders
}

int mainloop()
{
	MSG msg;

	while(TRUE)
	{
		while(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				return (int)msg.wParam;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		if(!gActive)
			WaitMessage();
		else {
			timer::doTick();
			input::doTick();
			game::doTick();
			render::render();
		}
	}
}