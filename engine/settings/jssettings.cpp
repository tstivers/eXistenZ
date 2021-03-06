#include "precompiled.h"
#include "settings/settings.h"
#include "settings/jssettings.h"
#include "script/script.h"

namespace jssettings
{
	JSBool dump(JSContext *cx, uintN argc, jsval *vp);

	typedef stdext::hash_map<char*, settings::Setting*, hash_char_ptr_traits> propmap_hash;
	typedef propmap_hash::iterator propmap_iterator;
	typedef shared_ptr<jssettings::propmap_hash> propmap_ptr;
	typedef stdext::hash_map<JSObject*, propmap_ptr> objmap_hash;

	JSBool jsgetsetting(JSContext *cx, JSObject *obj, jsid tinyid, jsval *vp);
	JSBool jssetsetting(JSContext *cx, JSObject *obj, jsid tinyid, jsval *vp);
	objmap_hash object_map;
}

REGISTER_STARTUP_FUNCTION(jssettings, jssettings::init, 10);

void jssettings::init()
{
	script::gScriptEngine->AddFunction("system.settings.dump", 0, jssettings::dump);
}

void jssettings::release()
{
	object_map.clear();
}

JSBool jssettings::dump(JSContext *cx, uintN argc, jsval *vp)
{
	settings::dump();
	return JS_TRUE;
}


bool jssettings::addsetting(settings::Setting* setting)
{
	char objname[512];
	strcpy(objname, setting->name);
	char* propname = strrchr(objname, '.');
	if (!propname)
		return false;

	*propname = 0;
	propname++;

	JSObject* obj = script::gScriptEngine->GetObject(objname, true);

	// generate the starting value
	jsval value;
	jsdouble bleh;
	char* string_value;
	int int_value;
	float float_value;

	switch (setting->type)
	{
	case settings::TYPE_STRING:
		setting->get(setting, (void**)&string_value);
		value = STRING_TO_JSVAL(JS_NewStringCopyZ(script::gScriptEngine->GetContext(), string_value));
		break;
	case settings::TYPE_INT:
		setting->get(setting, (void**)&int_value);
		value = INT_TO_JSVAL(int_value);
		break;
	case settings::TYPE_FLOAT:
		setting->get(setting, (void**)&float_value);
		bleh = float_value;
		JS_NewNumberValue(script::gScriptEngine->GetContext(), bleh, &value);
		break;
	}

	// currobj points to the object for the property, curr = prop name
	int flags = JSPROP_ENUMERATE | JSPROP_PERMANENT | JSPROP_ENUMERATE;
	if (setting->flags & settings::FLAG_READONLY)
		flags |= JSPROP_READONLY;
	script::gScriptEngine->AddProperty(obj, propname, value, jsgetsetting, jssetsetting, flags);

	propmap_ptr p;
	objmap_hash::iterator i = object_map.find(obj);
	if (i == object_map.end())
	{
		p = propmap_ptr(new propmap_hash());
		object_map.insert(objmap_hash::value_type(obj, p));
	}
	else
	{
		p = i->second;
	}

	p->insert(propmap_hash::value_type(_strdup(propname), setting));

	return true;
}

JSBool jssettings::jsgetsetting(JSContext *cx, JSObject *obj, jsid tinyid, jsval *vp)
{
	objmap_hash::iterator i = object_map.find(obj);
	ASSERT(i != object_map.end());
	propmap_ptr props = i->second;

	jsval id;
	JS_IdToValue(cx, tinyid, &id);

	propmap_iterator iter = props->find(JS_GetStringBytes(JS_ValueToString(cx, id)));
	ASSERT(iter != props->end());

	settings::Setting* setting = (*iter).second;

	switch (setting->type)
	{
	case settings::TYPE_STRING:
		char* string_value;
		setting->get(setting, (void**)&string_value);
		*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, string_value));
		break;
	case settings::TYPE_INT:
		*vp = INT_TO_JSVAL((int)(*((int*)setting->data)));
		break;
	case settings::TYPE_FLOAT:
		jsdouble d;
		d = (float)(*((float*)setting->data));
		JS_NewNumberValue(cx, d, vp);
		break;
	case settings::TYPE_VECTOR:

	default:
		break;
	}

	return JS_TRUE;
}

JSBool jssettings::jssetsetting(JSContext *cx, JSObject *obj, jsid tinyid, jsval *vp)
{
	objmap_hash::iterator i = object_map.find(obj);
	ASSERT(i != object_map.end());
	propmap_ptr props = i->second;

	jsval id;
	JS_IdToValue(cx, tinyid, &id);

	propmap_iterator iter = props->find(JS_GetStringBytes(JS_ValueToString(cx, id)));
	ASSERT(iter != props->end());

	settings::Setting* setting = (*iter).second;

	switch (setting->type)
	{
	case settings::TYPE_STRING:
		setting->set(setting, JS_GetStringBytes(JS_ValueToString(cx, *vp)));
		//strcpy((char*)setting->data, JS_GetStringBytes(JS_ValueToString(cx, *vp)));
		break;
	case settings::TYPE_INT:
		JS_ValueToInt32(cx, *vp, (int32*)setting->data);
		break;
	case settings::TYPE_FLOAT:
		jsdouble dvalue;
		JS_ValueToNumber(cx, *vp, &dvalue);
		float f;
		f = dvalue;
		setting->set(setting, &f);
		//*((float*)setting->data) = (float)dvalue;
		break;
	default:
		break;
	}

	return JS_TRUE;
}

