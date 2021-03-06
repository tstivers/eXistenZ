#pragma once

namespace script
{
	class ScriptedObject
	{
	public:

		struct ScriptClass
		{
			JSClass* classDef;
			JSPropertySpec* properties;
			JSFunctionSpec* methods;
			JSObject* prototype;
		};

		ScriptedObject() : m_scriptObject(NULL) 
		{
		}

		virtual ~ScriptedObject() 
		{
			ASSERT(!m_scriptObject);
		}

		virtual JSObject* getScriptObject() 
		{ 
			if(!m_scriptObject)
				m_scriptObject = createScriptObject();
			return m_scriptObject;
		}

	protected:
		virtual JSObject* createScriptObject() { return NULL; }
		virtual void destroyScriptObject() = 0;
		JSObject* m_scriptObject;
	};

	template<typename T, typename U>
	JSObject* RegisterScriptClass(ScriptEngine* engine)
	{
		T::m_scriptClass.prototype = JS_InitClass(
			engine->GetContext(),
			engine->GetGlobal(),
			U::m_scriptClass.prototype,
			T::m_scriptClass.classDef,
			NULL,
			0,
			T::m_scriptClass.properties,
			T::m_scriptClass.methods,
			NULL,
			NULL);

		ASSERT(T::m_scriptClass.prototype);
		return T::m_scriptClass.prototype;
	}

	template<typename T>
	T* GetReserved(JSContext* cx, JSObject* obj, int index = 0)
	{
		jsval val = JSVAL_VOID;
		JSBool ret = JS_GetReservedSlot(cx, obj, index, &val);
		ASSERT(ret == JS_TRUE);
		ASSERT(val != JSVAL_VOID);
		ASSERT(JSVAL_TO_PRIVATE(val) != NULL);
		return (T*)JSVAL_TO_PRIVATE(val);
	}

	template<typename T>
	bool GetProperty(JSContext* cx, JSObject* obj, const char* name, T& value)
	{
		jsval v;
		if(JS_GetProperty(cx, obj, name, &v) && v != JSVAL_VOID)
			return jsscript::jsval_to_(cx, v, &value);

		return false;
	}

	template<typename T>
	JSBool NameGetter(JSContext *cx, JSObject *obj, jsid id, jsval *vp)
	{
		T* e = GetReserved<T>(cx, obj);
		*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, e->getName().c_str()));
		return JS_TRUE;
	}

	template<typename T, const std::string& (T::* prop)(void)>
	JSBool StringGetter(JSContext *cx, JSObject *obj, jsid id, jsval *vp)
	{
		T* e = GetReserved<T>(cx, obj);
		*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (e->*prop)().c_str()));
		return JS_TRUE;
	}

	template<typename T, typename U, U (T::* prop)(void)>
	JSBool PropertyGetter(JSContext *cx, JSObject *obj, jsid id, jsval *vp)
	{
		T* e = GetReserved<T>(cx, obj);
		*vp = jsscript::to_jsval(cx, (e->*prop)());
		return JS_TRUE;
	}

	template<typename T, typename U, void (T::* prop)(U)>
	JSBool PropertySetter(JSContext *cx, JSObject *obj, jsid id, jsval *vp)
	{
		T* e = GetReserved<T>(cx, obj);
		boost::remove_const<boost::remove_reference<U>::type>::type val;
		if(jsscript::jsval_to_(cx, *vp, &val))
		{
			(e->*prop)(val);
			return JS_TRUE;
		}

		JS_ReportError(cx, "argument incorrect for property");
		return JS_FALSE;
	}
}