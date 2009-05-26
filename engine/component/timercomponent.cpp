#include "precompiled.h"
#include "entity/entity.h"
#include "component/timercomponent.h"
#include "component/jscomponent.h"
#include "timer/timers.h"

using namespace component;

REGISTER_COMPONENT_TYPE(TimerComponent, TIMERCOMPONENT);

#pragma warning(disable: 4355) // disable warning for using 'this' as an initializer

TimerComponent::TimerComponent(entity::Entity* entity, const string& name, const desc_type& desc)
: Component(entity, name, desc), m_started(false)
{
	m_delay = desc.delay;
	m_frequency = desc.frequency;
	m_action = desc.action;
	if (desc.start)
		start();
}

TimerComponent::~TimerComponent()
{
	if(m_acquired)
		release();

	if(m_scriptObject)
		destroyScriptObject();
}

void TimerComponent::acquire()
{
	if(m_acquired)
		return;

	Component::acquire();
}

void TimerComponent::release()
{
	if (m_started)
		stop();
	
	Component::release();	
}

void TimerComponent::start()
{
	if (m_started)
		stop();

	bool added = timer::addTimer(m_entity->getName() + "." + m_name, boost::bind(&TimerComponent::onTimer, this, _1), m_frequency, timer::game_ms + m_delay);
	ASSERT(added);
	m_started = true;
}

void TimerComponent::stop()
{
	if (!m_started)
		return;

	bool removed = timer::removeTimer(m_entity->getName() + "." + m_name);
	m_started = false;
}

void TimerComponent::restart()
{
	if (!m_started)
		return;

	stop();
	start();
}

void TimerComponent::fire()
{
	if (m_action)
		m_action(this);
}

void TimerComponent::setDelay(float value)
{
	m_delay = value;
	restart();
}

void TimerComponent::setFrequency(float value)
{
	m_frequency = value;
	restart();
}

void TimerComponent::setAction(function<void(Component*)> value)
{
	m_action = value;
	restart();
}

void TimerComponent::onTimer(string name)
{
	ASSERT(name == (m_entity->getName() + "." + m_name));

	if (m_action)
		m_action(this);

	if(m_frequency == 0.0)
		m_started = false;
}

JSObject* TimerComponent::createScriptObject()
{
	return jscomponent::createComponentScriptObject(this);
}

void TimerComponent::destroyScriptObject()
{
	jscomponent::destroyComponentScriptObject(this);
	m_scriptObject = NULL;
}