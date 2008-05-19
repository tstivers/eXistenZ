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
	void resize(int width, int height);
	void goFullScreen(bool fullscreen);
	void setResetDevice();
};