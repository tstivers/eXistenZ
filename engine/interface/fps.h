/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

class CD3DFont;

namespace ui {
	class FPS {
	public:
		FPS();
		~FPS();
		void render();
		void reset(); // hack
	
	public:
		int xpos;
		int ypos;
		int draw;

	private:
		CD3DFont* d3dfont;
		unsigned int frames;
		unsigned int last_ms;
		float fps;
	};
};
