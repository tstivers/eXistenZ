/////////////////////////////////////////////////////////////////////////////
// script.cpp
// script engine class
// $Id$
//

#include "precompiled.h"
#include "script/script.h"
#include "script/jsscript.h"
#include "vfs/vfs.h"
#include "vfs/file.h"

#pragma warning( disable : 4311 4312 )

void jsscript::init()
{
	gScriptEngine.AddFunction("execfile", 1, jsscript::jsexecfile);
	gScriptEngine.AddFunction("dumpobject", 1, jsscript::jsdumpobject);
}

JSBool jsscript::jsexecfile(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval)
{
	if(argc != 1) {
		gScriptEngine.ReportError("execfile() takes 1 argument");
		return BOOLEAN_TO_JSVAL(FALSE);	
	}

	vfs::IFilePtr file = vfs::getFile(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
	if(file){
		gScriptEngine.RunScript(file);		
	} else {
		gScriptEngine.ReportError("execfile(): unable to open %s", JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
		return BOOLEAN_TO_JSVAL(FALSE);
	}
	
	return BOOLEAN_TO_JSVAL(TRUE);
}

JSBool jsscript::jsdumpobject(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval)
{
	if(argc != 1) {
		gScriptEngine.ReportError("dumpobject(): takes 1 argument");
		return BOOLEAN_TO_JSVAL(FALSE);	
	}

	if(!JSVAL_IS_OBJECT(argv[0])) {
		gScriptEngine.ReportError("dumpobject(): argument must be an object");
		return BOOLEAN_TO_JSVAL(FALSE);	
	}

	gScriptEngine.DumpObject(JSVAL_TO_OBJECT(argv[0]));
	return BOOLEAN_TO_JSVAL(TRUE);
}
