#pragma once

namespace mesh
{

	class Mesh;

	Mesh* getMesh(string& name);
	bool addMesh(Mesh* mesh);
	bool delMesh(Mesh* mesh);
	bool delMesh(string& name);
	unsigned int clearMeshCache();
}