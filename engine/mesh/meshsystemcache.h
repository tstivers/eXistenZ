/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: meshsystemcache.h,v 1.1 2003/12/04 12:33:48 tstivers Exp $
//

#pragma once

namespace mesh {

	class MeshSystem;

	MeshSystem* getMeshSystem(std::string& name);
	bool addMeshSystem(MeshSystem* meshsys);
	bool delMeshSystem(MeshSystem* meshsys);
	bool delMeshSystem(std::string& name);
	unsigned int clearMeshSystemCache();

};