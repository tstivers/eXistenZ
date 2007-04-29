#include "precompiled.h"
#include "console/console.h"
#include "settings/settings.h"
#include "settings/jssettings.h"

namespace settings {
	struct eqstr {
		bool operator() (char* s1, char* s2) const {
			return strcmp(s1, s2) == 0;
		}
	};

	typedef stdext::hash_map<char*, Setting*, hash_char_ptr> settings_hash_map;

	bool standard_setter(char* name, void* value);
	bool standard_getter(char* name, void* value);

	settings_hash_map settings_map;
	void con_settings(int argc, char* argv[], void* user);
};

void settings::init(void)
{
	con::addCommand("settings", con_settings);
}

void settings::release(void)
{
	settings_hash_map::iterator iter;
	while(!settings_map.empty())
	{
		Setting* setting = (*settings_map.begin()).second;
		settings_map.erase(settings_map.begin());
		delete setting;
	}
}
					    
void settings::addsetting(char* name, U8 type, U32 flags, setFunction setter, getFunction getter, void* data)
{
	Setting* setting = new Setting();
	setting->name = _strdup(name);
	setting->type = type;
	setting->flags = flags;
	setting->data = data;

	if(data)
		setting->data = data;
	else switch(type) {	
			case TYPE_STRING:
				setting->data = malloc(MAX_PATH);
				((char*)(setting->data))[0] = 0;
				setting->flags |= FLAG_MANAGE_MEM;
				break;
			case TYPE_INT:
				setting->data = malloc(sizeof(int));
				setting->flags |= FLAG_MANAGE_MEM;
				break;
			case TYPE_FLOAT:
				setting->data = malloc(sizeof(float));
				setting->flags |= FLAG_MANAGE_MEM;
				break;
			default:
				break;
	}

	if(setter)
		setting->set = setter;
	else {
		switch(type) {
			case TYPE_STRING:
				setting->set = string_setter;
				break;
			case TYPE_INT:
				setting->set = int_setter;
				break;
			case TYPE_FLOAT:
				setting->set = float_setter;
				break;
			default:
				break;
		}
	}

	if(getter)
		setting->get = getter;
	else {
		switch(type) {
			case TYPE_STRING:
				setting->get = string_getter;
				break;
			case TYPE_INT:
				setting->get = int_getter;
				break;
			case TYPE_FLOAT:
				setting->get = float_getter;
				break;
			default:
				break;
		}
	}
	
	settings_map.insert(settings_hash_map::value_type(setting->name, setting));
	jssettings::addsetting(setting);
	//con::log(con::FLAG_INFO, "[settings::addsetting] added \"%s\"", setting->name);
}

settings::Setting* settings::findsetting(char* name)
{
	settings_hash_map::iterator iter = settings_map.find(name);
	if(iter != settings_map.end()) {
		//con::log(con::LVL_INFO, "[settings::addsetting] looked up \"%s\"", name);
		return (*iter).second;
	}
	else
		return NULL;
}

bool settings::string_setter(Setting* setting, void* value)
{	
	strcpy((char*)setting->data, (char*)value);
	return true;
}

bool settings::int_setter(Setting* setting, void* value)
{
	*((int*)setting->data) = *((int*)value);
	return true;
}

bool settings::float_setter(Setting* setting, void* value)
{
	*((float*)setting->data) = *((float*)value);
	return true;
}

bool settings::string_getter(Setting* setting, void** value)
{
	*value = setting->data;		
	return true;
}

bool settings::int_getter(Setting* setting, void** value)
{
	*value = setting->data;
	return true;
}

bool settings::float_getter(Setting* setting, void** value)
{
	*value = setting->data;
	return true;
}


bool settings::setstring(char* name, char* value)
{
	Setting* setting;

	ASSERT((setting = findsetting(name)));
	if(!(setting = findsetting(name)))
		return false;

	return setting->set(setting, value);
}

bool settings::setint(char* name, int value)
{
	Setting* setting;

	ASSERT((setting = findsetting(name)));
	if(!(setting = findsetting(name)))
		return false;

	return setting->set(setting, &value);
}

bool settings::setfloat(char* name, float value)
{
	Setting* setting;

	ASSERT((setting = findsetting(name)));
	if(!(setting = findsetting(name)))
		return false;

	return setting->set(setting, &value);
}

char* settings::getstring(char* name)
{
	Setting* setting;
	char* retval = NULL;

	ASSERT((setting = findsetting(name)));
	if(!(setting = findsetting(name)))
		return false;

	setting->get(setting, (void**)&retval);
	
	return retval;
}

int settings::getint(char* name)
{
	Setting* setting;
	int* retval = NULL;

	ASSERT((setting = findsetting(name)));
	if(!(setting = findsetting(name)))
		return false;

	setting->get(setting, (void**)&retval);
	
	return *retval;
}

float settings::getfloat(char* name)
{
	Setting* setting;
	float* retval = NULL;

	
	ASSERT((setting = findsetting(name)));
	if(!(setting = findsetting(name)))
		return false;

	setting->get(setting, (void**)&retval);
	
	return *retval;
}

void settings::dump(char* pattern, bool sort)
{
	std::list<std::string> blah;

	if(pattern) {
		for(settings_hash_map::iterator iter = settings_map.begin(); iter != settings_map.end(); iter++)
			if(wildcmp(pattern, ((Setting*)((*iter).second))->name))
				blah.push_back((std::string)((Setting*)((*iter).second))->name);
	} else {
	for(settings_hash_map::iterator iter = settings_map.begin(); iter != settings_map.end(); iter++)
		blah.push_back((std::string)((Setting*)((*iter).second))->name);
	}

	if(sort)
		blah.sort();

	for(std::list<std::string>::iterator li = blah.begin(); li != blah.end(); li++)
	{
		settings_hash_map::iterator iter = settings_map.find((char*)(*li).c_str());		
		Setting* setting = (Setting*)((*iter).second);
		switch(setting->type){
			case TYPE_STRING:
				con::log(con::FLAG_INFO, "%s%s = \"%s\";", setting->flags & FLAG_READONLY ? "//" : "", setting->name, getstring(setting->name));
				break;
			case TYPE_INT:
				con::log(con::FLAG_INFO, "%s%s = %i;", setting->flags & FLAG_READONLY ? "//" : "", setting->name, getint(setting->name));
				break;
			case TYPE_FLOAT:
				con::log(con::FLAG_INFO, "%s%s = %f;", setting->flags & FLAG_READONLY ? "//" : "", setting->name, getfloat(setting->name));
				break;
			default:
				LOG2("// %s = <unkown type>", setting->name);
				break;
		}
	}
}

void settings::con_settings(int argc, char* argv[], void* user)
{
	if(argc == 1)
		dump();
	else
		dump(argv[1]);
}