/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: meshcache.h,v 1.1 2003/12/04 12:33:48 tstivers Exp $
//

#pragma once

namespace mesh {
	
	class Mesh;

	Mesh* getMesh(std::string& name);
	bool addMesh(Mesh* mesh);
	bool delMesh(Mesh* mesh);
	bool delMesh(std::string& name);
	unsigned int clearMeshCache();
};