#include "precompiled.h"
#include "game/jsplayer.h"
#include "game/player.h"
#include "script/jsvector.h"

namespace jsplayer {

	JSBool posChanged(JSContext* cx, JSObject* obj, D3DXVECTOR3& new_vec, void* user);
	JSBool setPos(JSContext* cx, JSObject* obj, jsval id, jsval *vp);
	JSBool setSpeed(JSContext* cx, JSObject* obj, jsval id, jsval *vp);

	JSClass player_class = {
		"Player", JSCLASS_HAS_RESERVED_SLOTS(2),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	JSFunctionSpec player_methods[] = { 
//		{"setPos",	player_setPos,	3,0,0 },
//		{"setRot",  player_setRot,  3,0,0 },
//		{"setSize", player_setSize, 3,0,0 },
		{0,0,0,0,0}
	};

	jsvector::VectorOps posOps = {
		NULL, posChanged
	};
}

using namespace jsplayer;

JSBool jsplayer::createPlayerObject(JSContext* cx, JSObject* parent, const char* name, game::Player* player)
{
	JS_EnterLocalRootScope(cx);
	JSObject* pobj = JS_NewObject(cx, &player_class, NULL, parent);
	
	if(!pobj)
		goto error;

	if(!JS_SetReservedSlot(cx, pobj, 0, PRIVATE_TO_JSVAL(player)))
		goto error;

	if(!JS_DefineProperty(cx, parent, name, OBJECT_TO_JSVAL(pobj), NULL, NULL, JSPROP_READONLY | JSPROP_PERMANENT))
		goto error;
	JS_ForgetLocalRoot(cx, pobj);

	JSObject* pos = jsvector::NewWrappedVector(cx, pobj, &player->pos, false, &posOps, player);
	JS_DefineProperty(cx, pobj, "pos", OBJECT_TO_JSVAL(pos), NULL, setPos, JSPROP_PERMANENT);
	JS_ForgetLocalRoot(cx, pos);

	JSObject* rot = jsvector::NewWrappedVector(cx, pobj, &player->rot, false);
	JS_DefineProperty(cx, pobj, "rot", OBJECT_TO_JSVAL(rot), NULL, NULL, JSPROP_PERMANENT);
	JS_ForgetLocalRoot(cx, rot);
	
	jsval speed;
	JS_NewNumberValue(cx, player->getSpeed(), &speed);
	JS_DefineProperty(cx, pobj, "speed", speed, NULL, setSpeed, JSPROP_PERMANENT);

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

JSBool jsplayer::setPos(JSContext* cx, JSObject* obj, jsval id, jsval *vp)
{
	if(!JSVAL_IS_OBJECT(*vp))
		goto error;

	D3DXVECTOR3 pos;
	if(!jsvector::GetVector(cx, JSVAL_TO_OBJECT(*vp), pos))
		goto error;
	
	if(!JS_GetProperty(cx, obj, "pos", vp))
		goto error;

	if(!jsvector::SetVector(cx, JSVAL_TO_OBJECT(*vp), pos))
		goto error;

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsplayer::setPos] error setting player position");
	return JS_FALSE;
}

JSBool jsplayer::setSpeed(JSContext* cx, JSObject* obj, jsval id, jsval *vp)
{
	jsdouble d;
	if(!JS_ValueToNumber(cx, *vp, &d))
		return JS_FALSE;

	jsval val;
	JS_GetReservedSlot(cx, obj, 0, &val);
	game::Player* player = (game::Player*)JSVAL_TO_PRIVATE(val);
	player->setSpeed(d);

	return JS_TRUE;
}