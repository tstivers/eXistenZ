#pragma once

namespace texture {
	class DXTexture;
};

namespace mesh {

	class Mesh;

	class MeshEntry {
	public:
		MeshEntry() {};
		MeshEntry(D3DXMATRIX& transform, Mesh* mesh) { this->transform = transform ; this->mesh = mesh; };
		D3DXMATRIX transform;
		Mesh* mesh;
	};

	typedef std::vector<MeshEntry> MeshList;

	class MeshSystem {
	public:
		MeshSystem();
		~MeshSystem();
		void acquire();
		void release();

		bool acquired;
		std::string name;
		int refcount;

		// rendered geometry
		MeshList meshes;

		// collision geometry, etc.
	};

};