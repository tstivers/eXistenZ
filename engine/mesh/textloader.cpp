/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: textloader.cpp,v 1.2 2003/12/05 08:44:56 tstivers Exp $
//

#include "precompiled.h"
#include "mesh/textloader.h"
#include "mesh/mesh.h"
#include "mesh/meshsystem.h"
#include "mesh/meshcache.h"
#include "vfs/vfs.h"
#include "vfs/file.h"
#include "q3bsp/bleh.h"  // for vertex typedef
#include "console/console.h"
#include "texture/texturecache.h"

namespace mesh {
	// TODO: make this go away
	typedef q3bsp::BSPVertex Vertex;

	void parseVertex(Vertex* vertex, char* info);
};

using namespace mesh;

void mesh::parseVertex(Vertex* vertex, char* info)
{
	ZeroMemory(vertex, sizeof(Vertex));

	char* token = info;
	char* this_token;
	while(this_token = getToken(&token, ":[")) {
		if(!stricmp(this_token, "POS")) {
			this_token = getToken(&token, ",");
			vertex->pos.x = (float)atof(this_token);
			this_token = getToken(&token, ",");
			vertex->pos.y = (float)atof(this_token);
			this_token = getToken(&token, " ");
			vertex->pos.z = (float)atof(this_token);
			continue;
		}

		if(!stricmp(this_token, "NRM")) {
			this_token = getToken(&token, ",");
			vertex->nrm.x = (float)atof(this_token);
			this_token = getToken(&token, ",");
			vertex->nrm.y = (float)atof(this_token);
			this_token = getToken(&token, " ");
			vertex->nrm.z = (float)atof(this_token);
			continue;
		}

		if(!stricmp(this_token, "COL")) {
			this_token = getToken(&token, ",");
			D3DXCOLOR color;
			color.r = (float)atof(this_token);
			this_token = getToken(&token, ",");
			color.g = (float)atof(this_token);
			this_token = getToken(&token, ",");
			color.b = (float)atof(this_token);
			this_token = getToken(&token, " ");
			color.a = (float)atof(this_token);						
			vertex->diffuse = color;
			continue;
		}

		if(!stricmp(this_token, "UV")) {
			this_token = getToken(&token, ":");
			this_token = getToken(&token, ",");
			vertex->tex1.x = (float)atof(this_token);
			this_token = getToken(&token, " ");
			vertex->tex1.y = (float)atof(this_token);
		}
	}
}

Mesh* mesh::loadTextMesh(const std::string& filename)
{	
	VFile* file = vfs::getFile(filename.c_str());
	if(!file)
		return NULL;

	Mesh* mesh = new Mesh();	

	// i'm sick of this shit, do everything with xml and a parser
	// using callbacks so i never have to parse a fucking line of text
	// ever again
	char buf[1024];
	unsigned current_vert = 0;
	unsigned current_ind = 0;

	while(!file->eof) {

		file->readLine(buf, 1024);

		char* comment = strstr(buf, "//");
		if(comment) *comment = 0;

		strip(buf);

		if(!buf[0]) continue;
		char* token = buf;
		char* this_token;
		this_token = getToken(&token, " ");
		
		if(!stricmp(this_token, "MATERIAL:")) {
			mesh->material_name = token;
			mesh->texture = texture::getTexture(token);
		}
		
		if(!stricmp(this_token, "VERTICEFORMAT:")) {
			//mesh->vertice_format = fvfFromString(token);
			mesh->vertice_format = BSPVERTEXF;
		}

		if(!stricmp(this_token, "VERTICECOUNT:")) {
			mesh->vertice_count = atoi(token);
			// TODO: switch based on fvf value
			mesh->vertices = new Vertex[mesh->vertice_count];
		}

		if(!stricmp(this_token, "INDICECOUNT:")) {
			mesh->indice_count = atoi(token);
			mesh->indices = new unsigned short[mesh->indice_count];
			mesh->indice_type = D3DPT_TRIANGLELIST;
		}

		if(!_memicmp(this_token, "VERTEX[", strlen("VERTEX["))) {
			Vertex* vertices = (Vertex*)mesh->vertices;
			parseVertex(&vertices[current_vert], token);
			current_vert++;
		}

		if(!_memicmp(this_token, "INDICE[", strlen("INDICE["))) {			
			mesh->indices[current_ind] = atoi(token);
			current_ind++;
		}
	}

	return mesh;
}

MeshSystem* mesh::loadTextMeshSystem(const std::string& filename)
{
	VFile* file = vfs::getFile(filename.c_str());
	if(!file)
		return NULL;

	MeshSystem* meshsys = new MeshSystem();	

	// i'm sick of this shit, do everything with xml and a parser
	// using callbacks so i never have to parse a fucking line of text
	// ever again
	char buf[1024];
	unsigned current_vert = 0;
	unsigned current_ind = 0;

	while(!file->eof) {
		file->readLine(buf, 1024);

		char* comment = strstr(buf, "//");
		if(comment) *comment = 0;

		strip(buf);

		if(!buf[0]) continue;
		char* token = buf;
		char* this_token;
		this_token = getToken(&token, " ");

		if(!stricmp(this_token, "MESH:")) {
			Mesh* mesh = getMesh(std::string(token));
			if(!mesh) {
				LOG3("[mesh::loadMeshSystem] failed to load mesh %s for system %s",
					token, filename.c_str());
				continue;
			}

			D3DXMATRIX transform;
			meshsys->meshes.push_back(MeshEntry(*(D3DXMatrixIdentity(&transform)), mesh));
		}
	}
		return meshsys;
}