#include "precompiled.h"
#include "entity/jsposcomponent.h"
#include "entity/poscomponent.h"
#include "entity/entity.h"
#include "script/jsvector.h"

using namespace jsentity;
using namespace entity;

namespace jsentity
{
	extern JSObject* entity_prototype;
	extern JSObject* component_prototype;

	static void initClass(ScriptEngine* engine);
	static bool parseDesc(JSContext* cx, JSObject* obj, PosComponent::desc_type& desc);

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);
	static JSBool createPosComponent(JSContext *cx, uintN argc, jsval *vp);
	static JSBool setPos(JSContext *cx, uintN argc, jsval *vp);
	static JSBool getPos(JSContext *cx, uintN argc, jsval *vp);
	static JSBool setRot(JSContext *cx, uintN argc, jsval *vp);
	static JSBool getRot(JSContext *cx, uintN argc, jsval *vp);
	static JSBool setScale(JSContext *cx, uintN argc, jsval *vp);
	static JSBool getScale(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	//static JSBool prop_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	static JSBool parentGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	static JSBool parentSetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	JSObject* poscomponent_prototype = NULL;

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FN("setPos", setPos, 1, 1, 0),
		JS_FN("getPos", getPos, 0, 0, 0),
		JS_FN("setRot", setRot, 1, 1, 0),
		JS_FN("getRot", getRot, 0, 0, 0),
		JS_FN("setScale", setScale, 1, 1, 0),
		JS_FN("getScale", getScale, 0, 0, 0),
		JS_FS_END
	};

	static JSPropertySpec class_properties[] =
	{
		//{"name", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, name_getter, NULL},
		{"parent", 0, JSPROP_PERMANENT | JSPROP_SHARED, parentGetter, parentSetter},
		JS_PS_END 
	};

	static JSClass poscomponent_class =
	{
		"Component",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};
}

REGISTER_SCRIPT_INIT(PosComponent, initClass, 20);

void jsentity::initClass(ScriptEngine* engine)
{
	poscomponent_prototype = JS_InitClass(
		engine->GetContext(),
		engine->GetGlobal(),
		component_prototype,
		&poscomponent_class,
		NULL,
		0,
		class_properties,
		class_methods,
		NULL,
		NULL);

	ASSERT(poscomponent_prototype);

	JSFunctionSpec create_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FN("createPosComponent", createPosComponent, 1, 1, 0),
		JS_FS_END
	};

	JS_DefineFunctions(engine->GetContext(), entity_prototype, create_methods);
}

bool jsentity::parseDesc(JSContext* cx, JSObject* obj, PosComponent::desc_type& desc)
{
	jsval v;
	D3DXVECTOR3 vec;
	
	getProperty(cx, obj, "pos", desc.position);
	getProperty(cx, obj, "rot", desc.rotation);
	getProperty(cx, obj, "scale", desc.scale);
	getProperty(cx, obj, "parent", desc.parentName);

	return true;
}

JSBool jsentity::createPosComponent(JSContext *cx, uintN argc, jsval *vp)
{
	Entity* e = getReserved<Entity>(cx, JS_THIS_OBJECT(cx, vp));

	PosComponent* component;
	string name = JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx, vp)[0]));
	PosComponent::desc_type desc;
	if(argc == 2 && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[1]))
		parseDesc(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[1]), desc);
	Component* poscomponent = e->createComponent(name, desc, &component);
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(poscomponent->getScriptObject()));
	return JS_TRUE;
}

JSObject* jsentity::createPosComponentObject(entity::PosComponent* component)
{
	JSContext* cx = gScriptEngine->GetContext();

	JS_EnterLocalRootScope(cx);
	JSObject* entity = component->getEntity()->getScriptObject();
	jsval cval = JSVAL_VOID;
	JS_GetProperty(cx, entity, "components", &cval);
	JSObject* components = JSVAL_TO_OBJECT(cval);
	JSObject* obj = JS_DefineObject(
		cx, 
		components, 
		component->getName().c_str(), 
		&poscomponent_class, 
		poscomponent_prototype, 
		JSPROP_ENUMERATE | JSPROP_READONLY);
	JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(component));
	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

void jsentity::destroyPosComponentObject(entity::PosComponent* component)
{
	JS_DeleteProperty(
		gScriptEngine->GetContext(), 
		component->getEntity()->getScriptObject(), 
		component->getName().c_str());
	JS_SetReservedSlot(gScriptEngine->GetContext(), component->getScriptObject(), 0, PRIVATE_TO_JSVAL(NULL));
}

JSBool jsentity::setPos(JSContext *cx, uintN argc, jsval *vp)
{
	PosComponent* pc = getReserved<PosComponent>(cx, JS_THIS_OBJECT(cx, vp));

	D3DXVECTOR3 v;
	if(jsvector::ParseVector(cx, v, argc, JS_ARGV(cx, vp)) == JS_FALSE)
	{
		JS_ReportError(cx, "posComponent.setPos: unable to parse vector");
		return JS_FALSE;
	}

	pc->setPos(v);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool jsentity::getPos(JSContext *cx, uintN argc, jsval *vp)
{
	PosComponent* pc = getReserved<PosComponent>(cx, JS_THIS_OBJECT(cx, vp));

	D3DXVECTOR3 v = pc->getPos();
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsvector::NewVector(cx, NULL, v)));

	return JS_TRUE;
}

JSBool jsentity::setRot(JSContext *cx, uintN argc, jsval *vp)
{
	PosComponent* pc = getReserved<PosComponent>(cx, JS_THIS_OBJECT(cx, vp));

	D3DXVECTOR3 v;
	if(jsvector::ParseVector(cx, v, argc, JS_ARGV(cx, vp)) == JS_FALSE)
	{
		JS_ReportError(cx, "posComponent.setRot: unable to parse vector");
		return JS_FALSE;
	}

	pc->setRot(v);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool jsentity::getRot(JSContext *cx, uintN argc, jsval *vp)
{
	PosComponent* pc = getReserved<PosComponent>(cx, JS_THIS_OBJECT(cx, vp));

	D3DXVECTOR3 v = pc->getRot();
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsvector::NewVector(cx, NULL, v)));

	return JS_TRUE;
}

JSBool jsentity::setScale(JSContext *cx, uintN argc, jsval *vp)
{
	PosComponent* pc = getReserved<PosComponent>(cx, JS_THIS_OBJECT(cx, vp));

	D3DXVECTOR3 v;
	if(jsvector::ParseVector(cx, v, argc, JS_ARGV(cx, vp)) == JS_FALSE)
	{
		JS_ReportError(cx, "posComponent.setScale: unable to parse vector");
		return JS_FALSE;
	}

	pc->setScale(v);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool jsentity::getScale(JSContext *cx, uintN argc, jsval *vp)
{
	PosComponent* pc = getReserved<PosComponent>(cx, JS_THIS_OBJECT(cx, vp));

	D3DXVECTOR3 v = pc->getScale();
	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jsvector::NewVector(cx, NULL, v)));

	return JS_TRUE;
}

JSBool jsentity::parentGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	PosComponent* pc = getReserved<PosComponent>(cx, obj);
	PosComponent* parent = pc->getParent();
	if(parent)
	{
		*vp = OBJECT_TO_JSVAL(pc->getParent()->getScriptObject());
		return JS_TRUE;
	}

	*vp = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jsentity::parentSetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	PosComponent* pc = getReserved<PosComponent>(cx, obj);
	if(JSVAL_IS_OBJECT(*vp))
	{
		if(*vp == JSVAL_NULL) // component.parent = null;
		{
			pc->setParent(NULL);
			return JS_TRUE;
		}

		JSObject* parent = JSVAL_TO_OBJECT(*vp);
		pc->setParent(getReserved<PosComponent>(cx, parent));
		return JS_TRUE;
	}
	else if(JSVAL_IS_STRING(*vp))
	{
		string name;
		jsscript::jsval_to_<string>()(cx, *vp, &name);
		pc->setParent(name);
		return JS_TRUE;
	}

	JS_ReportError(cx, "parentSetter: argument must be a position component or component name");
	return JS_FALSE;
}