#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <maya/MFnDagNode.h>
#include <maya/MItDag.h>
#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MPxCommand.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MFnCamera.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnLight.h>
#include <maya/MColor.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MIOStream.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MItSelectionList.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFnMesh.h>
#include <maya/M3dView.h>
#include "d3d9.h"
#include "d3dx9core.h"
#include <list>
#include <map>
#include <vector>
#include <string>
#include <CommCtrl.h>

#define DLL_EXPORT __declspec(dllexport)