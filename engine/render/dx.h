/////////////////////////////////////////////////////////////////////////////
// dx.h
// rendering system directx functions interface
// $Id: dx.h,v 1.2 2003/12/23 04:51:58 tstivers Exp $
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