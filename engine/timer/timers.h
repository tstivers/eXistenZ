#pragma once

namespace timer {
	bool addTimer(const string& name, const string& action, unsigned int frequency_ms = 0, unsigned int start_ms = 0);
	bool removeTimer(const string& name);
	void fireTimers();
}