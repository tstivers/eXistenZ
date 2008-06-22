#include "precompiled.h"
#include "sound/sound.h"
#include "sound/jssoundmanager.h"

using namespace sound;
using namespace script;

namespace jsscript
{
	inline jsval to_jsval(JSContext* cx, Sound* arg)
	{
		return OBJECT_TO_JSVAL(arg->getScriptObject());
	}

	inline jsval to_jsval(JSContext* cx, FMOD::Channel* channel)
	{
		return JSVAL_NULL;
	}
}

namespace sound
{
	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	// static JSBool propGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSClass class_ops =
	{
		"SoundManager",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		JS_PS_END
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, minargs, flags),
		JS_FN("getSound", WRAP_NATIVE(SoundManager::getSound), 1, 1, 0),
		JS_FN("playSound", WRAP_NATIVE(SoundManager::playSound), 2, 2, 0),
		JS_FN("playSound3d", WRAP_NATIVE(SoundManager::playSound3d), 3, 3, 0),
		JS_FS_END
	};

	jsval to_jsval(JSContext* cx, Sound* sound)
	{
		return JSVAL_NULL;
	}
}

ScriptedObject::ScriptClass SoundManager::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(SoundManager, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<SoundManager, GlobalObject>(engine);
}

JSObject* sound::CreateSoundManagerObject(SoundManager* manager)
{
	JSContext* cx = script::gScriptEngine->GetContext();

	JS_EnterLocalRootScope(cx);
	// TODO: get scene object from manager
	JSObject* scene = script::gScriptEngine->GetObject("system.scene", true);
	JSObject* obj = JS_DefineObject(
		cx, 
		scene, 
		"sound", 
		SoundManager::m_scriptClass.classDef,
		SoundManager::m_scriptClass.prototype, 
		JSPROP_READONLY | JSPROP_PERMANENT);
	JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(manager));
	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

void sound::DestroySoundManagerObject(SoundManager* manager)
{
	// todo: get script object as manager->getScene()->getScriptObject()
	// or just not even bother since the scene should go away in theory
	JSContext* cx = script::gScriptEngine->GetContext();
	JSObject* scene = script::gScriptEngine->GetObject("system.scene", true);
	JSBool found;
	JS_SetPropertyAttributes(cx, scene, "sound", 0, &found);
	JS_DeleteProperty(cx, scene, "sound");
}