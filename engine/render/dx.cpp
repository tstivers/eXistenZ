#include "precompiled.h"
#include "render/render.h"
#include "render/dx.h"
#include "settings/settings.h"
#include "interface/interface.h" // hack for fullscreen reset
#include "render/shapes.h"
#include "window/appwindow.h"

namespace d3d
{
	IDirect3D9* d3d;
	IDirect3DDevice9* d3dDevice;

	D3DPRESENT_PARAMETERS d3dpp;
	bool resetDevice = false;
}

using namespace d3d;

bool d3d::init()
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	d3dpp.Windowed = settings::getint("system.render.fullscreen") ? FALSE : TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24X8;
	//if(d3dpp.Windowed)
	//	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	//else
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = settings::getint("system.render.backbuffercount");

	if (render::wait_vtrace)
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	else
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	UINT adapter = settings::getint("system.render.device");
	D3DDEVTYPE devicetype = D3DDEVTYPE_HAL;

	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	for (UINT i = 0; i < d3d->GetAdapterCount(); i++)
	{
		D3DADAPTER_IDENTIFIER9 id;
		HRESULT res = d3d->GetAdapterIdentifier(i, 0, &id);
		LOG("found adapter: %s", id.Description);
		if (strstr(id.Description, "PerfHUD") != 0)
		{
			adapter = i;
			devicetype = D3DDEVTYPE_REF;
			break;
		}
	}

	d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)settings::getint("system.render.multisampletype");
	HRESULT result;
	char* err = "";
	if (d3dpp.MultiSampleType)
	{
		if (FAILED(result = d3d->CheckDeviceMultiSampleType(adapter, devicetype, d3dpp.BackBufferFormat, d3dpp.Windowed, d3dpp.MultiSampleType, &d3dpp.MultiSampleQuality)))
		{
			switch (result)
			{
			case D3DERR_INVALIDCALL:
				err = "D3DERR_INVALIDCALL";
				break;
			case D3DERR_NOTAVAILABLE:
				err = "D3DERR_NOTAVAILABLE";
				break;
			case D3DERR_INVALIDDEVICE:
				err = "D3DERR_INVALIDDEVICE";
				break;
			default:
				err = "UNKNOWN";
			}
			LOG("failed setting multisample level %i (%s)", d3dpp.MultiSampleType, err);
			d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
			d3dpp.MultiSampleQuality = 0;
		}
		else
		{
			d3dpp.MultiSampleQuality -= 1;
			LOG("set multisample level %i (%i)", d3dpp.MultiSampleType, d3dpp.MultiSampleQuality);
		}
	}

	if (FAILED(d3d->CreateDevice(
				   adapter,
				   devicetype,
				   eXistenZ::g_appWindow->getHwnd(),
				   D3DCREATE_HARDWARE_VERTEXPROCESSING,
				   &d3dpp,
				   &d3dDevice)))				   
	{
		MessageBox(NULL, "[d3d::init] unable to create device", "ERROR", MB_OK);
		exit(1);
	}

	render::device = d3dDevice;
// 	if(FAILED(render::device->GetSwapChain(0, &render::swapchain))) {
// 		MessageBox(NULL, "[d3d::init] unable to get swapchain", "ERROR", MB_OK);
// 		exit(1);
// 	}

	return true;
}

bool d3d::checkDevice()
{
	if (resetDevice)
	{
		LOG("resetting device");
		ui::reset(); // hack for stupid d3dfont stuff
		render::releaseLine(); // released the d3dxline interface
		render::releaseFont();
		d3dpp.BackBufferCount = settings::getint("system.render.backbuffercount");
		d3dpp.BackBufferWidth = render::xres;
		d3dpp.BackBufferHeight = render::yres;
		d3dpp.BackBufferFormat = d3dpp.Windowed ? D3DFMT_UNKNOWN : D3DFMT_X8R8G8B8;
		d3dpp.PresentationInterval = render::wait_vtrace ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
		HRESULT result;
		
		if (FAILED(result = d3dDevice->Reset(&d3dpp)))
		{
			char* reason = "UNKNOWN";
			switch (result)
			{
			case D3DERR_DEVICELOST:
				reason = "DEVICELOST";
				break;
			case D3DERR_DEVICEREMOVED:
				reason = "DEVICEREMOVED";
				break;
			case D3DERR_DRIVERINTERNALERROR:
				reason = "DRIVERINTERNALERROR";
				break;
			case D3DERR_OUTOFVIDEOMEMORY:
				reason = "OUTOFVIDEOMEMORY";
				break;
			}

			LOG("could not reset device (reason: %s)", reason);
			return false;
		}
		else
		{
			LOG("device reset");
			resetDevice = false;
		}
	}

	switch (d3dDevice->TestCooperativeLevel())
	{
	case D3DERR_DEVICELOST:
		LOG("device lost");
		setResetDevice();
		return false;
	case D3DERR_DEVICENOTRESET:
		setResetDevice();
		return false;
	default:
		return true;
	}
}

void d3d::resize(int width, int height)
{
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	//LOG("resizing to %i:%i", width, height);
	setResetDevice();
	checkDevice();
}

void d3d::goFullScreen(bool fullscreen)
{
	d3dpp.Windowed = settings::getint("system.render.fullscreen") ? FALSE : TRUE;

// 	if(d3dpp.Windowed) {
// 		d3dpp.BackBufferCount = 1;
// 	} else {
// 		d3dpp.BackBufferCount = settings::getint("system.render.backbuffercount");
// 	}

	setResetDevice();
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
	if (d3dDevice->Present(NULL, NULL, NULL, NULL) != D3D_OK)
		checkDevice();
	//LOG("presented");
}

void d3d::release()
{
}

ID3DXFont* d3d::createFont(HFONT font)
{
	ID3DXFont* dxfont;
	//if(D3DXCreateFont(d3dDevice, font, &dxfont) != D3D_OK)
	return NULL;

	return dxfont;
}

void d3d::setResetDevice()
{
	resetDevice = true;
}

void d3d::takeScreenShot(const string filename)
{
	int width, height;
	RECT rect;
	if (d3dpp.Windowed)
	{
		width = GetSystemMetrics(SM_CXSCREEN);
		height = GetSystemMetrics(SM_CYSCREEN);
		GetClientRect(eXistenZ::g_appWindow->getHwnd(), &rect);
		ClientToScreen(eXistenZ::g_appWindow->getHwnd(), (LPPOINT)&rect.left);
		ClientToScreen(eXistenZ::g_appWindow->getHwnd(), (LPPOINT)&rect.right);
	}
	else
	{
		width = d3dpp.BackBufferWidth;
		height = d3dpp.BackBufferHeight;
	}

	LPDIRECT3DSURFACE9 frontbuf, noalphabuf;
	d3dDevice->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &frontbuf, NULL);
	d3dDevice->CreateOffscreenPlainSurface(width, height, D3DFMT_X8R8G8B8, D3DPOOL_SCRATCH, &noalphabuf, NULL);
	d3dDevice->GetFrontBufferData(0, frontbuf);
	D3DXLoadSurfaceFromSurface(noalphabuf, NULL, NULL, frontbuf, NULL, NULL, D3DX_FILTER_NONE, 0);
	D3DXSaveSurfaceToFile(filename.c_str(), D3DXIFF_PNG, noalphabuf, NULL, d3dpp.Windowed ? &rect : NULL);
	frontbuf->Release();
	noalphabuf->Release();
}