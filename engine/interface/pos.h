/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: pos.h,v 1.2 2003/12/23 04:51:58 tstivers Exp $
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