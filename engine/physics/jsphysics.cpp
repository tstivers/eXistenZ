#include "precompiled.h"
#include "physics/jsphysics.h"
#include "scene/scene.h"
#include "component/component.h"

using namespace jsphysics;
using namespace physics;
using namespace script;

namespace jsscript
{
	inline jsval to_jsval(JSContext* cx, component::Component* object)
	{
		return object ? OBJECT_TO_JSVAL(object->getScriptObject()) : JSVAL_NULL;
	}
}

namespace jsphysics
{

	static JSClass class_ops =
	{
		"PhysicsManager",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	// static JSBool propGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		JS_PS_END
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, minargs, flags),
		JS_FN("setParameter", WRAP_FASTNATIVE(PhysicsManager::setParameter), 2, 0),	
		JS_FN("setGroupCollisionFlag", WRAP_FASTNATIVE(PhysicsManager::setGroupCollisionFlag), 3, 0),
		JS_FN("getActorsInSphere", WRAP_FASTNATIVE(PhysicsManager::getActorsInSphere), 2, 0),
		JS_FN("getFirstActorInRay", WRAP_FASTNATIVE(PhysicsManager::getFirstActorInRay), 3, 0),
		JS_FS_END
	};

	void init();
	JSBool loadDynamicsXML(JSContext *cx, uintN argc, jsval *vp);
}

ScriptedObject::ScriptClass PhysicsManager::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(PhysicsManager, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<PhysicsManager, GlobalObject>(engine);
}


REGISTER_STARTUP_FUNCTION(jsphysics, jsphysics::init, 10);

void jsphysics::init()
{
	script::gScriptEngine->AddFunction("loadDynamicsXML", 1, loadDynamicsXML);
}

// is this actually used by anything?
JSBool jsphysics::loadDynamicsXML(JSContext *cx, uintN argc, jsval *vp)
{
	string filename =  JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx,vp)[0]));
	
	if(!scene::g_scene)
	{
		JS_ReportError(cx, "unable to load dynamics: no scene loaded");
		return JS_FALSE;
	}

	scene::g_scene->getPhysicsManager()->getShapeEntry(filename);

	return JS_TRUE;
}

JSObject* jsphysics::CreatePhysicsManagerObject(PhysicsManager* manager)
{
	JSContext* cx = script::gScriptEngine->GetContext();

	JS_EnterLocalRootScope(cx);
	// TODO: get scene object from manager
	JSObject* scene = script::gScriptEngine->GetObject("system.scene", true);
	JSObject* obj = JS_DefineObject(
		cx, 
		scene, 
		"physics", 
		PhysicsManager::m_scriptClass.classDef,
		PhysicsManager::m_scriptClass.prototype, 
		JSPROP_READONLY | JSPROP_PERMANENT);
	JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(manager));
	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

void jsphysics::DestroyPhysicsManagerObject(PhysicsManager* manager)
{
	// todo: get script object as manager->getScene()->getScriptObject()
	// or just not even bother since the scene should go away in theory
	JSContext* cx = script::gScriptEngine->GetContext();
	JSObject* scene = script::gScriptEngine->GetObject("system.scene", true);
	JSBool found;
	JS_SetPropertyAttributes(cx, scene, "physics", 0, &found);
	JS_DeleteProperty(cx, scene, "physics");
}