/////////////////////////////////////////////////////////////////////////////
// render.cpp
// rendering system implementation
// $Id: scenenode.cpp,v 1.1 2003/11/24 00:16:13 tstivers Exp $
//

#include "precompiled.h"
#include "render/render.h"
#include "render/scenegraph.h"
#include "console/console.h"
#include "q3bsp/bleh.h"
#include "render/aabb.h"

namespace render {
	enum {
		BOUNDS_NOINTERSECT = 0,
		BOUNDS_INTERSECT,
		BOUNDS_CONTAINED,
		BOUNDS_CONTAINER
	};

	inline int boundsCheck(const D3DXVECTOR3* box1, const D3DXVECTOR3* box2);
	inline bool pointInBox(const float* p, const D3DXVECTOR3* box);
}

using namespace render;
#define COLLISION_EPSILON 0.0f


inline int render::boundsCheck(const D3DXVECTOR3* box1, const D3DXVECTOR3* box2)
{
	float pdist[6];
	float dist;

	pdist[0] = -box1[0].x;
	pdist[1] = box1[1].x;
	pdist[2] = -box1[0].y;
	pdist[3] = box1[1].y;
	pdist[4] = -box1[0].z;
	pdist[5] = box1[1].z;

	D3DXVECTOR3 pos;
	pos.x = (box2[0].x + box2[1].x) / 2;
	pos.y = (box2[0].y + box2[1].y) / 2;
	pos.z = (box2[0].z + box2[1].z) / 2;

	D3DXVECTOR3 extents;
	extents.x = abs(box2[0].x - pos.x);
	extents.y = abs(box2[0].y - pos.y);
	extents.z = abs(box2[0].z - pos.z);
	
	for( int dir = 0; dir < 3; dir++ )
	{
		// negative direction
		dist = -( pos[dir] - ( pdist[(dir>>1)] - extents[dir] ) );
		if( dist > COLLISION_EPSILON )
			return 0;

		// positive direction
		dist = pos[dir] - ( pdist[(dir>>1)+1] + extents[dir] );
		if( dist > COLLISION_EPSILON )
			return 0;
	}

	return 1;
}

SceneNode::SceneNode(SceneNode* parent)
{
	this->parent = parent;
	children = NULL;
	vertexbuffer = NULL;
	indexbuffer = NULL;

	acquired = false;
	level = parent ? parent->level + 1 : 0;
	numvertices = 0;
	numpolys = 0;
	vertice_size = 0;
	indice_size = 0;

	for(int i = 0; i < 3; i++) {
		bounds[0][i] = BIGFLOAT;
		bounds[1][i] = -BIGFLOAT;
	}
}

SceneNode::~SceneNode()
{
	delete [] children;
	if(acquired)
		release();
}

void SceneNode::acquire()
{
	acquired = true;
}

void SceneNode::release()
{
	acquired = false;
}

void SceneNode::render()
{
}

void SceneNode::addStaticMesh(Mesh& mesh)
{
	meshes.push_back(&mesh);
	//LOG8("[BSP::bspconvert] adding mesh[%i] {(%f, %f, %f), (%f, %f, %f)}",
	//	meshes.size(),
	//	mesh.bounds[0].x, mesh.bounds[0].y, mesh.bounds[0].z,
	//	mesh.bounds[1].x, mesh.bounds[1].y, mesh.bounds[1].z);

	if(!parent) {
		// extend bounding box if outside
		if(mesh.bounds[0].x < bounds[0].x) bounds[0].x = mesh.bounds[0].x;
		if(mesh.bounds[0].y < bounds[0].y) bounds[0].y = mesh.bounds[0].y;
		if(mesh.bounds[0].z < bounds[0].z) bounds[0].z = mesh.bounds[0].z;
		if(mesh.bounds[1].x > bounds[0].x) bounds[1].x = mesh.bounds[1].x;
		if(mesh.bounds[1].y > bounds[0].y) bounds[1].y = mesh.bounds[1].y;
		if(mesh.bounds[1].z > bounds[0].z) bounds[1].z = mesh.bounds[1].z;
	}
}

bool SceneNode::checkSplit()
{

	if(level >= render::max_node_level)
		return false;

	for(MeshList::iterator it = meshes.begin(); it != meshes.end(); it++) {
		Mesh* mesh = *it;

		// don't count bigass meshes because they'll end up in every child and child's child anyway
		if(boundsCheck(&bounds[0], mesh->bounds) == BOUNDS_CONTAINER) {
			LOG7("[SceneNode::checkSplit] skipping big mesh {(%f, %f, %f), (%f, %f, %f)}",
				mesh->bounds[0].x, mesh->bounds[0].y, mesh->bounds[0].z,
				mesh->bounds[1].x, mesh->bounds[1].y, mesh->bounds[1].z);
			continue;
		}

		numvertices += mesh->vertice_count;
		vertice_size += mesh->vertice_count * sizeof(q3bsp::BSPVertex);
		indice_size += mesh->indice_count * sizeof(unsigned short);
		switch(mesh->indice_format) {
			case D3DPT_TRIANGLELIST: numpolys += mesh->indice_count / 3; break;
			case D3DPT_TRIANGLESTRIP: numpolys += mesh->indice_count - 2; break;
			default: ASSERT(1); break;
		}		
	}

	LOG8("[SceneNode::checkSplit] node level %i: {%f, %f, %f} {%f, %f, %f}",
		level,
		bounds[0][0], bounds[0][1], bounds[0][2],
		bounds[1][0], bounds[1][1], bounds[1][2]);

	LOG7("[SceneNode::checkSplit] node level %i: %i meshes, %i vertices, %i polygons, %ikb vertbuf, %ikb indicebuf",
		level,
		meshes.size(),
		numvertices,
		numpolys,
		vertice_size / 1024,
		indice_size / 1024);

	if(	(meshes.size() > render::max_node_meshes) ||
		(numvertices > render::max_node_vertices) ||
		(numpolys > render::max_node_polys) ||
		(vertice_size > render::max_node_vertsize) ||
		(indice_size > render::max_node_indicesize)) 			
		return true;

	return false;
}

void SceneNode::subdivide()
{
	if(!checkSplit()) {
		LOG("[SceneNode::subdivide] no subdivison required");
		return;
	}
	else
		LOG("[SceneNode::subdivide] subdividing...");

	children = new SceneNode*[4];
	for(int i = 0; i < 4; i++)
		children[i] = new SceneNode(this);

	children[0]->bounds[0].x = bounds[0].x;
	children[0]->bounds[0].y = bounds[0].y;
	children[0]->bounds[0].z = (bounds[0].z + bounds[1].z) / 2;
	children[0]->bounds[1].x = (bounds[0].x + bounds[1].x) / 2;
	children[0]->bounds[1].y = bounds[1].y;
	children[0]->bounds[1].z = bounds[1].z;

	children[1]->bounds[0].x = (bounds[0].x + bounds[1].x) / 2;
	children[1]->bounds[0].y = bounds[0].y;
	children[1]->bounds[0].z = (bounds[0].z + bounds[1].z) / 2;
	children[1]->bounds[1].x = bounds[1].x;
	children[1]->bounds[1].y = bounds[1].y;
	children[1]->bounds[1].z = bounds[1].z;

	children[2]->bounds[0].x = bounds[0].x;
	children[2]->bounds[0].y = bounds[0].y;
	children[2]->bounds[0].z = bounds[0].z;
	children[2]->bounds[1].x = (bounds[0].x + bounds[1].x) / 2;
	children[2]->bounds[1].y = bounds[1].y;
	children[2]->bounds[1].z = (bounds[0].z + bounds[1].z) / 2;

	children[3]->bounds[0].x = (bounds[0].x + bounds[1].x) / 2;
	children[3]->bounds[0].y = bounds[0].y;
	children[3]->bounds[0].z = bounds[0].z;
	children[3]->bounds[1].x = bounds[1].x;
	children[3]->bounds[1].y = bounds[1].y;
	children[3]->bounds[1].z = (bounds[0].z + bounds[1].z) / 2;

	for(int i = 0; i < 4; i++) {
		LOG2("[SceneNode::subdivide] populating child[%i]", i);
		for(MeshList::iterator it = meshes.begin(); it != meshes.end(); it++) {
			Mesh* mesh = *it;
			if(boundsCheck(mesh->bounds, children[i]->bounds) != 0)
				children[i]->addStaticMesh(*mesh);
		}
		children[i]->subdivide();
	}
}