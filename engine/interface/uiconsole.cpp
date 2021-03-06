#include "precompiled.h"
#include "interface/interface.h"
#include "render/render.h"
#include "settings/settings.h"
#include "console/console.h"
#include "render/dx.h"
#include "interface/uiconsole.h"
#include "input/input.h"
#include "render/font.h"
#include "timer/timer.h"
#include "script/script.h"

using namespace ui;

void con_clear_console(int argc, char* argv[], void* user);

Console::Console()
{
	while (scrollback.size() < 64)
	{
		scrollback.push_back(string(""));
	}

	cursorpos = 0;
	wireframe = 0;
	draw = 1;
	console::addCommand("clear", con_clear_console, this);
}

Console::~Console()
{
}

void Console::reset()
{
	delete d3dfont;
	d3dfont = NULL;
}

void Console::chooseFont()
{
	CHOOSEFONT cf;
	LOGFONT lf;

	// Initialize members of the CHOOSEFONT structure.

	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = (HWND)NULL;
	cf.hDC = (HDC)NULL;
	cf.lpLogFont = &lf;
	cf.iPointSize = 0;
	cf.Flags = CF_SCREENFONTS;
	cf.rgbColors = RGB(0, 0, 0);
	cf.lCustData = 0L;
	cf.lpfnHook = (LPCFHOOKPROC)NULL;
	cf.lpTemplateName = (LPSTR)NULL;
	cf.hInstance = (HINSTANCE) NULL;
	cf.lpszStyle = (LPSTR)NULL;
	cf.nFontType = SCREEN_FONTTYPE;
	cf.nSizeMin = 0;
	cf.nSizeMax = 0;

	// Display the CHOOSEFONT common-dialog box.

	ChooseFont(&cf);

	// Create a logical font based on the user's
	// selection and return a handle identifying
	// that font.

	//font = CreateFontIndirect(cf.lpLogFont);
}

void Console::render()
{
	if (!draw)
		return;

	D3DCOLOR color;
	if (ui::has_focus)
		color = D3DCOLOR_ARGB(255, 255, 255, 255);
	else
		color = D3DCOLOR_ARGB(128, 255, 255, 255);

	if (d3dfont == NULL)
	{
		d3dfont = new CD3DFont("Terminal", 9);
		d3dfont->InitDeviceObjects(render::device);
		d3dfont->RestoreDeviceObjects();
	}

	float x = (float)xpos;
	float top = (float)ypos;
	float curr_y = (float)(ypos + height - 25);

	sbmutex.lock();
	auto line = scrollback.begin();
	while ((curr_y > top) && (line != scrollback.end()))
	{
		d3dfont->DrawText(x, curr_y, color, line->c_str(), wireframe ? D3DFONT_WIREFRAME : 0);
		line++;
		curr_y -= 12;
	}
	sbmutex.unlock();

	char cursor = 0x01;
	if (ui::has_focus && (((int)timer::time_ms / 300) % 2))
		cmd.insert(cursorpos, 1, cursor);
	cmd.insert(0, "> ");
	d3dfont->DrawText(x, (float)(ypos + height - 10), color, cmd.c_str(), wireframe ? D3DFONT_WIREFRAME : 0);
	cmd.erase(0, 2);
	if (ui::has_focus && (((int)timer::time_ms / 300) % 2))
		cmd.erase(cursorpos, 1);
}

void Console::addMessage(const string& message)
{
	sbmutex.lock();
	scrollback.pop_back();
	scrollback.push_front(message);
	sbmutex.unlock();
}

void Console::addMessage(const char* message)
{
	return addMessage(string(message));
}

void Console::clear()
{
	sbmutex.lock();
	for (auto line = scrollback.begin(); line != scrollback.end(); line++)
		line->clear();
	sbmutex.unlock();
}

void Console::keypressed(const eXistenZ::KeyEventArgs& args)
{
	switch (args.key)
	{
	case VK_LEFT:
		if (cursorpos > 0)
			cursorpos--;
		break;
	case VK_RIGHT:
		if (cursorpos < cmd.size())
			cursorpos++;
		break;
	case VK_UP:
		if ((history.size() > 0) && (historypos < history.size()))
		{
			historypos++;
			cmd = history[historypos - 1];
			cursorpos = (unsigned int)cmd.size();
		}
		break;
	case VK_DOWN:
		if ((history.size() > 0) && (historypos > 0))
		{
			historypos--;
			cmd = history[historypos];
			cursorpos = (unsigned int)cmd.size();
		}
		break;
	case VK_DELETE:
		if (cursorpos < cmd.size())
			cmd.erase(cursorpos, 1);
		break;
	case VK_HOME:
		cursorpos = 0;
		break;
	case VK_END:
		cursorpos = (unsigned int)cmd.size();
		break;
	case VK_TAB:
		auto s = script::gScriptEngine->TabComplete(cmd);
		cmd += s.first;
		cursorpos += s.first.size();
		if(s.second.size() > 1)
		{
			addMessage(string("> ") + cmd);
			for(auto i = s.second.begin(); i != s.second.end(); i++)
				addMessage(*i);
		}
		break;
	}
}


void Console::charpressed(const eXistenZ::KeyEventArgs& args)
{
	//LOG("key pressed '0x%02x'", args.key);

	switch (args.key)
	{
	case 0x08: // backspace
		if (cursorpos > 0)
		{
			cursorpos--;
			cmd.erase(cursorpos, 1);
		}
		break;
	case 0x0d: // enter
		if (cmd.size() == 0)
		{
			input::acquire();
			draw = 0;
		}
		if (cmd.size() > 0)
		{
			if (cmdecho)
				LOG("> %s", cmd.c_str());
			console::processCmd((char*)cmd.c_str());
			history.push_front(cmd);
			cmd.clear();
			cursorpos = 0;
			historypos = 0;
		}
		break;
	default:
		cmd.insert(cursorpos, 1, args.key);
		cursorpos++;
		break;
	}
}


void con_clear_console(int argc, char* argv[], void* user)
{
	Console* console = (Console*)user;
	console->clear();
}
