/////////////////////////////////////////////////////////////////////////////
// dx.h
// rendering system directx functions interface
// $Id$
//

#pragma once

namespace d3d {
	bool init();
	void release();
	void clear();
	void present();
	ID3DXFont* createFont(HFONT font);
	void begin();
	void end();
	bool checkDevice();
};