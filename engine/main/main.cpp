#include "precompiled.h"
#include "main/main.h"

int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinst_prev, LPSTR cmdline, int cmdshow)
{
	eXInitializeEngine();
	eXFileSystem* filesystem = eXCreateFileSystem("${ModulePath}");
	filesystem->setWritePath("${ModulePath}");
	
	eXSettings* settings = eXCreateSettings();
	settings->load(filesystem.getFile("settings.ini"));
	
	eXSettingsDialog settings_dialog;
	settings_dialog.show(settings);
	
	eXWindow window;
	window.create(settings);
	window.show();
	
	eXEngine engine;
	engine.create(settings, filesystem, window);
	engine.executeScript(filesystem.getFile("startup.js"));
	while(engine.isRunning())
		engine.run();
	
	engine->release();
	window->release();
	settings->release();
	filesystem->release();
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
			physics::getResults();
			timer::fireTimers();
			game::doTick();
			physics::startSimulation();
			render::render();
			JS_MaybeGC(gScriptEngine->GetContext());
		}
	}
}