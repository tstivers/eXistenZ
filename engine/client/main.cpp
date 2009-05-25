#include "precompiled.h"
#include "client/main.h"
#include "vfs/vfs.h"
#include "settings/settings.h"
#include "console/console.h"
#include "script/script.h"
#include "client/appwindow.h"
#include "input/input.h"
#include "texture/texturecache.h"
#include "render/render.h"
#include "shader/shadercache.h"
#include "timer/timer.h"
#include "physics/physics.h"
#include "timer/timers.h"
#include "game/game.h"
#include "script/jsvector.h"
#include "script/jsfunction.h"
#include "scene/scene.h"

script::ScriptEngine* script::gScriptEngine = NULL;
HINSTANCE gHInstance = 0;
int gActive = 0;

void addSystemSettings();
int mainloop();

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinst_prev, LPSTR cmdline, int cmdshow)
{
	int exitcode = 0;
	gHInstance = hinst;

	// execute registered startup functions
	registeredfunctions::fireStartupFunctions();

	// set vfs root so we can load the main config file
	vfs::setRoot(settings::getstring("system.env.exepath"));

	// load and execute the config script
	{
		vfs::File file = vfs::getFile("config.js");
		if (file)
		{
			LOG("reading config from \"%s\"", file->getFilename());
			script::gScriptEngine->RunScript(file);
			file.reset();
		}
		else LOG("unable to open \"config.js\"");
	}
	// execute command line
	console::processCmd(cmdline);

	// TODO: everything from here on down should be driven by a script

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
	//settings::release();
	//jssettings::release();
	//jscon::release();
	//console::release();
	//physics::release();
	//script::release();

	//LOG("\n----------- eXistenZ client build %i shutting down -----------", _build_num);

	return exitcode;
}

REGISTER_STARTUP_FUNCTION(client, addSystemSettings, 10);

void addSystemSettings()
{
	char buf[512];
	char* bufptr;

	settings::addsetting("system.env.exepath", settings::TYPE_STRING, settings::FLAG_READONLY, NULL, NULL, NULL);
	settings::addsetting("system.env.exefilename", settings::TYPE_STRING, settings::FLAG_READONLY, NULL, NULL, NULL);
	settings::addsetting("system.env.cwd", settings::TYPE_STRING, settings::FLAG_READONLY, NULL, NULL, NULL);

	GetModuleFileName(NULL, buf, 512);
	if (bufptr = strrchr(buf, '\\'))
	{
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

	while (TRUE)
	{
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return (int)msg.wParam;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		if (!gActive)
			WaitMessage();
		else
		{
			timer::doTick();
			input::doTick();
			if(scene::g_scene)
				scene::g_scene->getPhysicsManager()->getResults();
			timer::fireTimers();
			game::doTick();
			if(scene::g_scene)
				scene::g_scene->getPhysicsManager()->startSimulation();
			render::render();			
			JS_MaybeGC(script::gScriptEngine->GetContext());
		}
	}
}