/////////////////////////////////////////////////////////////////////////////
// render.cpp
// rendering system implementation
// $Id$
//

#include "precompiled.h"
#include "render/render.h"
#include "render/dx.h"
#include "settings/settings.h"
#include "client/appwindow.h"
#include "console/console.h"
#include "interface/interface.h" // hack for fullscreen reset

namespace d3d {
	IDirect3D9* d3d;
	IDirect3DDevice9* d3dDevice;

	D3DPRESENT_PARAMETERS d3dpp;
};

bool d3d::init()
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.Windowed = settings::getint("system.render.fullscreen") ? FALSE : TRUE;

	if(d3dpp.Windowed) {
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.BackBufferCount = 1;
	} else {
		d3dpp.BackBufferWidth = settings::getint("system.render.resolution.x");
		d3dpp.BackBufferHeight = settings::getint("system.render.resolution.y");
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		//d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
		d3dpp.BackBufferCount = settings::getint("system.render.backbuffercount");
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	}

	if(render::wait_vtrace)
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	else
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	
	UINT adapter = settings::getint("system.render.device");
	D3DDEVTYPE devicetype = D3DDEVTYPE_HAL;

	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	for (UINT i = 0; i < d3d->GetAdapterCount(); i++)
	{
		D3DADAPTER_IDENTIFIER9 id;
		HRESULT res = d3d->GetAdapterIdentifier(i,0,&id);
		if (!strcmp(id.Description, "NVIDIA NVPerfHUD"))
		{
			adapter = i;
			devicetype = D3DDEVTYPE_REF;
			break;
		}
	}

	if(FAILED((d3d->CreateDevice(
		adapter, 
		devicetype, 
		appwindow::getHwnd(),
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,
		&d3dpp, 
		&d3dDevice)))) {
			MessageBox(NULL, "[d3d::init] unable to create device", "ERROR", MB_OK);
			exit(1);
		}

	render::device = d3dDevice;

	return true;
}

bool d3d::checkDevice()
{
	switch(d3dDevice->TestCooperativeLevel()) {
	case D3DERR_DEVICELOST: 
		LOG("[d3d::checkDevice] device lost");
		return false;
	case D3DERR_DEVICENOTRESET:
		ui::reset(); // hack for stupid d3dfont stuff
		if(FAILED(d3dDevice->Reset(&d3dpp))) {
			LOG("[d3d::checkDevice] could not reset device");
			return false;
		}
		LOG("[d3d::checkDevice] device reset");
	default:
		return true;
	}	
}

void d3d::begin()
{
	d3dDevice->BeginScene();
}

void d3d::end()
{
	d3dDevice->EndScene();
}

void d3d::clear()
{
	d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(128, 128, 128, 255), 1.0f, 0);
}

void d3d::present()
{
	d3dDevice->Present(NULL, NULL, NULL, NULL);
}

void d3d::release()
{
}

ID3DXFont* d3d::createFont(HFONT font)
{
	ID3DXFont* dxfont;
	if(D3DXCreateFont(d3dDevice, font, &dxfont) != D3D_OK)
		return NULL;

	return dxfont;
}