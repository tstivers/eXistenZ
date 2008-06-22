#include "precompiled.h"
#include "entity/jsentity.h"
#include "entity/entity.h"
#include "entity/entitymanager.h"
#include "entity/component.h"

using namespace jsentity;
using namespace entity;
using namespace script;

namespace jsentity
{
	// property declarations
	// static JSBool propGetter/propSetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	// method declarations
	// static JSBool methodName(JSContext *cx, uintN argc, jsval *vp);
	static JSBool removeComponent(JSContext *cx, uintN argc, jsval *vp);

	// class ops
	static JSBool entityResolveOp(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp);
	static JSBool componentsResolveOp(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp);
	static JSBool componentsEnumerateOp(JSContext *cx, JSObject *obj, JSIterateOp enum_op, jsval *statep, jsid *idp);

	JSClass class_def =
	{
		"Entity",
		JSCLASS_HAS_RESERVED_SLOTS(1) | JSCLASS_NEW_RESOLVE | JSCLASS_NEW_RESOLVE_GETS_START,
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, (JSResolveOp)entityResolveOp,
		JS_ConvertStub,  JS_FinalizeStub
	};

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		{"name", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, PropertyGetter<Entity, const string&, &Entity::getName>, NULL},
		{0,0,0,0,0}
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FN("acquire", (JSNativeCall<BOOST_TYPEOF(Entity::acquire), &Entity::acquire>), 0, 0, 0),
		JS_FN("release", (JSNativeCall<BOOST_TYPEOF(Entity::release), &Entity::release>), 0, 0, 0),
		JS_FN("removeComponent", (JSNativeCall<BOOST_TYPEOF(Entity::removeComponent), &Entity::removeComponent>), 1, 1, 0),
		JS_FS_END
	};

	JSClass components_class = 
	{
		"Components",
		JSCLASS_HAS_RESERVED_SLOTS(1) | JSCLASS_NEW_ENUMERATE | JSCLASS_NEW_RESOLVE | JSCLASS_NEW_RESOLVE_GETS_START,
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		(JSEnumerateOp)componentsEnumerateOp, (JSResolveOp)componentsResolveOp,
		JS_ConvertStub,  JS_FinalizeStub
	};

	static JSObject* components_prototype = NULL;
}

Entity::ScriptClass entity::Entity::m_scriptClass = 
{
	&class_def,
	class_properties,
	class_methods,
	NULL
};

static void initEntityClass(ScriptEngine* engine)
{
	Entity::m_scriptClass.prototype = JS_InitClass(
		engine->GetContext(),
		engine->GetGlobal(),
		NULL,
		Entity::m_scriptClass.classDef,
		NULL,
		0,
		Entity::m_scriptClass.properties,
		Entity::m_scriptClass.methods,
		NULL,
		NULL);

	ASSERT(Entity::m_scriptClass.prototype);

	components_prototype = JS_InitClass(
		engine->GetContext(),
		engine->GetGlobal(),
		NULL,
		&components_class,
		NULL,
		0,
		NULL,
		NULL,
		NULL,
		NULL);

	ASSERT(components_prototype);
}

REGISTER_SCRIPT_INIT(Entity, initEntityClass, 10);

JSObject* jsentity::createEntityObject(Entity* entity)
{
	JSContext* cx = gScriptEngine->GetContext();

	JS_EnterLocalRootScope(cx);
	JSObject* manager = entity->getManager()->getScriptObject();
	JSObject* obj = JS_DefineObject(
		cx, 
		manager, 
		entity->getName().c_str(), 
		Entity::m_scriptClass.classDef, 
		Entity::m_scriptClass.prototype, 
		JSPROP_ENUMERATE | JSPROP_READONLY);
	JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(entity));
	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

void jsentity::destroyEntityObject(entity::Entity* entity)
{
	JS_DeleteProperty(
		gScriptEngine->GetContext(), 
		entity->getManager()->getScriptObject(), 
		entity->getName().c_str());
	JS_SetReservedSlot(gScriptEngine->GetContext(), entity->getScriptObject(), 0, PRIVATE_TO_JSVAL(NULL));
}

JSBool jsentity::removeComponent(JSContext *cx, uintN argc, jsval *vp)
{
	Entity* entity = GetReserved<Entity>(cx, JS_THIS_OBJECT(cx, vp));

	if(!JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
	{
		JS_ReportError(cx, "removeComponent: argument not string");
		return JS_FALSE;
	}

	entity->removeComponent(JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));
	return JS_TRUE;
}

JSBool jsentity::entityResolveOp(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp)
{
	if(!JSVAL_IS_STRING(id) || obj == Entity::m_scriptClass.prototype)
	{
		*objp = NULL;
		return JS_TRUE;
	}

	Entity* entity = GetReserved<Entity>(cx, obj);

	string name = JS_GetStringBytes(JSVAL_TO_STRING(id));
	if(name == "components")
	{
		JSObject* components = JS_DefineObject(cx, obj, "components", &components_class, components_prototype, JSPROP_PERMANENT | JSPROP_READONLY);
		JS_SetReservedSlot(cx, components, 0, PRIVATE_TO_JSVAL(entity));
		ASSERT(components);
		*objp = obj;
	}
	else
		*objp = NULL;

	return JS_TRUE;
}

JSBool jsentity::componentsResolveOp(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp)
{
	if(!JSVAL_IS_STRING(id) || obj == components_prototype)
	{
		*objp = NULL;
		return JS_TRUE;
	}

	JSObject* jsentity = JS_GetParent(cx, obj);
	Entity* entity = GetReserved<Entity>(cx, jsentity);

	string name = JS_GetStringBytes(JSVAL_TO_STRING(id));
	if(Component* c = entity->getComponent(name))
	{
		c->getScriptObject();
		*objp = obj;
	}
	else
		*objp = NULL;

	return JS_TRUE;
}

JSBool jsentity::componentsEnumerateOp(JSContext *cx, JSObject *obj, JSIterateOp enum_op, jsval *statep, jsid *idp)
{
	if(obj == components_prototype)
	{
		*statep = JSVAL_NULL;
		return JS_TRUE;
	}

	Entity* entity = GetReserved<Entity>(cx, obj);
	ASSERT(entity);

	Entity::component_iterator* it;

	switch(enum_op)
	{
	case JSENUMERATE_INIT:
		it = new Entity::component_iterator();
		*it = entity->begin();
		*statep = PRIVATE_TO_JSVAL(it);
		if(idp)
			*idp = INT_TO_JSVAL(entity->getComponentCount());
		break;

	case JSENUMERATE_NEXT:
		it = (Entity::component_iterator*)JSVAL_TO_PRIVATE(*statep);
		if(*it != entity->end())
		{
			JS_ValueToId(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (*it)->first.c_str())), idp);
			(*it)++;
			return JS_TRUE;
		}
		// no break here, we want to fall through if we were at the end
	case JSENUMERATE_DESTROY:
		it = (Entity::component_iterator*)JSVAL_TO_PRIVATE(*statep);
		*statep = JSVAL_NULL;
		delete it;
	}

	return JS_TRUE;
}