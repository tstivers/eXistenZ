/////////////////////////////////////////////////////////////////////////////
// dx.h
// rendering system directx functions interface
// $Id: dx.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
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
};