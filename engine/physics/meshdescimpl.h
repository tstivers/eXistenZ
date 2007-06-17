#pragma once
#include <NxPhysics.h>
#include "stream.h"

namespace scene {
	class SceneBSP;
}

namespace physics {
	
	class MeshDescImpl : public MeshDesc {
	public:
		MeshDescImpl(const char* name);
		~MeshDescImpl();
		
		bool cook();
	
	protected:
		NxTriangleMeshDesc desc;		
		std::vector<D3DXVECTOR3> vertices;
		std::vector<unsigned int> indices;
		NxTriangleMesh* mesh;
	};
	
	class BSPMeshDescImpl : public MeshDescImpl {
	public:
		BSPMeshDescImpl(const char* name, scene::SceneBSP* scene);
		~BSPMeshDescImpl();
		
	private:
				
	};
}