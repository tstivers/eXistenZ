#include "precompiled.h"
#include "game/game.h"
#include "script/script.h"
#include "settings/settings.h" 
#include "console/console.h" // goes away
#include "scene/scene.h"
#include "input/inputmanager.h"
#include "window/appwindow.h"
#include "game/player.h" // goes away

using namespace eXistenZ;

Game::Game(const string& commandLine)
: m_gameState(GS_INITIALIZING)
{
	INFO("game object created, command line was \"%s\"", commandLine.c_str());

	// create the settings manager
	m_settingsManager = unique_ptr<SettingsManager>(new SettingsManager());

	// load settings
	m_settingsManager->load("settings.ini");

	// create the script engine
	m_scriptEngine = unique_ptr<script::ScriptEngine>(new script::ScriptEngine());

	// create game script object
	this->createScriptObject();

	// create settings script object
	m_settingsManager->getScriptObject();

	// create other top-level managers
	m_inputManager = unique_ptr<InputManager>(new InputManager());
	m_appWindow = unique_ptr<AppWindow>(new AppWindow());

	// wire up inputmanager

	// process command line

	// execute autoexec.js
	m_scriptEngine->executeFile("autoexec.js");
	getService<script::ScriptEngine>();
}

Game::~Game()
{
	INFO("game object destroyed");
}

void Game::run()
{
}

