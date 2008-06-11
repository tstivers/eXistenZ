#pragma once

namespace q3bsp
{
	class BSP;
	extern BSP* bsp;
	extern int debug;

	bool loadBSP(char* filename);
	void init(void);
	void render(void);
	void release(void);
	void con_list_maps(int argc, char* argv, void* user);
};

