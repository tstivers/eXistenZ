#include "precompiled.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bsppatch.h"
#include "render/render.h"
#include "console/console.h"
#include "nvtristrip.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"

using namespace q3bsp;

MyBiquadraticPatch::MyBiquadraticPatch()
{
	vertices = NULL;
	indices = NULL;
	list = NULL;
}

MyBiquadraticPatch::~MyBiquadraticPatch()
{
	delete [] vertices;
	delete [] indices;
	delete [] list;
}

void MyBiquadraticPatch::Tesselate(int newTesselation)
{
	tesselation = newTesselation;

	float px, py;
	BSPVertex temp[3];
	num_verts = (tesselation+1)*(tesselation+1);
	vertices = new BSPVertex[num_verts];

	for(int v = 0; v <= tesselation; ++v)
	{
		px = (float)v / tesselation;
		
		vertices[v] = controlPoints[0]*((1.0f-px)*(1.0f-px))+
			controlPoints[3]*((1.0f-px)*px*2)+
			controlPoints[6]*(px*px);
	}

	for(int u=1; u<=tesselation; ++u)
	{
		py=(float)u/tesselation;

		temp[0]=controlPoints[0]*((1.0f-py)*(1.0f-py))+
			controlPoints[1]*((1.0f-py)*py*2)+
			controlPoints[2]*(py*py);

		temp[1]=controlPoints[3]*((1.0f-py)*(1.0f-py))+
			controlPoints[4]*((1.0f-py)*py*2)+
			controlPoints[5]*(py*py);

		temp[2]=controlPoints[6]*((1.0f-py)*(1.0f-py))+
			controlPoints[7]*((1.0f-py)*py*2)+
			controlPoints[8]*(py*py);

		for(int v=0; v<=tesselation; ++v)
		{
			px=(float)v/tesselation;

			vertices[u*(tesselation+1)+v]=	temp[0]*((1.0f-px)*(1.0f-px))+
				temp[1]*((1.0f-px)*px*2)+
				temp[2]*(px*px);
		}
	}

	//Create indices
	num_indices = tesselation*(tesselation+1)*2;
	indices = new unsigned int[num_indices];	

	for(int row=0; row<tesselation; ++row)
	{
		for(int point=0; point<=tesselation; ++point)
		{
			//calculate indices			
			indices[(row*(tesselation+1)+point)*2+1]=row*(tesselation+1)+point;
			indices[(row*(tesselation+1)+point)*2]=(row+1)*(tesselation+1)+point;
		}
	}
}

void MyBiquadraticPatch::dumpIndices()
{
	num_polys = 0;
	for(int row=0; row<tesselation; ++row) {
		num_polys += 2*(tesselation+1) - 2;		
	}

	list = new int[num_polys * 3];

	int current_idx = 0;
	num_polys = 0;

	for(int row=0; row<tesselation; ++row) {
		list[current_idx++] = indices[(row*2*(tesselation+1)) + 0];
		list[current_idx++] = indices[(row*2*(tesselation+1)) + 1];
		list[current_idx++] = indices[(row*2*(tesselation+1)) + 2];
		num_polys++;

		for(int i = 3; i < 2*(tesselation+1); i++) {		
			list[current_idx + 0] = list[current_idx - 2];
			list[current_idx + 1] = indices[(row*2*(tesselation+1)) + i];
			list[current_idx + 2] = list[current_idx - 1];
			current_idx += 3;
			num_polys++;
			i++;
			if(i < 2*(tesselation+1)) {
				list[current_idx + 0] = list[current_idx - 1];
				list[current_idx + 1] = list[current_idx - 2];
				list[current_idx + 2] = indices[(row*2*(tesselation+1)) + i];							
				current_idx += 3;
				num_polys++;
			}
		}		
	}
}

void BSP::generatePatches()
{
	for(int face_index = 0; face_index < num_faces; face_index++) {
		BSPFace& face = faces[face_index];

		// skip non-patches
		if(face.type != 2)
			continue;

		int width = face.size[0];
		int height = face.size[1];

		int numPatchesWide = (width - 1) / 2;
		int numPatchesHigh = (height - 1) / 2;
		int total_vertices = 0;
		int total_indices = 0;

		int numQuadraticPatches = numPatchesWide * numPatchesHigh;
		MyBiquadraticPatch* quadraticPatches = new MyBiquadraticPatch[numQuadraticPatches];

		//fill in the quadratic patches
		for(int y = 0; y < numPatchesHigh; ++y) {		
			for(int x = 0; x < numPatchesWide; ++x) {
				for(int row = 0; row < 3; ++row) {
					for(int point = 0; point < 3; ++point) {
						quadraticPatches[y * numPatchesWide + x].controlPoints[row * 3 + point] = 
							verts[face.vertex + (y * 2 * width + x * 2) + row * width + point];
					}
				}

				//tesselate the patch
				quadraticPatches[y*numPatchesWide+x].Tesselate(render::tesselation);
				quadraticPatches[y*numPatchesWide+x].dumpIndices();
				total_vertices += quadraticPatches[y * numPatchesWide + x].num_verts;
				total_indices += quadraticPatches[y * numPatchesWide + x].num_polys * 3;
			}
		}

		// we have our tesselated patches now, combine them all into one bigass vert/index array
		BSPVertex* vertices = new BSPVertex[total_vertices];
		int* indices = new int[total_indices];
		int current_vert = 0;
		int current_indice = 0;

		for(int i = 0; i < numQuadraticPatches; i++) {
			memcpy(&(vertices[current_vert]), quadraticPatches[i].vertices, quadraticPatches[i].num_verts * sizeof(BSPVertex));
			for(int indice = 0; indice < quadraticPatches[i].num_polys * 3; indice++) {
				indices[current_indice + indice] = quadraticPatches[i].list[indice] + current_vert;
			}
			current_vert += quadraticPatches[i].num_verts;
			current_indice += quadraticPatches[i].num_polys * 3;
		}

		// TODO: optimize mesh here
		
		// toss onto the end of verts/indices
		BSPVertex* tmp_verts = new BSPVertex[num_verts + total_vertices];
		memcpy(tmp_verts, this->verts, num_verts * sizeof(BSPVertex));
		memcpy(&(tmp_verts[num_verts]), vertices, total_vertices * sizeof(BSPVertex));

		int* tmp_indices = new int[num_indices + total_indices];
		memcpy(tmp_indices, this->indices, num_indices * sizeof(int));
		memcpy(&(tmp_indices[num_indices]), indices, total_indices * sizeof(int));

		face.vertex = num_verts;
		face.numverts = total_vertices;
		face.meshvertex = num_indices;
		face.nummeshverts = total_indices;

		delete [] this->indices;
		delete [] this->verts;
		delete [] indices;
		delete [] vertices;
		delete [] quadraticPatches;

		this->indices = tmp_indices;
		this->verts = tmp_verts;
		num_verts += total_vertices;
		num_indices += total_indices;		
	}
}

void q3bsp::genPatch(scene::BSPFace& face, int width, int height)
{
	int numPatchesWide = (width - 1) / 2;
	int numPatchesHigh = (height - 1) / 2;
	int total_vertices = 0;
	int total_indices = 0;

	int numQuadraticPatches = numPatchesWide * numPatchesHigh;
	MyBiquadraticPatch* quadraticPatches = new MyBiquadraticPatch[numQuadraticPatches];

	//fill in the quadratic patches
	for(int y = 0; y < numPatchesHigh; ++y) {		
		for(int x = 0; x < numPatchesWide; ++x) {
			for(int row = 0; row < 3; ++row) {
				for(int point = 0; point < 3; ++point) {
					quadraticPatches[y * numPatchesWide + x].controlPoints[row * 3 + point] = 
						face.vertices[(y * 2 * width + x * 2) + row * width + point];
				}
			}

			//tesselate the patch
			quadraticPatches[y*numPatchesWide+x].Tesselate(render::tesselation);
			quadraticPatches[y*numPatchesWide+x].dumpIndices();
			total_vertices += quadraticPatches[y * numPatchesWide + x].num_verts;
			total_indices += quadraticPatches[y * numPatchesWide + x].num_polys * 3;
		}
	}

	// we have our tesselated patches now, combine them all into one bigass vert/index array
	BSPVertex* vertices = new BSPVertex[total_vertices];
	unsigned short* indices = new unsigned short[total_indices];
	int current_vert = 0;
	int current_indice = 0;

	for(int i = 0; i < numQuadraticPatches; i++) {
		memcpy(&(vertices[current_vert]), quadraticPatches[i].vertices, quadraticPatches[i].num_verts * sizeof(BSPVertex));
		for(int indice = 0; indice < quadraticPatches[i].num_polys * 3; indice++) {
			indices[current_indice + indice] = quadraticPatches[i].list[indice] + current_vert;
		}
		current_vert += quadraticPatches[i].num_verts;
		current_indice += quadraticPatches[i].num_polys * 3;
	}

	delete [] face.indices;
	delete [] face.vertices;
	delete [] quadraticPatches;

	face.indices = indices;
	face.vertices = vertices;
	face.num_indices = total_indices;
	face.num_vertices = total_vertices;
	face.type = 1;
}
