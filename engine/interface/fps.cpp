/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: fps.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#include "precompiled.h"
#include "interface/fps.h"
#include "interface/interface.h"
#include "render/render.h"
#include "render/d3dfont.h"
#include "timer/timer.h"

using namespace ui;

FPS::FPS()
{
	d3dfont = NULL;
	frames = 0;
	last_ms = timer::time_ms;
	fps = 0;
	draw = 1;
}

FPS::~FPS()
{
	if(d3dfont)
		delete d3dfont;
}

void FPS::render()
{
	if(!draw)
		return;

	char buf[32];	

	if(d3dfont == NULL)
	{
		d3dfont = new CD3DFont("Devotion", 20, 0);
		d3dfont->InitDeviceObjects(render::device);
		d3dfont->RestoreDeviceObjects();
	}
	
	frames++;	

	if(timer::time_ms - last_ms >= 1000)
	{
		fps = (double)frames / ((double)((double)timer::time_ms - (double)last_ms) / 1000);
		frames = 0;
		last_ms = timer::time_ms;
	}

	sprintf(buf, "%.2f fps", fps);
	d3dfont->DrawText(xpos, ypos, D3DCOLOR_XRGB(255, 255, 255), buf);
}
