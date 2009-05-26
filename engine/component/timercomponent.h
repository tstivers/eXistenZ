#pragma once

#include "component/component.h"
#include "entity/interfaces.h"

namespace component
{
	struct TimerComponentDesc : public ComponentDesc
	{
		float delay;
		float frequency;
		function<void(Component*)> action;
		bool start;

		TimerComponentDesc()
			: delay(0.0), frequency(0.0), action(NULL), start(true)
		{}
	};

	class TimerComponent : public Component
	{
	public:
		// typedefs
		typedef TimerComponentDesc desc_type;
		typedef TimerComponent component_type;

		// constructor/destructor
		TimerComponent(entity::Entity* entity, const string& name, const desc_type& desc);
		~TimerComponent();

		// Component overloads
		int getType() { return TIMERCOMPONENT; }
		void acquire();
		void release();

		// methods
		bool isStarted() { return m_started; }
		void start();
		void stop();
		void restart();
		void fire();

		// properties
		float getDelay() { return m_delay; }
		void setDelay(float value);
		float getFrequency() { return m_frequency; }
		void setFrequency(float value);
		function<void(Component*)> getAction() { return m_action; }
		void setAction(function<void(Component*)> value);

		// script class
		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		// methods
		void onTimer(string name);

		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members		
		float m_delay;
		float m_frequency;
		function<void(Component*)> m_action;
		bool m_started;
	};
}
