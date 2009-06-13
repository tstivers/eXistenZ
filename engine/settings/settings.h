#pragma once

namespace eXistenZ
{
	class SettingsManager : public script::ScriptedObject
	{
	public:
		SettingsManager();
		~SettingsManager();

		void load(const string& filename);
		void save(const string& filename);

		template<typename T>
		void add(const string& name, const T defaultValue);

		template<typename T>
		T get(const string& name);

		template<typename T>
		void set(const string& name, const T value);
		
		// for js interface
		static ScriptClass m_scriptClass;
	
	protected:
		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();

		typedef map<string, string> settings_map;
		settings_map m_settings;
	};
}