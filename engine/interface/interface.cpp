#include "precompiled.h"
#include "interface/interface.h"
#include "render/render.h"
#include "settings/settings.h"
#include "console/console.h"
#include "render/dx.h"
#include "interface/uiconsole.h"
#include "interface/fps.h"
#include "interface/pos.h"
#include "input/input.h"

namespace ui
{
	int draw;
	int printFunctionNames = 0;
	Console console;
	FPS fps;
	Pos pos;
	int has_focus;
	void con_focus_console();
	void con_focus_console_slash();
	void consoleMessageCallback(const char* file, unsigned int line, const char* function, unsigned int flags, const char* message, void* user);
};

REGISTER_STARTUP_FUNCTION(ui, ui::init, 10);

void ui::init()
{
	// TODO: move console crap into interface/console.cpp and interface/jsconsole.cpp
	settings::addsetting("system.ui.draw", settings::TYPE_INT, 0, NULL, NULL, &draw);
	settings::addsetting("system.ui.console.x", settings::TYPE_INT, 0, NULL, NULL, &console.xpos);
	settings::addsetting("system.ui.console.y", settings::TYPE_INT, 0, NULL, NULL, &console.ypos);
	settings::addsetting("system.ui.fps.x", settings::TYPE_INT, 0, NULL, NULL, &fps.xpos);
	settings::addsetting("system.ui.fps.y", settings::TYPE_INT, 0, NULL, NULL, &fps.ypos);
	settings::addsetting("system.ui.fps.draw", settings::TYPE_INT, 0, NULL, NULL, &fps.draw);
	settings::addsetting("system.ui.pos.x", settings::TYPE_INT, 0, NULL, NULL, &pos.xpos);
	settings::addsetting("system.ui.pos.y", settings::TYPE_INT, 0, NULL, NULL, &pos.ypos);
	settings::addsetting("system.ui.pos.draw", settings::TYPE_INT, 0, NULL, NULL, &pos.draw);
	settings::addsetting("system.ui.console.width", settings::TYPE_INT, 0, NULL, NULL, &console.width);
	settings::addsetting("system.ui.console.height", settings::TYPE_INT, 0, NULL, NULL, &console.height);
	settings::addsetting("system.ui.console.filter", settings::TYPE_INT, 0, NULL, NULL, &console.filter);
	settings::addsetting("system.ui.console.cmdecho", settings::TYPE_INT, 0, NULL, NULL, &console.cmdecho);
	settings::addsetting("system.ui.console.wireframe", settings::TYPE_INT, 0, NULL, NULL, &console.wireframe);
	settings::addsetting("system.ui.console.draw", settings::TYPE_INT, 0, NULL, NULL, &console.draw);
	settings::addsetting("system.ui.console.printFunctionNames", settings::TYPE_INT, 0, NULL, NULL, &printFunctionNames);
	settings::setint("system.ui.draw", 1);
	settings::setint("system.ui.console.x", 20);
	settings::setint("system.ui.console.y", 400);
	settings::setint("system.ui.fps.x", 700);
	settings::setint("system.ui.fps.y", 0);
	settings::setint("system.ui.pos.x", 0);
	settings::setint("system.ui.pos.y", 0);
	settings::setint("system.ui.console.width", 760);
	settings::setint("system.ui.console.height", 140);
	settings::setint("system.ui.console.filter", 0xffff);
	settings::setint("system.ui.console.cmdecho", 1);

	console::addCommand("toggle_console", console::toggle_int, &console.draw);
	console::addCommand("focus_console", con_focus_console);
	console::addCommand("focus_console_slash", con_focus_console_slash);
	console::addCommand("toggle_ui", console::toggle_int, &draw);
	console::addCommand("toggle_pos", console::toggle_int, &pos.draw);

	Log::addConsumer("console", LF_ALL, consoleMessageCallback, &console);
}

void ui::release()
{
}

void ui::reset()
{
	console.reset();
	fps.reset();
	pos.reset();
}

void ui::render()
{
	if (!draw)
		return;

	console.render();
	fps.render();
	pos.render();
}

void ui::charpressed(const eXistenZ::KeyEventArgs& args)
{
	if (args.key == '`')
	{
		input::acquire();
		console.draw = 0;
		return;
	}

	if (args.key == 0x09)
	{
		input::acquire();
		return;
	}

	if (args.key == 0x1b)
	{
		console::executeCommand("quit");
	}

	console.charpressed(args);
}

void ui::keypressed(const eXistenZ::KeyEventArgs& args)
{
	console.keypressed(args);
}

void ui::con_focus_console()
{
	input::unacquire();
	console.draw = 1;
}

void ui::con_focus_console_slash()
{
	input::unacquire();
	console.draw = 1;
	console.charpressed(eXistenZ::KeyEventArgs('/', eXistenZ::BS_PRESSED, 0));
}

void ui::consoleMessageCallback(const char* file, unsigned int line, const char* function, unsigned int flags, const char* message, void* user)
{
	if (!(((Console*)user)->filter & flags))
		return;

	char buffer[512];
	if (function && *function & printFunctionNames)
		sprintf(buffer, "[%s] %s", function, message);
	else
		strcpy(buffer, message);

	((Console*)user)->addMessage(buffer);
}