/////////////////////////////////////////////////////////////////////////////
// script.cpp
// script engine class
// $Id: jsscript.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#include "precompiled.h"
#include "script/script.h"
#include "script/jsscript.h"
#include "vfs/vfs.h"
#include "vfs/file.h"

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

	VFile* file = vfs::getFile(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
	if(file){
		gScriptEngine.RunScript(file);
		file->close();
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
