/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: meshsystem.h,v 1.1 2003/12/04 12:33:48 tstivers Exp $
//

#pragma once

namespace texture {
	class DXTexture;
};

namespace mesh {

	class Mesh;

	typedef struct {
		D3DXMATRIX transform;
		Mesh* mesh;
	} MeshEntry;

	typedef std::vector<MeshEntry> MeshList;

	class MeshSystem {
	public:
		MeshSystem();
		~MeshSystem();
		void acquire();
		void release();

		std::string name;
		int refcount;

		// rendered geometry
		MeshList meshes;

		// collision geometry, etc.
	};

};