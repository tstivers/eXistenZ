/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id$
//

#include "precompiled.h"
#include "q3bsp/bspconvert.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bsprender.h"
#include "render/render.h"
#include "console/console.h"
#include "nvtristrip.h"

void q3bsp::convertBSP(BSP& bsp)
{
	polylist_hash polyhash;

	LOG("[BSP::convertBSP] starting conversion");

	for(int face_idx = 0; face_idx < bsp.num_faces; face_idx++)
	{
		BSPFace& face = bsp.faces[face_idx];
		
		if((face.type < 0) || (face.type > 3))
			continue;

		/*if(render::use_scenegraph && face.type == 2) {
			if((face.texture >= 0) && (face.texture <= bsp.num_textures) && bsp.textures[face.texture] && bsp.textures[face.texture]->draw) {
				render::Mesh* mesh = new render::Mesh();
				mesh->texture = bsp.textures[face.texture];
				mesh->lightmap = (face.lightmap >= 0) && (face.lightmap <= bsp.num_lightmaps) ? bsp.lightmaps[face.lightmap] : NULL;
				mesh->transform = &render::world;
				mesh->vertice_format = sizeof(BSPVertex);
				mesh->vertice_count = face.numverts;
				mesh->indice_format = D3DPT_TRIANGLELIST;
				mesh->indice_count = face.nummeshverts;
				mesh->poly_count = face.nummeshverts / 3;
				mesh->vertices = new BSPVertex[face.numverts];
				mesh->indices = new unsigned short[face.nummeshverts];
				BSPVertex* vertices = (BSPVertex*)mesh->vertices;
				unsigned short* indices = (unsigned short*)mesh->indices;

				for(int i = 0; i < face.numverts; i++) {
					vertices[i] = bsp.verts[face.vertex + i];
					mesh->bbox.extend(&(vertices[i].pos));
				}

				for(int i = 0; i < face.nummeshverts; i++)
					indices[i] = bsp.indices[face.meshvertex + i];

				//LOG7("[BSP::bspconvert] adding mesh {(%f, %f, %f), (%f, %f, %f)}",
				//	mesh->bounds[0].x, mesh->bounds[0].y, mesh->bounds[0].z,
				//	mesh->bounds[1].x, mesh->bounds[1].y, mesh->bounds[1].z);

				render::scene.addStaticMesh(*mesh);
			}
			//continue;
		} */

		polylist_value* bucket;
		polylist_hash::iterator it = polyhash.find(polylist_key(face.texture, face.lightmap));
		
		if(it == polyhash.end()) {
			bucket = new polylist_value;
			polyhash.insert(polylist_hash::value_type(polylist_key(face.texture, face.lightmap), bucket));
		}
		else
			bucket = (*it).second;

		// copy vertices
		for(int i = 0; i < face.numverts; i++)
			bucket->vertices.push_back(bsp.verts[face.vertex + i]);

		// copy indices
		for(int i = 0; i < face.nummeshverts; i++)
			bucket->indices.push_back(bsp.indices[face.meshvertex + i] + bucket->vertice_count);

		// update totals
		bucket->vertice_count += face.numverts;
		bucket->indice_count += face.nummeshverts;
	}

	//render::scene.finalizeStatic();

	int total_verts = 0;
	int total_indices = 0;
	int total_buckets = 0;

	for(polylist_hash::iterator it = polyhash.begin(); it != polyhash.end(); it++) {
		total_buckets++;
		polylist_value* bucket = (*it).second;
		ASSERT(bucket->indice_count < 0xffff);
		total_verts += bucket->vertice_count;
		total_indices += bucket->indice_count;
	}

	LOG2("[BSP::convertBSP] processed %i buckets", total_buckets);

	BSPRenderTest* renderer = new BSPRenderTest(&bsp);
	renderer->meshes = new Mesh[total_buckets];
	renderer->num_meshes = total_buckets;

	int i = 0;
	for(polylist_hash::iterator it = polyhash.begin(); it != polyhash.end(); it++, i++) {
		polylist_value* bucket = (*it).second;
		renderer->meshes[i].texture = ((*it).first.texture >= 0) && ((*it).first.texture <= bsp.num_textures) ? bsp.textures[(*it).first.texture] : NULL;
		renderer->meshes[i].lightmap = ((*it).first.lightmap >= 0) && ((*it).first.lightmap <= bsp.num_lightmaps) ? bsp.lightmaps[(*it).first.lightmap] : NULL;
		renderer->meshes[i].num_indices = bucket->indice_count;
		renderer->meshes[i].num_vertices = bucket->vertice_count;
		renderer->meshes[i].indices = new unsigned short[bucket->indice_count];
		renderer->meshes[i].vertices = new BSPVertex[bucket->vertice_count];

		for(int j = 0; j < bucket->indice_count; j++)
			renderer->meshes[i].indices[j] = bucket->indices[j];

		for(int j = 0; j < bucket->vertice_count; j++)
			renderer->meshes[i].vertices[j] = bucket->vertices[j];
	}

/*	SetCacheSize(CACHESIZE_GEFORCE3);
	SetStitchStrips(true);

	for(int i = 0; i < total_buckets; i++) {
		Mesh& mesh = renderer->meshes[i];
		PrimitiveGroup* primitives;
		unsigned short num_prims;

		GenerateStrips(mesh.indices, mesh.num_indices, &primitives, &num_prims);
		ASSERT(num_prims == 1);

		unsigned short* new_indices = new unsigned short[primitives[0].numIndices];
		memcpy(new_indices, primitives[0].indices, primitives[0].numIndices * sizeof(unsigned short));
		delete [] mesh.indices;
		mesh.indices = new_indices;
		
		LOG3("[BSP::convertBSP] optimized from %i to %i indices",
			mesh.num_indices,
			primitives[0].numIndices);

		mesh.num_indices = primitives[0].numIndices;
		delete [] primitives;
	}
*/

	for(polylist_hash::iterator it = polyhash.begin(); it != polyhash.end(); it++, i++) {
		polylist_value* bucket = (*it).second;
		bucket->indices.clear();
		bucket->vertices.clear();
		delete bucket;
	}

	bsp.renderer = renderer;
}