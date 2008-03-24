#pragma once

namespace mesh {

	class Mesh;
	class MeshSystem;

	Mesh* loadMesh(const string& name);
	MeshSystem* loadMeshSystem(const string& name);
}