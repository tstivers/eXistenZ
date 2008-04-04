#include "precompiled.h"
#include "script/script.h"
#include "script/jsscript.h"
#include "vfs/vfs.h"
#include "vfs/file.h"

#pragma warning( disable : 4311 4312 )

REGISTER_STARTUP_FUNCTION(jsscript, jsscript::init, 10);

namespace jsscript
{
JSBool jssetzeal(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
}

void jsscript::init()
{
	gScriptEngine->AddFunction("execfile", 1, jsscript::jsexecfile);
	gScriptEngine->AddFunction("dumpobject", 1, jsscript::jsdumpobject);
	gScriptEngine->AddFunction("setzeal", 1, jsscript::jssetzeal);
}

JSBool jsscript::jsexecfile(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval)
{
	if(argc != 1) {
		gScriptEngine->ReportError("execfile() takes 1 argument");
		return JS_FALSE;	
	}

	vfs::IFilePtr file = vfs::getFile(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
	if(file){
		gScriptEngine->RunScript(file);		
	} else {
		gScriptEngine->ReportError("execfile(): unable to open %s", JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
		return JS_FALSE;
	}
	
	return JS_TRUE;
}

JSBool jsscript::jsdumpobject(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval)
{
	if(argc != 1) {
		gScriptEngine->ReportError("dumpobject(): takes 1 argument");
		return JS_FALSE;	
	}

	if(!JSVAL_IS_OBJECT(argv[0])) {
		gScriptEngine->ReportError("dumpobject(): argument must be an object");
		return JS_FALSE;	
	}

	gScriptEngine->DumpObject(JSVAL_TO_OBJECT(argv[0]));
	return JS_TRUE;
}

JSBool jsscript::jssetzeal(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval)
{
	if(argc != 1) {
		gScriptEngine->ReportError("setzeal(): takes 1 argument");
		return JS_FALSE;	
	}

	if(!JSVAL_IS_NUMBER(argv[0])) {
		gScriptEngine->ReportError("setzeal(): argument must be a number");
		return JS_FALSE;	
	}

	int32 zeal;
	JS_ValueToECMAInt32(cx, argv[0], &zeal);

#ifdef DEBUG
	JS_SetGCZeal(cx, zeal);
#endif

	return JS_TRUE;
}
