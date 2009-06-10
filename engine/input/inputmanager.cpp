#include "precompiled.h"
#include "inputmanager.h"
#include "jsinputmanager.h"

using namespace eXistenZ;

// static initializers
InputManager::registered_impulse_map InputManager::s_impulses;
InputManager::impulse_state_list InputManager::s_impulseState;

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}

IInputConsumer* InputManager::setFocus(IInputConsumer* consumer)
{
	if(m_currentConsumer)
		m_currentConsumer->onBlur(this, consumer);
	if(consumer)
		consumer->onFocus(this, m_currentConsumer);
	IInputConsumer* oldConsumer = m_currentConsumer;
	m_currentConsumer = consumer;
	return oldConsumer;
}

int InputManager::bindKey(int key, function<void(int, ButtonState)> action, ButtonState state)
{
	static int id = 1;

	m_keyBindMap[state].insert(make_pair(key, BindEntry(id, action)));
	return id++;
}

void InputManager::unbindKey(int id)
{
	for(int i = 0; i < 2; i++)
		for(bind_map::iterator it = m_keyBindMap[i].begin(); it != m_keyBindMap[i].end(); it++)
			if(it->second.id == id)
				m_keyBindMap[i].erase(it);
}

int InputManager::registerCustomImpulse(const string& name)
{
	ASSERT(s_impulses.left.find(name) == s_impulses.left.end());

	int id = s_impulseState.size();
	s_impulseState.push_back(ImpulseState());
	s_impulses.insert(registered_impulse_map::value_type(name, id));
	return id;
}

void InputManager::registerImpulse(const string& name, int id)
{
	ASSERT(s_impulses.left.find(name) == s_impulses.left.end());
	ASSERT(s_impulses.right.find(id) == s_impulses.right.end());

	if(id + 1 > s_impulseState.size())
		s_impulseState.resize(id + 1);
	s_impulses.insert(registered_impulse_map::value_type(name, id));
}

int InputManager::getImpulseId(const string& name)
{
	ASSERT(s_impulses.left.find(name) != s_impulses.left.end());
	return s_impulses.left.find(name)->second;
}

const string& InputManager::getImpulseName(int id)
{
	ASSERT(s_impulses.right.find(id) != s_impulses.right.end());
	return s_impulses.right.find(id)->second;
}

vector<pair<int, string>> InputManager::getImpulseList()
{
	vector<pair<int, string>> list;

	for(registered_impulse_map::left_iterator it = s_impulses.left.begin(); it != s_impulses.left.end(); it++)
		list.push_back(make_pair(it->second, it->first));

	return list;
}

void InputManager::mapKey(int key, int impulse)
{
	ASSERT(s_impulses.right.find(impulse) != s_impulses.right.end());
	unmapKey(key, impulse);
	m_keyToImpulseMap.insert(impulse_map::value_type(key, impulse));
}

void InputManager::unmapKey(int key, int impulse)
{
	m_keyToImpulseMap.erase(impulse_map::value_type(key, impulse));
}

void InputManager::mapMouseButton(int button, int impulse)
{
	ASSERT(s_impulses.right.find(impulse) != s_impulses.right.end());
	unmapMouseButton(button, impulse);
	m_mouseButtonToImpulseMap.insert(impulse_map::value_type(button, impulse));
}

void InputManager::unmapMouseButton(int button, int impulse)
{
	m_mouseButtonToImpulseMap.erase(impulse_map::value_type(button, impulse));
}

bool InputManager::impulsePressed(int id)
{
	ASSERT(s_impulses.right.find(id) != s_impulses.right.end());
	return s_impulseState[id].pressed;
}

bool InputManager::impulseReleased(int id)
{
	ASSERT(s_impulses.right.find(id) != s_impulses.right.end());
	return s_impulseState[id].released;
}

bool InputManager::impulseActive(int id)
{
	ASSERT(s_impulses.right.find(id) != s_impulses.right.end());
	ImpulseState& state = s_impulseState[id];
	return state.pressed || state.released || state.refcount;
}

void InputManager::resetImpulses()
{
	for(int i = 0, size = s_impulseState.size(); i < size; i++)
		s_impulseState[size].reset();
}

void InputManager::clearImpulses()
{
	for(int i = 0, size = s_impulseState.size(); i < size; i++)
		s_impulseState[size].clear();
}

void InputManager::onKey(const KeyEventArgs& args)
{
	// update the status of all impulses the key is bound to
	for(impulse_map::left_const_iterator it = m_keyToImpulseMap.left.find(args.key); it != m_keyToImpulseMap.left.end() && it->first == args.key; it++)
		onImpulse(it->second, args.state);

	// send the key to the current consumer, execute binds if it is not handled by the consumer
	if(m_currentConsumer)
	{
		if(m_currentConsumer->onKey(args))
			executeBinds(args.key, args.state);
	}
	else
		executeBinds(args.key, args.state);
}

void InputManager::onImpulse(int impulse, ButtonState bs)
{
	ASSERT(s_impulses.right.find(impulse) != s_impulses.right.end());

	ImpulseState& state = s_impulseState[impulse];
	if(bs == BS_PRESSED)
	{
		state.pressed = true;
		state.refcount++;
	}
	else
	{
		state.released = true;
		if(state.refcount > 0)
			state.refcount--;
	}
}

void InputManager::executeBinds(int key, ButtonState state)
{
	for(bind_map::const_iterator it = m_keyBindMap[state].find(key); it != m_keyBindMap[state].end() && it->first == key; it++)
		it->second.action(key, state);
}

JSObject* InputManager::createScriptObject()
{
	return Javascript::CreateInputManagerObject(this);
}

void InputManager::destroyScriptObject()
{
	Javascript::DestroyInputManagerObject(this);
	m_scriptObject = NULL;
}