#pragma once

namespace timer {
	bool addTimer(const string& name, const string& action, float frequency_ms = 0.0f, float start_ms = 0.0f);
	bool addTimer(const string& name, const function<void(string)>, float frequency_ms = 0.0f, float start_ms = 0.0f);
	bool removeTimer(const string& name);
	void fireTimers();
}