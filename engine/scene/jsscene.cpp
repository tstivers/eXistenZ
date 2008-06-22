#include "precompiled.h"
#include "scene/jsscene.h"
#include "scene/scene.h"
#include "script/script.h"
#include "entity/jsentity.h"
#include "entity/entity.h"
#include "render/render.h"

namespace jsscene
{
	JSBool addEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	//JSBool updateEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool removeEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
};

using namespace jsscene;
using namespace scene;

REGISTER_STARTUP_FUNCTION(jsscene, jsscene::init, 10);

void jsscene::init()
{
	script::gScriptEngine->AddFunction("system.scene.addEntity", 1, jsscene::addEntity);
	script::gScriptEngine->AddFunction("system.scene.removeEntity", 1, jsscene::removeEntity);
}

JSBool jsscene::addEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	if (!render::scene)
	{
		script::gScriptEngine->ReportError("addEntity(): no scene loaded");
		return JS_FALSE;
	}

	if (argc != 1)
	{
		script::gScriptEngine->ReportError("usage: addEntity(Entity)");
		return JS_FALSE;
	}

	if (!JSVAL_IS_OBJECT(argv[0]))
	{
		script::gScriptEngine->ReportError("addEntity(): argument wasn't an object");
		return JS_FALSE;
	}

	JSObject* entity_obj = JSVAL_TO_OBJECT(argv[0]);
	//if (JS_InstanceOf(cx, entity_obj, &jsentity::entity_class, NULL) == JS_FALSE)
	//{
	//	script::gScriptEngine->ReportError("addEntity(): argument wasn't an entity object");
	//	return JS_FALSE;
	//}

	jsval entity_val;
	JS_GetReservedSlot(cx, entity_obj, 0, &entity_val);
	entity::Entity* entity = (entity::Entity*)JSVAL_TO_PRIVATE(entity_val);
	render::scene->addEntity(entity);

	return JS_TRUE;
}

JSBool jsscene::removeEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	if (!render::scene)
	{
		script::gScriptEngine->ReportError("addEntity(): no scene loaded");
		return JS_FALSE;
	}

	if (argc != 1)
	{
		script::gScriptEngine->ReportError("usage: addEntity(Entity)");
		return JS_FALSE;
	}

	if (!JSVAL_IS_OBJECT(argv[0]))
	{
		script::gScriptEngine->ReportError("addEntity(): argument wasn't an object");
		return JS_FALSE;
	}

	JSObject* entity_obj = JSVAL_TO_OBJECT(argv[0]);
	//if (JS_InstanceOf(cx, entity_obj, &jsentity::entity_class, NULL) == JS_FALSE)
	//{
	//	script::gScriptEngine->ReportError("addEntity(): argument wasn't an entity object");
	//	return JS_FALSE;
	//}

	jsval entity_val;
	JS_GetReservedSlot(cx, entity_obj, 0, &entity_val);
	entity::Entity* entity = (entity::Entity*)JSVAL_TO_PRIVATE(entity_val);
	render::scene->removeEntity(entity);
	//entity::removeEntity(entity);
	JS_ClearScope(cx, entity_obj);
	JS_SetReservedSlot(cx, entity_obj, 0, JSVAL_NULL);

	return JS_TRUE;
}