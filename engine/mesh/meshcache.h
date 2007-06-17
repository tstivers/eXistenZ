#pragma once

namespace mesh {
	
	class Mesh;

	Mesh* getMesh(std::string& name);
	bool addMesh(Mesh* mesh);
	bool delMesh(Mesh* mesh);
	bool delMesh(std::string& name);
	unsigned int clearMeshCache();
};