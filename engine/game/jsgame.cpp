#include "precompiled.h"
#include "game/jsgame.h"
#include "game/game.h"
#include "script/script.h"
#include "client/appwindow.h"

namespace jsgame {
	JSBool jsstartMap(JSContext *cx, JSObject *obj, uintN argc,
		jsval *argv, jsval *rval);
	JSBool jsquit(JSContext *cx, JSObject *obj, uintN argc,
		jsval *argv, jsval *rval);
	JSBool jstimestamp(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
}

REGISTER_STARTUP_FUNCTION(jsgame, jsgame::init, 10);

void jsgame::init()
{
	gScriptEngine->AddFunction("game.startMap", 1, jsgame::jsstartMap);
	gScriptEngine->AddFunction("quit", 0, jsgame::jsquit);
	gScriptEngine->AddFunction("timestamp", 0, jsgame::jstimestamp);
}

JSBool jsgame::jsstartMap(JSContext *cx, JSObject *obj, uintN argc,
						  jsval *argv, jsval *rval)
{
	if(argc != 1) {
		gScriptEngine->ReportError("startMap() takes 1 argument");
		return JS_FALSE;	
	}

	game::startMap(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));

	return JS_TRUE;
}

JSBool jsgame::jsquit(JSContext *cx, JSObject *obj, uintN argc,
					  jsval *argv, jsval *rval)
{
	PostMessage(appwindow::getHwnd(), WM_QUIT, 0, 0);

	return JS_TRUE;
}

JSBool jsgame::jstimestamp(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	SYSTEMTIME now;
	GetLocalTime(&now);
	char timestamp[64];
	sprintf(timestamp, "%04d-%02d-%02d-%02d-%02d-%02d-%04d",
		now.wYear,
		now.wMonth,
		now.wDay,
		now.wHour,
		now.wMinute,
		now.wSecond,
		now.wMilliseconds);

	*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, timestamp));
	return JS_TRUE;
}
