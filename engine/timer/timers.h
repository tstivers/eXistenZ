#pragma once

namespace timer {
	bool addTimer(const std::string& name, const std::string& action, unsigned int frequency_ms = 0, unsigned int start_ms = 0);
	bool removeTimer(const std::string& name);
	void fireTimers();
}