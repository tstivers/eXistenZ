#pragma once

#include <NxPhysics.h>
#include <NxControllerManager.h>
#include "script/script.h"

void NxMat34ToD3DXMatrix(const NxMat34* in, D3DXMATRIX* out);
void D3DXMatrixToNxMat34(const D3DXMATRIX* in, NxMat34* out);

namespace physics
{
	
	typedef vector<shared_ptr<NxShapeDesc>> ShapeList;
	typedef shared_ptr<ShapeList> ShapeEntry;

	class PhysicsManager : public script::ScriptedObject
	{
	public:
		PhysicsManager(scene::Scene* scene);
		~PhysicsManager();

		scene::Scene* getScene() { return m_scene; }
		NxScene* getPhysicsScene() { return m_physicsScene; }
		NxControllerManager* getControllerManager() { return m_controllerManager; }
		NxPhysicsSDK* getPhysicsSDK() { return m_physicsSDK; }
		NxCookingInterface* getCookingInterface() { return m_cookingInterface; }

		void startSimulation();
		void getResults();
		void renderDebugView();

		// NxScene wrappers
		void setGravity(float value);
		void setTimestep(float value);
		void setMaxIter(float value);
		void setParameter(int parameter, float value);
		void setGroupCollisionFlag(int group1, int group2, bool enable);
		vector<component::Component*> getActorsInSphere(D3DXVECTOR3 origin, float radius);
		component::Component* getFirstActorInRay(D3DXVECTOR3 origin, D3DXVECTOR3 direction, float distance);

		// shapentry junk
		// TODO: make this cleaner
		ShapeEntry getShapeEntry(const string& name);

		// for js interface
		static ScriptClass m_scriptClass;

	protected:
		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();
		void initScriptObject();

		// shape cache
		typedef map<string, ShapeEntry> ShapeMap;
		ShapeMap m_shapeCache;

		scene::Scene* m_scene;
		NxPhysicsSDK* m_physicsSDK;
		NxScene* m_physicsScene;
		NxRemoteDebugger* m_debugger;
		NxCookingInterface* m_cookingInterface;
		NxControllerManager* m_controllerManager;
		const NxDebugRenderable* m_debugRenderable;		
	};	
}