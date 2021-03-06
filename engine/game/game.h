#pragma once

namespace game
{
	class Game : public script::ScriptedObject
	{
	public:
		Game();
		~Game();

	protected:
		unique_ptr<scene::Scene> m_scene; // TODO: SceneManager
	};

	void init(void);
	void doTick(void);
	void release(void);

	bool startMap(char* name);

	enum
	{
		STATE_LOADLEVEL,
		STATE_RUN,
		STATE_EXIT
	};

	extern int game_state;
	extern float mouse_sens_x;
	extern float mouse_sens_y;
	extern int noclip;

	// console commands
	void con_map(char* cmd, char* cmdline, void* user);
	class Player;
	extern Player* player;
};