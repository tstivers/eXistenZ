/////////////////////////////////////////////////////////////////////////////
// render.cpp
// rendering system implementation
// $Id$
//

#include "precompiled.h"
#include "render/render.h"
#include "render/scenegraph.h"
#include "console/console.h"
#include "q3bsp/bleh.h"
#include "math/aabb.h"
#include "render/shapes.h"

namespace render {
}

using namespace render;

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
}

SceneNode::~SceneNode()
{
	delete [] children;
	if(acquired)
		release();
}

void SceneNode::acquire()
{
	if(acquired)
		return;

	if((vertice_size < max_node_vertsize) && (vertice_size > 0)) {
		if(FAILED(render::device->CreateVertexBuffer((UINT)vertice_size,
			D3DUSAGE_WRITEONLY,
			BSPVertex::FVF,
			D3DPOOL_MANAGED,
			&vertexbuffer,
			NULL))) {
				LOG("[BSPRenderTest::acquire] failed to create vertex buffer");
				return;
			}

		void* buffer;

		if(FAILED(vertexbuffer->Lock(0, (UINT)vertice_size, &buffer, D3DLOCK_DISCARD))) {
			LOG("[BSPRenderTest::acquire] failed to lock vertex buffer");
			return;
		}

		int vertbuf_pos = 0;
		for(MeshList::iterator it = meshes.begin(); it != meshes.end(); it++) {
			Mesh* mesh = *it;
			if(mesh->rendergroup)
				continue;

		/*	mesh->rendergroup = new RenderGroup();
			mesh->rendergroup->texture = mesh->texture;
			mesh->rendergroup->lightmap = mesh->lightmap;
			mesh->rendergroup->vertexbuffer = vertexbuffer;
			mesh->rendergroup->transform = mesh->transform;
			mesh->rendergroup->vertices = mesh->vertices;
			mesh->rendergroup->indices = mesh->indices;
			//mesh->rendergroup->vertex_stride = sizeof(BSPVertex);
			mesh->rendergroup->type = (D3DPRIMITIVETYPE)mesh->vertice_format;
			//mesh->rendergroup->basevertex = 
			mesh->rendergroup->minindex = 0;
			mesh->rendergroup->numvertices = mesh->vertice_count;
			//mesh->rendergroup.startindex = 
			*/
		}
	}

	acquired = true;
}

void SceneNode::release()
{
	acquired = false;
}

void SceneNode::render()
{
	if(children)
		for(int i = 0; i < 8; i++)
			children[i]->render();
	else {
		if(meshes.size()) {
			if(bbox.contains(&render::cam_pos)) {
				render::drawBox(&bbox.min, &bbox.max, 0.0f, 1.0f, 0.0f);
				FRAMEDO(LOG4("[SceneNode::render] meshes:%i  vertices: %i  polys: %i", meshes.size(), this->numvertices, this->numpolys));
			}
			else
				render::drawBox(&bbox.min, &bbox.max, 1.0f, 1.0f, 1.0f);
		}
		
		for(MeshList::iterator it = meshes.begin(); it != meshes.end(); it++) {
			Mesh* mesh = *it;
			if(bbox.contains(&render::cam_pos))
				render::drawBox(&mesh->bbox.min, &mesh->bbox.max, 0.0f, 0.0f, 1.0f);
			else
				render::drawBox(&mesh->bbox.min, &mesh->bbox.max, 1.0f, 0.0f, 0.0f);
		}
	}
}

void SceneNode::addStaticMesh(Mesh& mesh)
{
	meshes.push_back(&mesh);

	if(!parent) {
		// extend bounding box if outside
		bbox.extend(&mesh.bbox);
	}
}

bool SceneNode::checkSplit()
{

	if(level >= render::max_node_level)
		return false;

	int contained = 0;

	for(MeshList::iterator it = meshes.begin(); it != meshes.end(); it++) {
		Mesh* mesh = *it;

		// don't count bigass meshes because they'll end up in every child and child's child anyway
		if(bbox.isContained(&(mesh->bbox))) {
			//LOG("[SceneNode::checkSplit] skipping big mesh");
			//bbox.debugPrint();
			//LOG("[SceneNode::checkSplit] is conained by");
			//mesh->bbox.debugPrint();
			continue;
		}

		if(bbox.contains(&(mesh->bbox)))
			contained++;

		numvertices += mesh->vertice_count;
		vertice_size += mesh->vertice_count * sizeof(BSPVertex);
		indice_size += mesh->indice_count * sizeof(unsigned short);
		switch(mesh->indice_format) {
			case D3DPT_TRIANGLELIST: numpolys += mesh->indice_count / 3; break;
			case D3DPT_TRIANGLESTRIP: numpolys += mesh->indice_count - 2; break;
			default: ASSERT(1); break;
		}		
	}

	//LOG8("[SceneNode::checkSplit] node level %i: {%f, %f, %f} {%f, %f, %f}",
	//	level,
	//	bounds[0][0], bounds[0][1], bounds[0][2],
	//	bounds[1][0], bounds[1][1], bounds[1][2]);

	LOG8("[SceneNode::checkSplit] node level %i: %i[%i] meshes, %i vertices, %i polygons, %ikb vertbuf, %ikb indicebuf",
		level,
		meshes.size(), contained,
		numvertices,
		numpolys,
		vertice_size / 1024,
		indice_size / 1024);

	//bbox.debugPrint();	

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
		//LOG("[SceneNode::subdivide] no subdivison required");
		return;
	}
//	else
//		LOG("[SceneNode::subdivide] subdividing...");

	children = new SceneNode*[8];
	for(int i = 0; i < 8; i++)
		children[i] = new SceneNode(this);

	children[0]->bbox.min = D3DXVECTOR3(bbox.min.x, bbox.pos.y, bbox.min.z);
	children[0]->bbox.max = D3DXVECTOR3(bbox.pos.x, bbox.max.y, bbox.pos.z);

	children[1]->bbox.min = D3DXVECTOR3(bbox.min.x, bbox.pos.y, bbox.pos.z);
	children[1]->bbox.max = D3DXVECTOR3(bbox.pos.x, bbox.max.y, bbox.max.z);

	children[2]->bbox.min = D3DXVECTOR3(bbox.pos.x, bbox.pos.y, bbox.min.z);
	children[2]->bbox.max = D3DXVECTOR3(bbox.max.x, bbox.max.y, bbox.pos.z);

	children[3]->bbox.min = D3DXVECTOR3(bbox.pos.x, bbox.pos.y, bbox.pos.z);
	children[3]->bbox.max = D3DXVECTOR3(bbox.max.x, bbox.max.y, bbox.max.z);

	children[4]->bbox.min = D3DXVECTOR3(bbox.min.x, bbox.min.y, bbox.min.z);
	children[4]->bbox.max = D3DXVECTOR3(bbox.pos.x, bbox.pos.y, bbox.pos.z);

	children[5]->bbox.min = D3DXVECTOR3(bbox.min.x, bbox.min.y, bbox.pos.z);
	children[5]->bbox.max = D3DXVECTOR3(bbox.pos.x, bbox.pos.y, bbox.max.z);

	children[6]->bbox.min = D3DXVECTOR3(bbox.pos.x, bbox.min.y, bbox.min.z);
	children[6]->bbox.max = D3DXVECTOR3(bbox.max.x, bbox.pos.y, bbox.pos.z);

	children[7]->bbox.min = D3DXVECTOR3(bbox.pos.x, bbox.min.y, bbox.pos.z);
	children[7]->bbox.max = D3DXVECTOR3(bbox.max.x, bbox.pos.y, bbox.max.z);

	for(int i = 0; i < 8; i++) {
		//LOG2("[SceneNode::subdivide] populating child[%i]", i);
		children[i]->bbox.recalcPos();
		ASSERT(children[i]->bbox.cdist > 0.0f);
		for(MeshList::iterator it = meshes.begin(); it != meshes.end(); it++) {
			Mesh* mesh = *it;
			if(children[i]->bbox.intersects(&(mesh->bbox)))
				children[i]->addStaticMesh(*mesh);
		}
		children[i]->subdivide();
	}
}