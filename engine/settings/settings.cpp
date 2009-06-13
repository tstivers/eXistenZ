#include "precompiled.h"
#include "settings/settings.h"

using namespace eXistenZ;

SettingsManager::SettingsManager()
{
}

SettingsManager::~SettingsManager()
{
}

template<typename T>
void SettingsManager::add(const string &name, const T value)
{
	ASSERT(m_settings.find(name) == m_settings.end());
	m_settings.insert(name, lexical_cast<string>(value));
}

template<typename T>
T SettingsManager::get(const string& name)
{
	ASSERT(m_settings.find(name) != m_settings.end());
	return lexical_cast<T>(m_settings.find(name)->second);
}

template<typename T>
void SettingsManager::set(const string &name, const T value)
{
	ASSERT(m_settings.find(name) != m_settings.end());
	m_settings[name] = lexical_cast<string>(value);
}

JSObject* SettingsManager::createScriptObject()
{
}

void SettingsManager::destroyScriptObject()
{
}

