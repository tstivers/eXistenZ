#include "precompiled.h"
#include "physics/jsphysics.h"
#include "physics/physics.h"
#include "script/script.h"
#include "physics/shapecache.h"
#include "component/actorcomponent.h"


namespace jsphysics
{
	void init();
	JSBool loadDynamicsXML(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool setParameter(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool setGroupCollisionFlag(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool getShapesFromSphere(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
}

using namespace jsphysics;

REGISTER_STARTUP_FUNCTION(jsphysics, jsphysics::init, 10);

void jsphysics::init()
{
	script::gScriptEngine->AddFunction("loadDynamicsXML", 1, loadDynamicsXML);
	script::gScriptEngine->AddFunction("system.physics.setParameter", 2, setParameter);
	script::gScriptEngine->AddFunction("system.physics.setGroupCollisionFlag", 3, setGroupCollisionFlag);
	script::gScriptEngine->AddFunction("system.physics.getEntitiesInSphere", 2, getShapesFromSphere);

}

JSBool jsphysics::loadDynamicsXML(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	string filename =  JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	NxActor* actor;
	physics::getShapeEntry(filename);

	return JS_TRUE;
}

JSBool jsphysics::setParameter(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int parameter = JSVAL_TO_INT(argv[0]);
	jsdouble value;
	JS_ValueToNumber(cx, argv[1], &value);

	physics::setParameter(parameter, value);

	return JS_TRUE;
}

JSBool jsphysics::setGroupCollisionFlag(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int group1, group2;
	bool enable;
	jsscript::jsval_to_(cx, argv[0], &group1);
	jsscript::jsval_to_(cx, argv[1], &group2);
	jsscript::jsval_to_(cx, argv[2], &enable);

	physics::gScene->setGroupCollisionFlag(group1, group2, enable);

	return JS_TRUE;
}

JSBool jsphysics::getShapesFromSphere(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	D3DXVECTOR3 origin;
	float radius;

	jsscript::jsval_to_(cx, argv[0], &origin);
	jsscript::jsval_to_(cx, argv[1], &radius);

	NxShape* shapes[1000];
	NxShape** shapeptr = shapes;

	NxSphere bleh;
	bleh.center = (NxVec3)origin;
	bleh.radius = radius;

	int nbshapes = physics::gScene->overlapSphereShapes(bleh, NX_DYNAMIC_SHAPES, 1000, shapeptr, NULL);

	JSObject* ret = JS_NewObject(cx, NULL, NULL, NULL);
	*rval = OBJECT_TO_JSVAL(ret);
	
	int index = 0;
	for(int i = 0; i < nbshapes; i++)
	{
		component::ActorComponent* actor;
		component::Component* c = (component::Component*)shapes[i]->getActor().userData;
		if(c && (actor = dynamic_cast<component::ActorComponent*>(c)))
		{
			jsval v = OBJECT_TO_JSVAL(actor->getScriptObject());
			JS_SetElement(cx, ret, index++, &v);
		}
	}

	return JS_TRUE;
}