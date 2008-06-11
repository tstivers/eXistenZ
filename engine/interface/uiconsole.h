#pragma once

class CD3DFont;

namespace ui
{

	class Console
	{
	public:
		Console();
		~Console();
		void render();
		void chooseFont();
		void addMessage(const char* message);
		void keypressed(char key, bool extended = false);
		void clear();
		void reset();

	public:
		int xpos;
		int ypos;
		int width;
		int height;
		int cmdecho;
		U32 filter;
		CD3DFont* d3dfont;

		deque<char*> scrollback;
		deque<string> history;
		string cmd;
		unsigned int cursorpos;
		unsigned int historypos;
		int wireframe;
		int draw;

		typedef deque<char*>::iterator scrollback_iter;
	};
};