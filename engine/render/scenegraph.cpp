/////////////////////////////////////////////////////////////////////////////
// render.cpp
// rendering system implementation
// $Id: scenegraph.cpp,v 1.1 2003/11/24 00:16:13 tstivers Exp $
//

#include "precompiled.h"
#include "render/render.h"
#include "render/scenegraph.h"

namespace render {
}

using namespace render;

SceneGraph::SceneGraph()
{
	tree = new SceneNode(NULL);
}

SceneGraph::~SceneGraph()
{
	delete tree;
}

MESHHANDLE SceneGraph::addStaticMesh(Mesh& mesh)
{
	meshes.push_back(&mesh);
	tree->addStaticMesh(mesh);

	return (MESHHANDLE)meshes.size();
}

MESHHANDLE SceneGraph::addDynamicMesh(Mesh& mesh)
{
	return (MESHHANDLE)meshes.size();
}

void SceneGraph::acquire()
{
}

void SceneGraph::release()
{
}

void SceneGraph::render()
{
}

void SceneGraph::finalizeStatic()
{
	tree->subdivide();
}