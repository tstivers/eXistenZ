#include "precompiled.h"
#include "script/script.h"
#include "script/jsscript.h"
#include "vfs/vfs.h"
#include "vfs/file.h"

#pragma warning( disable : 4311 4312 )

REGISTER_STARTUP_FUNCTION(jsscript, jsscript::init, 10);

namespace jsscript
{
	JSBool jsexecfile(JSContext *cx, uintN argc, jsval *vp);
	JSBool jsdumpobject(JSContext *cx, uintN argc, jsval *vp);
	JSBool jssetzeal(JSContext *cx, uintN argc, jsval *vp);
	JSBool jsclassof(JSContext *cx, uintN argc, jsval *vp);
	JSBool jsparentof(JSContext *cx, uintN argc, jsval *vp);
}

void jsscript::init()
{
	script::gScriptEngine->AddFunction("execfile", 1, jsscript::jsexecfile);
	script::gScriptEngine->AddFunction("dumpobject", 1, jsscript::jsdumpobject);
	script::gScriptEngine->AddFunction("setzeal", 1, jsscript::jssetzeal);
	script::gScriptEngine->AddFunction("classof", 1, jsscript::jsclassof);
	script::gScriptEngine->AddFunction("parentof", 1, jsscript::jsparentof);
}

JSBool jsscript::jsexecfile(JSContext *cx, uintN argc, jsval *vp)
{
	if (argc != 1)
	{
		script::gScriptEngine->ReportError("execfile() takes 1 argument");
		return JS_FALSE;
	}

	vfs::File file = vfs::getFile(JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx,vp)[0])));
	if (file)
	{
		script::gScriptEngine->RunScript(file);
	}
	else
	{
		script::gScriptEngine->ReportError("execfile(): unable to open %s", JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx,vp)[0])));
		return JS_FALSE;
	}

	return JS_TRUE;
}

JSBool jsscript::jsdumpobject(JSContext *cx, uintN argc, jsval *vp)
{
	if (argc != 1)
	{
		script::gScriptEngine->ReportError("dumpobject(): takes 1 argument");
		return JS_FALSE;
	}

	if (!JSVAL_IS_OBJECT(JS_ARGV(cx,vp)[0]))
	{
		script::gScriptEngine->ReportError("dumpobject(): argument must be an object");
		return JS_FALSE;
	}

	script::gScriptEngine->DumpObject(JSVAL_TO_OBJECT(JS_ARGV(cx,vp)[0]));
	return JS_TRUE;
}

JSBool jsscript::jssetzeal(JSContext *cx, uintN argc, jsval *vp)
{
	if (argc != 1)
	{
		script::gScriptEngine->ReportError("setzeal(): takes 1 argument");
		return JS_FALSE;
	}

	if (!JSVAL_IS_NUMBER(JS_ARGV(cx,vp)[0]))
	{
		script::gScriptEngine->ReportError("setzeal(): argument must be a number");
		return JS_FALSE;
	}

	int32 zeal;
	JS_ValueToECMAInt32(cx, JS_ARGV(cx,vp)[0], &zeal);

#ifdef DEBUG
	JS_SetGCZeal(cx, zeal);
#endif

	return JS_TRUE;
}

JSBool jsscript::jsclassof(JSContext *cx, uintN argc, jsval *vp)
{
	if (argc != 1)
	{
		script::gScriptEngine->ReportError("classof(): takes 1 argument");
		return JS_FALSE;
	}

	if (!JSVAL_IS_OBJECT(JS_ARGV(cx,vp)[0]))
	{
		JS_RVAL(cx,vp) = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "none"));
	}
	else
		JS_RVAL(cx,vp) = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, script::gScriptEngine->GetClassName(JSVAL_TO_OBJECT(JS_ARGV(cx,vp)[0]))));

	return JS_TRUE;
}

JSBool jsscript::jsparentof(JSContext *cx, uintN argc, jsval *vp)
{
	if (argc != 1)
	{
		script::gScriptEngine->ReportError("parentof(): takes 1 argument");
		return JS_FALSE;
	}

	if (!JSVAL_IS_OBJECT(JS_ARGV(cx,vp)[0]))
		return JS_FALSE;

	JS_RVAL(cx,vp) = OBJECT_TO_JSVAL(JS_GetParent(cx, JSVAL_TO_OBJECT(JS_ARGV(cx,vp)[0])));

	return JS_TRUE;
}
