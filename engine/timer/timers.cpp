#include "precompiled.h"
#include "timer/timer.h"
#include "timer/timers.h"
#include "console/console.h"

namespace timer {
	class Timer {
	public:
		Timer(const std::string& name, const std::string& action, const unsigned int frequency_ms, const unsigned int next_ms) :
		  name(name), action(action), frequency_ms(frequency_ms), next_ms(next_ms){};
		~Timer() {};

		std::string name;
		std::string action;
		unsigned int frequency_ms;
		unsigned int next_ms;
		bool operator<(const timer::Timer& right) const {return this->name < right.name;};
	};

	typedef std::set<Timer> timers_t;
	timers_t timers;
}

using namespace timer;

bool timer::addTimer(const std::string& name, const std::string& action, unsigned int frequency_ms /* = 0 */, unsigned int next_ms /* = 0 */)
{
	Timer t(name, action, frequency_ms, next_ms);
	if(!t.next_ms)
		t.next_ms = timer::game_ms + t.frequency_ms;
	timers_t::iterator i = timers.find(t);
	if(i != timers.end())
		*i = t;
	else
		timers.insert(t);

	return true;
}

bool timer::removeTimer(const std::string& name)
{
	Timer t(name, std::string(""), 0, 0);
	timers_t::iterator i = timers.find(t);
	if(i != timers.end())
	{
		timers.erase(i);
		return true;
	}
		
	return false;
}

void timer::fireTimers()
{
	for(timers_t::iterator i = timers.begin(); i != timers.end(); i++)
	{
		if((*i).next_ms <= timer::game_ms)
		{
			con::processCmd((*i).action.c_str());
			if((*i).frequency_ms != 0)
				(*i).next_ms = timer::game_ms + (*i).frequency_ms;
			else
				timers.erase(i);
		}
	}
}