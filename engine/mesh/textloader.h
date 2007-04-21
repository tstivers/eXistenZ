#pragma once

namespace mesh {

	class Mesh;
	class MeshSystem;

	Mesh* loadTextMesh(const std::string& filename);
	MeshSystem* loadTextMeshSystem(const std::string& filename);
}