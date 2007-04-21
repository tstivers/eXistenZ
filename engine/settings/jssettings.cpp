/////////////////////////////////////////////////////////////////////////////
// console.cpp
// console class
// $Id$
//
#include "precompiled.h"
#include "console/console.h"
#include "settings/settings.h"
#include "settings/jssettings.h"
#include "script/script.h"

#pragma warning( disable : 4311 4312 )

void jssettings::init()
{
	gScriptEngine->AddFunction("system.settings.dump", 0, jssettings::dump);
}

JSBool jssettings::dump(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval)
{
	settings::dump();
	return BOOLEAN_TO_JSVAL(TRUE);
}


bool jssettings::addsetting(settings::Setting* setting)
{
	char objname[512];
	strcpy(objname, setting->name);
	char* propname = strrchr(objname, '.');
	if(!propname)
		return false;

	*propname = 0;
	propname++;

	JSObject* obj = gScriptEngine->GetObject(objname, true);

	// generate the starting value
	jsval value;
	jsdouble bleh;
	char* string_value;
	int int_value;
	float float_value;

	switch(setting->type)
	{
	case settings::TYPE_STRING:
		setting->get(setting, (void**)&string_value);
		value = STRING_TO_JSVAL(JS_NewStringCopyZ(gScriptEngine->GetContext(), string_value));
		break;
	case settings::TYPE_INT:
		setting->get(setting, (void**)&int_value);
		value = INT_TO_JSVAL(int_value);
		break;
	case settings::TYPE_FLOAT:
		setting->get(setting, (void**)&float_value);
		bleh = float_value;
		JS_NewDoubleValue(gScriptEngine->GetContext(), bleh, &value);
		break;
	default:
		ASSERT(1);
		return false;
		break;
	}

	// currobj points to the object for the property, curr = prop name
	int flags = JSPROP_ENUMERATE | JSPROP_PERMANENT;
	if(setting->flags & settings::FLAG_READONLY)
		flags |= JSPROP_READONLY;
	gScriptEngine->AddProperty(obj, propname, value, jsgetsetting, jssetsetting, flags);	
	
	jsval jspropmap;
	propmap_hash* props;

	JS_GetReservedSlot(gScriptEngine->GetContext(), obj, 0, &jspropmap);
	props = (propmap_hash*)JSVAL_TO_PRIVATE(jspropmap);

	if(!props) {		
		// make new map and set it
		props = new propmap_hash();
		JS_SetReservedSlot(gScriptEngine->GetContext(), obj, 0, PRIVATE_TO_JSVAL(props));
	}

	props->insert(propmap_hash::value_type(_strdup(propname), setting));
	
	return true;
}

JSBool jssettings::jsgetsetting(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	jsval jspropmap;
	propmap_hash* props;	
	
	JS_GetReservedSlot(cx, obj, 0, &jspropmap);
	props = (propmap_hash*)JSVAL_TO_PRIVATE(jspropmap);

	if(!props)
		return JS_FALSE;

	propmap_iterator iter = props->find(JS_GetStringBytes(JS_ValueToString(cx, id)));
	if(iter == props->end())
		return JS_FALSE;

	settings::Setting* setting = (*iter).second;
	jsdouble bleh;
	char* string_value;
	switch(setting->type)
	{
	case settings::TYPE_STRING:
		setting->get(setting, (void**)&string_value);
		*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, string_value));
		break;
	case settings::TYPE_INT:
		*vp = INT_TO_JSVAL((int)(*((int*)setting->data)));
		break;
	case settings::TYPE_FLOAT:
		bleh = (float)(*((float*)setting->data));
		JS_NewDoubleValue(cx, bleh, vp);
		break;
	default:
		break;
	}

	return JS_TRUE;
}

JSBool jssettings::jssetsetting(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	jsval jspropmap;
	propmap_hash* props;	
	
	JS_GetReservedSlot(cx, obj, 0, &jspropmap);
	props = (propmap_hash*)JSVAL_TO_PRIVATE(jspropmap);

	if(!props)
		return JS_FALSE;

	propmap_iterator iter = props->find(JS_GetStringBytes(JS_ValueToString(cx, id)));
	if(iter == props->end())
		return JS_FALSE;

	settings::Setting* setting = (*iter).second;

	switch(setting->type)
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

