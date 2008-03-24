#pragma once

namespace mesh {

	class Mesh;
	class MeshSystem;

	Mesh* loadTextMesh(const string& filename);
	MeshSystem* loadTextMeshSystem(const string& filename);
}