/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: marker.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace render {
	void drawMarker(D3DXVECTOR3 pos, D3DXVECTOR3 color, float scale = 1.0f);
	void drawMarker(float x, float y, float z, D3DXVECTOR3 color, float scale = 10.0f);
	char* addMarker(char* name, float x, float y, float z, float r, float g, float b, float scale = 10.0f);
	void delMarker(char* name);
	void drawMarkers();
	void con_add_marker(int argc, char* argv[], void* user);
	void con_del_marker(int argc, char* argv[], void* user);
};