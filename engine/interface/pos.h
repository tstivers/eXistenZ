/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: pos.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

class CD3DFont;

namespace ui {
	class Pos {
	public:
		Pos();
		~Pos();
		void render();

	public:
		int xpos;
		int ypos;
		int draw;

	private:
		CD3DFont* d3dfont;
	};
};