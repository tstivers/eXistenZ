#include "precompiled.h"
#include "game/jsgame.h"
#include "game/game.h"
#include "script/script.h"
#include "window/appwindow.h"

namespace jsgame
{
	JSBool jsstartMap(JSContext *cx, uintN argc, jsval *vp);					  
	JSBool jsquit(JSContext *cx, uintN argc, jsval *vp);
	JSBool jstimestamp(JSContext *cx, uintN argc, jsval *vp);
}

REGISTER_STARTUP_FUNCTION(jsgame, jsgame::init, 10);

void jsgame::init()
{
	script::gScriptEngine->AddFunction("game.startMap", 1, jsgame::jsstartMap);
	script::gScriptEngine->AddFunction("quit", 0, jsgame::jsquit);
	script::gScriptEngine->AddFunction("timestamp", 0, jsgame::jstimestamp);
}

JSBool jsgame::jsstartMap(JSContext *cx, uintN argc, jsval *vp)
{
	if (argc != 1)
	{
		script::gScriptEngine->ReportError("startMap() takes 1 argument");
		return JS_FALSE;
	}

	game::startMap(JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx,vp)[0])));

	return JS_TRUE;
}

JSBool jsgame::jsquit(JSContext *cx, uintN argc, jsval *vp)
{
	PostMessage(eXistenZ::g_appWindow->getHwnd(), WM_QUIT, 0, 0);

	return JS_TRUE;
}

JSBool jsgame::jstimestamp(JSContext *cx, uintN argc, jsval *vp)
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

	JS_RVAL(cx,vp) = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, timestamp));
	return JS_TRUE;
}
