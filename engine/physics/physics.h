#pragma once

#include <NxPhysics.h>

namespace scene
{
	class SceneBSP;
}

void NxMat34ToD3DXMatrix(const NxMat34* in, D3DXMATRIX* out);
void D3DXMatrixToNxMat34(const D3DXMATRIX* in, NxMat34* out);

namespace physics
{
	void init();
	void acquire();
	void release();
	void destroy();
	void startSimulation();
	void getResults();
	void addStaticMesh(string name, scene::SceneBSP* scene);
	void setParameter(int parameter, float value);
	void renderDebug();

	typedef vector<shared_ptr<NxShapeDesc>> ShapeList;
	typedef shared_ptr<ShapeList> ShapeEntry;

	ShapeEntry getShapeEntry(const string& name);

	extern NxPhysicsSDK* gPhysicsSDK;
	extern NxScene* gScene;
	extern int debugRender;
}