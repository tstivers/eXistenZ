/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: uiconsole.cpp,v 1.3 2003/12/23 04:51:58 tstivers Exp $
//

#include "precompiled.h"
#include "interface/interface.h"
#include "render/render.h"
#include "settings/settings.h"
#include "console/console.h"
#include "render/dx.h"
#include "interface/uiconsole.h"
#include "input/input.h"
#include "render/d3dfont.h"
#include "timer/timer.h"

using namespace ui;

void consoleMessageCallback(U32 flags, void* obj, const char* message);
void con_clear_console(int argc, char* argv[], void* user);

Console::Console()
{
	con::addConsumer(consoleMessageCallback, this);

	while(scrollback.size() < 64){
		char* bleh = new char[512];
		bleh[0] = 0;
		scrollback.push_back(bleh);
	}

	cursorpos = 0;
	wireframe = 0;
	draw = 1;
	con::addCommand("clear", con_clear_console, this);
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
    cf.rgbColors = RGB(0,0,0); 
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
	if(!draw)
		return;

	D3DCOLOR color;
	if(ui::has_focus)
		color = D3DCOLOR_ARGB(255, 255, 255, 255);
	else
		color = D3DCOLOR_ARGB(128, 255, 255, 255);

	if(d3dfont == NULL)
	{
		d3dfont = new CD3DFont("Terminal", 9, 0);
		d3dfont->InitDeviceObjects(render::device);
		d3dfont->RestoreDeviceObjects();	
	}

	float x = (float)xpos;
	float top = (float)ypos;
	float curr_y = (float)(ypos + height - 25);

	scrollback_iter line = scrollback.begin();

	while(curr_y > top)
	{	
		d3dfont->DrawText(x, curr_y, color, *line, wireframe ? D3DFONT_WIREFRAME : 0);
		line++;
		curr_y -= 12;
	}

	char cursor = 0x01;
	if(ui::has_focus && (((int)timer::time_ms / 300) % 2))
		cmd.insert(cursorpos, 1, cursor);
	cmd.insert(0, "> ");
	d3dfont->DrawText(x, (float)(ypos + height - 10), color, cmd.c_str(), wireframe ? D3DFONT_WIREFRAME : 0);
	cmd.erase(0, 2);
	if(ui::has_focus && (((int)timer::time_ms / 300) % 2))
		cmd.erase(cursorpos, 1);
}

void Console::addMessage(const char* message)
{
	char* line = scrollback.back();
	strcpy(line, message);
	scrollback.pop_back();
	scrollback.push_front(line);
}

void Console::clear()
{
	for(scrollback_iter line = scrollback.begin(); line != scrollback.end(); line++)
		(*line)[0] = 0;
}

void Console::keypressed(char key, bool extended)
{
	//LOG3("[Console] key pressed '0x%02x' %s", key, extended ? "<extended>" : "");

	if(extended)
		switch(key) {
		case VK_LEFT:
			if(cursorpos > 0)
				cursorpos--;
			break;
		case VK_RIGHT:
			if(cursorpos < cmd.size())
				cursorpos++;
			break;
		case VK_UP:
			if((history.size() > 0) && (historypos < history.size())) {
				historypos++;
				cmd = history[historypos - 1];
				cursorpos = (unsigned int)cmd.size();
			}			
			break;
		case VK_DOWN:
			if((history.size() > 0) && (historypos > 0)) {
				historypos--;
				cmd = history[historypos];
				cursorpos = (unsigned int)cmd.size();
			}
			break;
		case VK_DELETE:
			if(cursorpos < cmd.size())
				cmd.erase(cursorpos, 1);
			break;
		case VK_HOME:
			cursorpos = 0;
			break;
		case VK_END:
			cursorpos = (unsigned int)cmd.size();
			break;
		}
	else switch(key) {
		case 0x08: // backspace
			if(cursorpos > 0) {
				cursorpos--;
				cmd.erase(cursorpos, 1);
			}
			break;
		case 0x0d: // enter
			if(cmd.size() == 0) {
				input::acquire();
				draw = 0;
			}
			if(cmd.size() > 0) {
				if(cmdecho)
					LOG2("> %s", cmd.c_str());
				con::processCmd((char*)cmd.c_str());
				history.push_front(cmd);
				cmd.clear();
				cursorpos = 0;
				historypos = 0;
			}
			break;
		default:
			cmd.insert(cursorpos, 1, key);
			cursorpos++;
			break;
	}
}

void consoleMessageCallback(U32 flags, void* obj, const char* message)
{	
	if(((Console*)obj)->filter & flags)
		((Console*)obj)->addMessage(message);
}

void con_clear_console(int argc, char* argv[], void* user)
{
	Console* console = (Console*)user;
	console->clear();
}
