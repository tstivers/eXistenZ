/////////////////////////////////////////////////////////////////////////////
// dx.h
// rendering system directx functions interface
// $Id: timer.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace timer {
	void init(void);
	void release(void);

	void doTick();

	extern unsigned int time_ms;
	extern float delta_ms;
};