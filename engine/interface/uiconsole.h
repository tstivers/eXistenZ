/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: uiconsole.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

class CD3DFont;

namespace ui {

	class Console {
	public:
		Console();
		~Console();
		void render();
		void chooseFont();
		void addMessage(const char* message);
		void keypressed(char key, bool extended = false);
		void clear();

	public:
		int xpos;
		int ypos;
		int width;
		int height;
		int cmdecho;
		U32 filter;		
		CD3DFont* d3dfont;

		std::deque<char*> scrollback;
		std::deque<std::string> history;
		std::string cmd;
		int cursorpos;
		int historypos;
		int wireframe;
		int draw;

		typedef std::deque<char*>::iterator scrollback_iter;
	};
};	