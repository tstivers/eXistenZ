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
		void addMessage(const string& message);
		void charpressed(const eXistenZ::KeyEventArgs& args);
		void keypressed(const eXistenZ::KeyEventArgs& args);
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

		deque<string> scrollback;
		deque<string> history;
		string cmd;
		unsigned int cursorpos;
		unsigned int historypos;
		int wireframe;
		int draw;
		mutex sbmutex;

		typedef deque<char*>::iterator scrollback_iter;
	};
};