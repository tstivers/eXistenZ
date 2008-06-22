#pragma once

#include "component/component.h"

namespace jscomponent
{
	template<typename T>
	JSObject* createComponentScriptObject(T* component)
	{
		JSContext* cx = script::gScriptEngine->GetContext();

		JS_EnterLocalRootScope(cx);
		JSObject* entity = component->getEntity()->getScriptObject();
		jsval cval = JSVAL_VOID;
		JS_GetProperty(cx, entity, "components", &cval);
		JSObject* components = JSVAL_TO_OBJECT(cval);
		JSObject* obj = JS_DefineObject(
			cx, 
			components, 
			component->getName().c_str(), 
			T::m_scriptClass.classDef, 
			T::m_scriptClass.prototype, 
			JSPROP_ENUMERATE | JSPROP_READONLY);
		JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(component));
		JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
		return obj;
	}

	template<typename T>
	void destroyComponentScriptObject(T* component)
	{
		JSObject* components;
		script::GetProperty(script::gScriptEngine->GetContext(), component->getEntity()->getScriptObject(), "components", components);
		JS_SetReservedSlot(script::gScriptEngine->GetContext(), component->getScriptObject(), 0, PRIVATE_TO_JSVAL(NULL));
		JS_DeleteProperty(
			script::gScriptEngine->GetContext(), 
			components, 
			component->getName().c_str());
	}

	template<typename T>
	JSBool createComponent(JSContext *cx, uintN argc, jsval *vp)
	{
		entity::Entity* e = GetReserved<entity::Entity>(cx, JS_THIS_OBJECT(cx, vp));

		string name;
		jsscript::jsval_to_(cx, JS_ARGV(cx, vp)[0], &name);
		T::desc_type desc;
		if(argc == 2 && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[1]))
			parseDesc(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[1]), desc);
		T* component = e->createComponent(name, desc, &component);
		if(component)
			JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(component->getScriptObject()));
		else
			JS_SET_RVAL(cx, vp, JSVAL_VOID);

		return JS_TRUE;
	}

	#define WRAPPED_LINK(name, source, target) {#name, 0, JSPROP_PERMANENT | JSPROP_SHARED, linkGetter<source, target, & ## source ## :: ## name>, linkSetter<source, target, & ## source ## :: ## name>}

	template<typename C, typename T, component::ComponentLink<T> C::* link>
	JSBool linkGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
	{
		C* c = GetReserved<C>(cx, obj);
		T* component = c->*link;

		if(component)
		{
			*vp = OBJECT_TO_JSVAL(component->getScriptObject());
			return JS_TRUE;
		}

		*vp = JSVAL_VOID;
		return JS_TRUE;
	}

	template<typename C, typename T, component::ComponentLink<T> C::* link>
	JSBool linkSetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
	{
		C* c = GetReserved<C>(cx, obj);
		if(JSVAL_IS_OBJECT(*vp))
		{
			if(*vp == JSVAL_NULL) // component.link = null;
			{
				c->*link = NULL;
				return JS_TRUE;
			}

			JSObject* linkobj = JSVAL_TO_OBJECT(*vp);
			
			if(!dynamic_cast<T*>(GetReserved<Component>(cx, linkobj)))
			{
				INFO("ERROR: tried to set link to invalid component type on %s.%s",
					c->getEntity()->getName().c_str(), c->getName().c_str());
				return JS_FALSE;
			}

			c->*link = GetReserved<T>(cx, linkobj);
			return JS_TRUE;
		}
		else if(JSVAL_IS_STRING(*vp))
		{
			string name;
			jsscript::jsval_to_(cx, *vp, &name);
			
			JSObject* linkobj = JSVAL_TO_OBJECT(*vp);
			
			if(!dynamic_cast<T*>(c->getEntity()->getComponent(name)))
			{
				INFO("ERROR: tried to set link to invalid component type on %s.%s",
					c->getEntity()->getName().c_str(), c->getName().c_str());
				return JS_FALSE;
			}

			c->*link = name;
			return JS_TRUE;
		}

		JS_ReportError(cx, "ERROR: unable to parse argument for setting link on %s.%s",
			c->getEntity()->getName().c_str(), c->getName().c_str());
		return JS_FALSE;
	}
}