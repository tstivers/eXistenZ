/////////////////////////////////////////////////////////////////////////////
// render.cpp
// rendering system implementation
// $Id$
//

#include "precompiled.h"
#include "render/render.h"
#include "render/scenegraph.h"

namespace render
{
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
	tree->acquire();
}

void SceneGraph::release()
{
}

void SceneGraph::render()
{
	if (render::use_scenegraph)
		tree->render();
}

void SceneGraph::finalizeStatic()
{
	tree->subdivide();
}