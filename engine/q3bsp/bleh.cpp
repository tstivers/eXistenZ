/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: bleh.cpp,v 1.1 2003/11/18 18:39:42 tstivers Exp $
//

#include "precompiled.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bsppatch.h"
#include "vfs/vfs.h"
#include "vfs/file.h"
#include "console/console.h"
#include "texture/texturecache.h"
#include "texture/texture.h"

using namespace q3bsp;

BSP::BSP()
{
	num_verts = 0;
	num_indices = 0;
	num_faces = 0;
	num_nodes = 0;
	num_leafs = 0;
	num_leaffaces = 0;
	num_leafbrushes = 0;
	num_brushes = 0;
	num_brushsides = 0;
	num_planes = 0;	
	num_clusters = 0;
	num_textures = 0;
	num_lightmaps = 0;
	cluster_size = 0;

	verts = NULL;
	indices = NULL;
	faces = NULL;
	nodes = NULL;
	leafs = NULL;
	leaffaces = NULL;
	leafbrushes = NULL;
	brushes = NULL;
	brushsides = NULL;
	planes = NULL;
	clusters = NULL;
	bsptextures = NULL;

	drawn_faces = NULL;
	sorted_faces = NULL;
	transparent_faces = NULL;

	dxvertbuf = NULL;
	dxindexbuf = NULL;
}

BSP::~BSP()
{
	delete [] verts;
	delete [] indices;
	delete [] faces;
	delete [] nodes;
	delete [] leafs;
	delete [] leaffaces;
	delete [] leafbrushes;
	delete [] brushes;
	delete [] brushsides;
	delete [] planes;
	delete [] clusters;
	delete [] bsptextures;

	delete [] drawn_faces;
	delete [] sorted_faces;
	delete [] transparent_faces;

	if(dxvertbuf) dxvertbuf->Release();
	if(dxindexbuf) dxindexbuf->Release();

	//TODO: release textures
}
