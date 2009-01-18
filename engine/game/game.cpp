#include "precompiled.h"
#include "game/game.h"
#include "game/player.h"
#include "console/console.h"
#include "input/input.h"
#include "input/bind.h"
#include "interface/interface.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bspcache.h"
#include "render/render.h"
#include "settings/settings.h"
#include "timer/timer.h"
#include "client/appwindow.h"
#include "script/script.h"
#include "vfs/vfs.h"
#include "vfs/file.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "physics/physics.h"
#include "jsplayer.h"


namespace game
{
	int game_state;

	void processInput();
	float mouse_sens_x;
	float mouse_sens_y;

	int noclip;

	void move_up();
	void move_down();
	void move_left();
	void move_right();
	void move_forward();
	void move_back();
	void move_jump();
	void con_quit();
	void con_break();
	void toggle_movemode();

	char init_command[MAX_PATH];

	Player* player;
};

REGISTER_STARTUP_FUNCTION(game, game::init, 10);

void game::init()
{
	player = createPlayer(D3DXVECTOR3(0.5, 1 , 0.5));
	jsplayer::createPlayerObject(script::gScriptEngine->GetContext(), script::gScriptEngine->GetObject("game"), "player", player);
	game_state = STATE_RUN;
	settings::addsetting("game.mouse.sensitivity.x", settings::TYPE_FLOAT, 0, NULL, NULL, &mouse_sens_x);
	settings::addsetting("game.mouse.sensitivity.y", settings::TYPE_FLOAT, 0, NULL, NULL, &mouse_sens_y);
	settings::addsetting("game.noclip", settings::TYPE_INT, 0, NULL, NULL, &noclip);

	settings::addsetting("game.init_command", settings::TYPE_STRING, 0, NULL, NULL, init_command);

	settings::setfloat("game.mouse.sensitivity.x", 0.50f);
	settings::setfloat("game.mouse.sensitivity.y", 1.0f);
	init_command[0] = 0;

	noclip = 1;

	console::addCommand("map", con_map);
	console::addCommand("toggle_clipping", console::toggle_int, &noclip);
	console::addCommand("toggle_movemode", toggle_movemode);
	console::addCommand("move_up", move_up);
	console::addCommand("move_down", move_down);
	console::addCommand("move_left", move_left);
	console::addCommand("move_right", move_right);
	console::addCommand("move_forward", move_forward);
	console::addCommand("move_back", move_back);
	console::addCommand("move_jump", move_jump);
	console::addCommand("quit", con_quit);
	console::addCommand("dbg_break", con_break);
}

void game::release()
{
}

void game::toggle_movemode()
{
	if (player->getMoveMode() == MM_FLY)
		player->setMoveMode(MM_WALK);
	else
		player->setMoveMode(MM_FLY);
}

void game::doTick()
{
	switch (game_state)
	{
	case STATE_RUN:
		processInput();
		if(render::scene)
			render::scene->doTick();
		break;
	default:
		break;
	}
}

void game::processInput()
{
	//console::log_frame = false;

	if (init_command[0])
	{
		console::processCmd(init_command);
		init_command[0] = 0;
	}

	if (!input::has_focus)
		return;

	// default key mappings cannot be changed
	if (KEYPRESSED(DIK_ESCAPE))
	{
		PostMessage(appwindow::getHwnd(), WM_QUIT, 0, 0);
	}

	if (KEYDOWN(DIK_GRAVE))
	{
		input::unacquire();
		settings::setint("system.ui.console.draw", 1);
	}

	if ((KEYDOWN(DIK_LALT) || KEYDOWN(DIK_RALT)) && KEYDOWN(DIK_RETURN))
	{
		appwindow::toggleFullScreen();
		input::kbstate[DIK_RETURN] = 0; // eat the enter
	}

	// handle mouse crap here for now
	player->doRotation(D3DXVECTOR3((float)input::mousestate.lY * mouse_sens_x,
								   (float)input::mousestate.lX * mouse_sens_y,
								   0.0f));

	// process key mappings
	input::processBinds();

	// update player position
	player->updatePos();

	// attach camera to player
	render::cam_pos = player->getPos();
	render::cam_pos += render::cam_offset;
	render::cam_rot = player->getRot();

	return;
}

void game::move_up()
{
	player->doMove(MOVE_UP);
}

void game::move_down()
{
	player->doMove(MOVE_DOWN);
}

void game::move_left()
{
	player->doMove(MOVE_LEFT);
}

void game::move_right()
{
	player->doMove(MOVE_RIGHT);
}

void game::move_forward()
{
	player->doMove(MOVE_FORWARD);
}

void game::move_back()
{
	player->doMove(MOVE_BACK);
}

void game::move_jump()
{
	player->doMove(MOVE_JUMP);
}

void game::con_map(char* cmd, char* cmdline, void* user)
{
	if (!cmdline)
	{
		LOG("usage: /map <mapname>");
		return;
	}

	startMap(cmdline);
	input::acquire();
}

void game::con_quit()
{
	PostMessage(appwindow::getHwnd(), WM_QUIT, 0, 0);
}

bool game::startMap(char* name)
{
	char bspname[MAX_PATH];

	sprintf(bspname, "maps/%s.bsp", name);

	// load the bsp
	render::scene = scene::Scene::load(bspname);
	if (!render::scene)
		return false;

	render::scene->name = name;

	render::scene->init();

	physics::acquire();
	player->acquire();
	render::scene->acquire();
	//physics::addStaticBSP(name, (scene::SceneBSP*)render::scene);

	// load the script
	sprintf(bspname, "scripts/%s.js", name);
	vfs::File file = vfs::getFile(bspname);
	if (file)
	{
		script::gScriptEngine->RunScript(file);
	}

	return true;
}

void game::con_break()
{
	DebugBreak();
}