/////////////////////////////////////////////////////////////////////////////
// console.cpp
// console class
// $Id$
//

#include "precompiled.h"
#include "game/jsgame.h"
#include "game/game.h"
#include "script/script.h"
#include "console/console.h"
#include "client/appwindow.h"

extern ScriptEngine gScriptEngine;

namespace jsgame {
	JSBool jsstartMap(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);
	JSBool jsquit(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);
}

void jsgame::init()
{
	gScriptEngine.AddFunction("game.startMap", 1, jsgame::jsstartMap);
	gScriptEngine.AddFunction("quit", 0, jsgame::jsquit);
}

JSBool jsgame::jsstartMap(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval)
{
	if(argc != 1) {
		gScriptEngine.ReportError("startMap() takes 1 argument");
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
