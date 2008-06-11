#pragma once

namespace mesh
{

	class MeshSystem;

	MeshSystem* getMeshSystem(string& name);
	bool addMeshSystem(MeshSystem* meshsys);
	bool delMeshSystem(MeshSystem* meshsys);
	bool delMeshSystem(string& name);
	unsigned int clearMeshSystemCache();

};