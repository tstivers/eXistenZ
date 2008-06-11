#include "precompiled.h"
#include "timer/timer.h"
#include "timer/timers.h"
#include "console/console.h"

namespace timer
{
	class Timer
	{
	public:
		Timer(const string& name, const string& action, const unsigned int frequency_ms, const unsigned int next_ms) :
				name(name), action(action), frequency_ms(frequency_ms), next_ms(next_ms) {};
		Timer(const string& name, function < void(string) > action, const unsigned int frequency_ms, const unsigned int next_ms) :
				name(name), action_f(action), frequency_ms(frequency_ms), next_ms(next_ms) {};

		~Timer() {};

		string name;
		string action;
		function<void(string)> action_f;
		float frequency_ms;
		float next_ms;
	};

	typedef shared_ptr<Timer> pTimer;

	struct timer_less : public binary_function<pTimer, pTimer, bool>
	{
		bool operator()(const pTimer _Left, const pTimer _Right) const
		{
			return (_Left->next_ms > _Right->next_ms);
		}
	};

	typedef priority_queue<pTimer, deque<pTimer>, timer_less> timerqueue_t;
	typedef unordered_map<string, pTimer> timermap_t;
	timerqueue_t timer_queue;
	timermap_t timer_map;

	void addTimer(pTimer timer);
}

using namespace timer;

bool timer::addTimer(const string& name, const string& action, float frequency_ms /* = 0 */, float next_ms /* = 0 */)
{
	addTimer(pTimer(new Timer(name, action, frequency_ms, next_ms)));
	return true;
}

bool timer::addTimer(const string& name, function<void(string)> action, float frequency_ms /* = 0 */, float next_ms /* = 0 */)
{
	addTimer(pTimer(new Timer(name, action, frequency_ms, next_ms)));
	return true;
}

void timer::addTimer(pTimer timer)
{
	ASSERT(timer->frequency_ms >= 0.0f);
	if (timer->next_ms < game_ms)
		timer->next_ms = game_ms;

	removeTimer(timer->name);

	timer_map.insert(timermap_t::value_type(timer->name, timer));
	timer_queue.push(timer);
}

bool timer::removeTimer(const string& name)
{
	timermap_t::iterator i = timer_map.find(name);
	if (i == timer_map.end())
		return false;

	vector<pTimer> temp;
	while (timer_queue.top() != i->second)
	{
		temp.push_back(timer_queue.top());
		timer_queue.pop();
	}

	timer_queue.pop();

	for (vector<pTimer>::iterator j = temp.begin(); j != temp.end(); j++)
		timer_queue.push(*j);

	timer_map.erase(i);

	return false;
}

void timer::fireTimers()
{
	while (!timer_queue.empty() && (timer_queue.top()->next_ms <= game_ms))
	{
		pTimer t = timer_queue.top();
		timer_queue.pop();
		if (t->action_f)
			t->action_f(t->name);
		else
			console::processCmd(t->action.c_str());
		if (t->frequency_ms != 0.0f)
		{
			t->next_ms += t->frequency_ms;
			if (t->next_ms <= game_ms)
				t->next_ms = game_ms + 1.0f;
			timer_queue.push(t);
		}
		else
		{
			timer_map.erase(t->name);
		}
	}
}