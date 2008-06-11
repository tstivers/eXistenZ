#include "precompiled.h"
#include "interface/fps.h"
#include "interface/interface.h"
#include "render/render.h"
#include "render/font.h"
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
	if (d3dfont)
		delete d3dfont;
}

void FPS::reset()
{
	delete d3dfont;
	d3dfont = NULL;
}

void FPS::render()
{
	if (!draw)
		return;

	char buf[32];

	if (d3dfont == NULL)
	{
		d3dfont = new CD3DFont("Verdana", 20);
		d3dfont->InitDeviceObjects(render::device);
		d3dfont->RestoreDeviceObjects();
	}

	frames++;

	if (timer::time_ms - last_ms >= 1000)
	{
		fps = (float)((double)frames / ((double)((double)timer::time_ms - (double)last_ms) / 1000));
		frames = 0;
		last_ms = timer::time_ms;
	}

	sprintf(buf, "%.2f fps", fps);
	d3dfont->DrawText((float)xpos, (float)ypos, D3DCOLOR_XRGB(255, 255, 255), buf);
}
