#pragma once

#include "common/inputevents.h"

namespace eXistenZ
{
	class IInputConsumer
	{
	public:
		virtual bool onKey(const KeyEventArgs& args) { return true; }
		virtual bool onChar(const KeyEventArgs& args) { return true; }
		virtual bool onMouseMove(const MouseMoveEventArgs& args) { return true; }
		virtual bool onMouseButton(const MouseButtonEventArgs& args) { return true; }
		// TODO: gamepad sticks, buttons

		virtual void onFocus(InputManager* manager, IInputConsumer* previous) {};
		virtual void onBlur(InputManager* manager, IInputConsumer* next) {};
	};

	struct ImpulseState
	{
		bool pressed;
		bool released;
		int refcount;
		ImpulseState() : pressed(false), released(false), refcount(0) {}
		void reset() { pressed = false; released = false; refcount = 0; }
		void clear() { pressed = false; released = false; }
	};

	struct BindEntry
	{
		int id;
		function<void(int, ButtonState)> action;
		BindEntry(int id, function<void(int, ButtonState)> action) : id(id), action(action) {}
	};

	class InputManager : public script::ScriptedObject
	{
	public:
		InputManager();
		~InputManager();

		// set/get the current consumer
		IInputConsumer* setFocus(IInputConsumer* consumer);
		IInputConsumer* getFocus() { return m_currentConsumer; }

		// key->function binding
		int bindKey(int key, function<void(int, ButtonState)>, ButtonState = BS_PRESSED);
		void unbindKey(int id);

		// key/button->impulse mapping
		void mapKey(int key, int impulse);
		void unmapKey(int key, int impulse);
		void mapMouseButton(int button, int impulse);
		void unmapMouseButton(int button, int impulse);

		// impulse registration
		static int registerCustomImpulse(const string& name);
		static void registerImpulse(const string& name, int id);
		static int getImpulseId(const string& name);
		static const string& getImpulseName(int id);
		static vector<pair<int, string>> getImpulseList();

		// impulse query
		bool impulsePressed(int id); // true if impulse was pressed this tick
		bool impulseReleased(int id); // true if impulse was released this tick
		bool impulseActive(int id); // true if impulse was active this tick (pressed || down || released)
		void resetImpulses(); // DEBUG: resets flags and refcount
		void clearImpulses(); // clears all edge-trigger flags

		// wired up to appwindow
		void onKey(const KeyEventArgs& args);
		void onChar(const KeyEventArgs& args);
		void onMouseMove(const MouseMoveEventArgs& args);
		void onRawMouseMove(const RawMouseMoveEventArgs& args);
		void onMouseButton(const MouseButtonEventArgs& args);
		void onRawMouseButton(const MouseButtonEventArgs& args);
		void onImpulse(int impulse, ButtonState state);

		// TODO: xinput gamepad functions (GamepadManager class)

		// for js interface
		static ScriptClass m_scriptClass;

	protected:
		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();
		void initScriptObject();

		// methods
		void executeBinds(int key, ButtonState state);

		// typedefs
		typedef bimap<multiset_of<int>, multiset_of<int>, set_of_relation<>> impulse_map;
		typedef bimap<string, int> registered_impulse_map;
		typedef vector<ImpulseState> impulse_state_list;
		typedef multimap<int, BindEntry> bind_map;

		// fields
		static registered_impulse_map s_impulses;
		static impulse_state_list s_impulseState;

		impulse_map m_keyToImpulseMap;
		impulse_map m_mouseButtonToImpulseMap;
		bind_map m_keyBindMap[2];
		IInputConsumer* m_currentConsumer;
	};

	extern unique_ptr<InputManager> g_inputManager;
}