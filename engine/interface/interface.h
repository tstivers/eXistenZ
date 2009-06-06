#pragma once

#include "common/inputevents.h"

namespace ui
{
	void init();
	void release();
	void render();
	void charpressed(const eXistenZ::KeyEventArgs& args);
	void keypressed(const eXistenZ::KeyEventArgs& args);

	extern int has_focus;
	void reset();
};
