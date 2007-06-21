#pragma once

#include "vfs/vfs.h"

namespace script {
	
	enum PROP_TYPE {
		PROP_TYPE_INT,
		PROP_TYPE_FLOAT,
		PROP_TYPE_DOUBLE,
		PROP_TYPE_BOOL,
		PROP_TYPE_STRING,
		PROP_TYPE_VECTOR
	};

	struct Property {
		Property(const char* name, PROP_TYPE type, void* data, GetterOp getter, SetterOp setter);
		~Property();
		
		char* name;
		PROP_TYPE type;
		void* data;
		GetterOp getter;
		SetterOp setter;
	};		
	
	typedef std::smart_ptr<Property> PropertyPtr;
	typedef stdext::hash_map<char*, PropertyPtr, hash_char_ptr_traits> PropertyMap;	
	typedef std::smart_ptr<PropertyMap> PropertyMapPtr;
	typedef stdext::hash_map<JSObject*, PropertyMapPtr> ObjectMap;

	class ScriptEngine {
	private:
		JSRuntime *rt;
		JSContext *cx;
		JSObject *globalObj;
		JSErrorReporter reporter;
		ObjectMap object_map;
	
	public:
		ScriptEngine();
		~ScriptEngine();

		JSObject* getGlobal() { return globalObj; }
		JSContext* getContext() { return cx; }
			
		JSErrorReporter setErrorReporter(JSErrorReporter reporter);	
		void reportError(char* format, ...);
	
		bool runScript(char* script);
		bool runScript(char* script, jsval* retval);
		bool runScript(char* name, uintN lineno, char* script);
		bool runScript(char* name, uintN lineno, char* script, jsval* retval);
		bool runScript(vfs::IFilePtr file);
	
		bool addFunction(const char* name, uintN argc, JSNative call);
		bool delFunction(const char* name);
		bool callFunction(const char* name, JSArgumentList& args, jsval* rval = NULL);
		
		bool addProperty(JSObject* parent, const char* name, jsval value, JSPropertyOp getter, JSPropertyOp setter, uintN flags);
		bool addProperty(const char* name, int* var, bool readonly = false, GetterOp getter = NULL, SetterOp setter = NULL);
		bool addProperty(const char* name, float* var, bool readonly = false, GetterOp getter = NULL, SetterOp setter = NULL);
		bool addProperty(const char* name, double* var, bool readonly = false, GetterOp getter = NULL, SetterOp setter = NULL);
		bool addProperty(const char* name, bool* var, bool readonly = false, GetterOp getter = NULL, SetterOp setter = NULL);
		bool addProperty(const char* name, std::string* var, bool readonly = false, GetterOp getter = NULL, SetterOp setter = NULL);
		
		bool getProperty(const char* name, int* data);
		bool getProperty(const char* name, float* data);
		bool getProperty(const char* name, double* data);
		bool getProperty(const char* name, bool* data);
		bool getProperty(const char* name, std::string* data);
	
		bool setProperty(const char* name, int data);
		bool setProperty(const char* name, float data);
		bool setProperty(const char* name, double data);
		bool setProperty(const char* name, bool data);
		bool setProperty(const char* name, const char* data);
		
		bool delProperty(const char* name);
			
		JSObject* addObject(const char* name, JSObject* parent);
		JSObject* getObject(const char* name, bool create = false);
	};
}

// hack, replace with getScriptEngine()
extern script::ScriptEngine* gScriptEngine;