#include "precompiled.h"
#include "game/jsplayer.h"
#include "game/player.h"
#include "script/jsvector.h"

namespace jsplayer
{

	JSBool posChanged(JSContext* cx, JSObject* obj, D3DXVECTOR3& new_vec, void* user);
	JSBool sizeChanged(JSContext* cx, JSObject* obj, D3DXVECTOR3& new_vec, void* user);
	JSBool getPos(JSContext *cx, uintN argc, jsval *vp);
	JSBool setPos(JSContext *cx, uintN argc, jsval *vp);
	JSBool getRot(JSContext *cx, uintN argc, jsval *vp);
	JSBool setRot(JSContext *cx, uintN argc, jsval *vp);
	JSBool setSpeed(JSContext* cx, JSObject* obj, jsid id, jsval *vp);
	JSBool setStepUp(JSContext* cx, JSObject* obj, jsid id, jsval *vp);
	JSBool setSize(JSContext* cx, JSObject* obj, jsid id, jsval *vp);
	JSBool setJumpHeight(JSContext* cx, JSObject* obj, jsid id, jsval *vp);
	JSBool setGravity(JSContext* cx, JSObject* obj, jsid id, jsval *vp);

	game::Player* getPlayerReserved(JSContext* cx, JSObject* obj);

	JSClass player_class =
	{
		"Player", JSCLASS_HAS_RESERVED_SLOTS(2),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	JSFunctionSpec player_methods[] =
	{
		{"getPos",	getPos,	0,0 },
		{"setPos",	setPos,	3,0 },
		{"getRot",	getRot,	0,0 },
		{"setRot",  setRot, 3,0 },
//		{"setSize", player_setSize, 3,0,0 },
		{0, 0, 0, 0}
	};

	jsvector::jsVectorOps posOps =
	{
		NULL, posChanged
	};
}

using namespace jsplayer;

JSBool jsplayer::createPlayerObject(JSContext* cx, JSObject* parent, const char* name, game::Player* player)
{
	JS_EnterLocalRootScope(cx);
	JSObject* pobj = JS_NewObject(cx, &player_class, NULL, parent);

	if (!pobj)
		goto error;

	if (!JS_SetReservedSlot(cx, pobj, 0, PRIVATE_TO_JSVAL(player)))
		goto error;

	if (!JS_DefineProperty(cx, parent, name, OBJECT_TO_JSVAL(pobj), NULL, NULL, JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE | JSPROP_ENUMERATE))
		goto error;
	JS_ForgetLocalRoot(cx, pobj);

	if (!JS_DefineFunctions(cx, pobj, player_methods))
		goto error;

	JSObject* size = jsvector::NewWrappedVector(cx, pobj, &player->size, false);
	JS_DefineProperty(cx, pobj, "size", OBJECT_TO_JSVAL(size), NULL, setSize, JSPROP_PERMANENT | JSPROP_ENUMERATE);
	JS_ForgetLocalRoot(cx, size);

	jsval speed;
	JS_NewNumberValue(cx, player->getSpeed(), &speed);
	JS_DefineProperty(cx, pobj, "speed", speed, NULL, setSpeed, JSPROP_PERMANENT | JSPROP_ENUMERATE);

	jsval step_up;
	JS_NewNumberValue(cx, player->getStepUp(), &step_up);
	JS_DefineProperty(cx, pobj, "step_up", step_up, NULL, setStepUp, JSPROP_PERMANENT | JSPROP_ENUMERATE);

	jsval jump_height;
	JS_NewNumberValue(cx, player->jump_height, &jump_height);
	JS_DefineProperty(cx, pobj, "jump_height", jump_height, NULL, setJumpHeight, JSPROP_PERMANENT | JSPROP_ENUMERATE);

	jsval gravity;
	JS_NewNumberValue(cx, player->gravity, &gravity);
	JS_DefineProperty(cx, pobj, "gravity", gravity, NULL, setGravity, JSPROP_PERMANENT | JSPROP_ENUMERATE);

	JS_LeaveLocalRootScope(cx);
	return JS_TRUE;

error:
	JS_LeaveLocalRootScope(cx);
	JS_ReportError(cx, "[jsplayer::createPlayerObject] error creating player object");
	return JS_FALSE;
}

JSBool jsplayer::posChanged(JSContext* cx, JSObject* obj, D3DXVECTOR3& new_vec, void* user)
{
	game::Player* player = (game::Player*)user;
	player->setPos(new_vec);
	return JS_TRUE;
}

JSBool jsplayer::sizeChanged(JSContext* cx, JSObject* obj, D3DXVECTOR3& new_vec, void* user)
{
	game::Player* player = (game::Player*)user;
	player->setSize(new_vec);
	return JS_TRUE;
}

JSBool jsplayer::getPos(JSContext *cx, uintN argc, jsval *vp)
{	
	game::Player* player = getPlayerReserved(cx, JS_THIS_OBJECT(cx,vp));
	if (!player)
		return JS_FALSE;

	JSObject* vec = jsvector::NewVector(cx, NULL, player->getPos());
	JS_RVAL(cx,vp) = OBJECT_TO_JSVAL(vec);

	return JS_TRUE;
}

JSBool jsplayer::setPos(JSContext *cx, uintN argc, jsval *vp)
{
	D3DXVECTOR3 vec;

	if (!jsvector::ParseVector(cx, vec, argc, JS_ARGV(cx,vp)))
		return JS_FALSE;

	game::Player* player = getPlayerReserved(cx, JS_THIS_OBJECT(cx,vp));
	if (!player)
		return JS_FALSE;

	player->setPos(vec);

	return JS_TRUE;
}

JSBool jsplayer::getRot(JSContext *cx, uintN argc, jsval *vp)
{
	game::Player* player = getPlayerReserved(cx, JS_THIS_OBJECT(cx,vp));
	if (!player)
		return JS_FALSE;

	JSObject* vec = jsvector::NewVector(cx, NULL, player->getRot());
	JS_RVAL(cx,vp) = OBJECT_TO_JSVAL(vec);

	return JS_TRUE;
}

JSBool jsplayer::setRot(JSContext *cx, uintN argc, jsval *vp)
{
	D3DXVECTOR3 vec;

	if (!jsvector::ParseVector(cx, vec, argc, JS_ARGV(cx,vp)))
		return JS_FALSE;

	game::Player* player = getPlayerReserved(cx, JS_THIS_OBJECT(cx,vp));
	if (!player)
		return JS_FALSE;

	player->setRot(vec);

	return JS_TRUE;
}

JSBool jsplayer::setSize(JSContext* cx, JSObject* obj, jsid id, jsval *vp)
{
	D3DXVECTOR3 vec;
	if (!jsvector::ParseVector(cx, vec, 1, vp))
		goto error;

	if (!JS_GetProperty(cx, obj, "size", vp))
		goto error;

	if (!getPlayerReserved(cx, obj)->setSize(vec))
		goto error;

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsplayer::setSize] error setting player size");
	return JS_FALSE;
}



JSBool jsplayer::setSpeed(JSContext* cx, JSObject* obj, jsid id, jsval *vp)
{
	jsdouble d;
	if (!JS_ValueToNumber(cx, *vp, &d))
		return JS_FALSE;

	getPlayerReserved(cx, obj)->setSpeed(d);

	return JS_TRUE;
}

JSBool jsplayer::setStepUp(JSContext* cx, JSObject* obj, jsid id, jsval *vp)
{
	jsdouble d;
	if (!JS_ValueToNumber(cx, *vp, &d))
		return JS_FALSE;

	getPlayerReserved(cx, obj)->setStepUp(d);

	return JS_TRUE;
}

game::Player* jsplayer::getPlayerReserved(JSContext* cx, JSObject* obj)
{
	jsval val;
	if (!JS_GetReservedSlot(cx, obj, 0, &val))
		return NULL;
	return (game::Player*)JSVAL_TO_PRIVATE(val);
}

JSBool jsplayer::setJumpHeight(JSContext* cx, JSObject* obj, jsid id, jsval *vp)
{
	jsdouble d;
	if (!JS_ValueToNumber(cx, *vp, &d))
		return JS_FALSE;

	getPlayerReserved(cx, obj)->jump_height = d;

	return JS_TRUE;
}

JSBool jsplayer::setGravity(JSContext* cx, JSObject* obj, jsid id, jsval *vp)
{
	jsdouble d;
	if (!JS_ValueToNumber(cx, *vp, &d))
		return JS_FALSE;

	getPlayerReserved(cx, obj)->gravity = d;

	return JS_TRUE;
}