#include "precompiled.h"
#include "scene/jsscene.h"
#include "scene/scene.h"
#include "script/script.h"
#include "console/console.h"
#include "entity/jsentity.h"
#include "entity/entity.h"
#include "render/render.h"

namespace jsscene {
	JSBool addEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);	
	//JSBool updateEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	//JSBool removeEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
};

using namespace jsscene;
using namespace scene;

void jsscene::init()
{
	gScriptEngine->AddFunction("system.scene.addEntity", 1, jsscene::addEntity);	
}

JSBool jsscene::addEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	if(!render::scene) {
		gScriptEngine->ReportError("addEntity(): no scene loaded");
		return JS_FALSE;
	}

	if(argc != 1) {
		gScriptEngine->ReportError("usage: addEntity(Entity)");
		return JS_FALSE;
	}

	if(!JSVAL_IS_OBJECT(argv[0])) {
		gScriptEngine->ReportError("addEntity(): argument wasn't an object");
		return JS_FALSE;
	}

	JSObject* entity_obj = JSVAL_TO_OBJECT(argv[0]);
	if(JS_InstanceOf(cx, entity_obj, &jsentity::JSEntity, NULL) == JS_FALSE) {
		gScriptEngine->ReportError("addEntity(): argument wasn't an entity object");
		return JS_FALSE;
	}

	jsval entity_val;
	JS_GetReservedSlot(cx, entity_obj, 0, &entity_val);
	entity::Entity* entity = (entity::Entity*)JSVAL_TO_PRIVATE(entity_val);
	render::scene->addEntity(entity);

	return JS_TRUE;
}