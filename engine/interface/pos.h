/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

class CD3DFont;

namespace ui {
	class Pos {
	public:
		Pos();
		~Pos();
		void render();
		void reset();

	public:
		int xpos;
		int ypos;
		int draw;

	private:
		CD3DFont* d3dfont;
	};
};