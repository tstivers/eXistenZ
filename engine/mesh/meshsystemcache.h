#pragma once

namespace mesh {

	class MeshSystem;

	MeshSystem* getMeshSystem(std::string& name);
	bool addMeshSystem(MeshSystem* meshsys);
	bool delMeshSystem(MeshSystem* meshsys);
	bool delMeshSystem(std::string& name);
	unsigned int clearMeshSystemCache();

};