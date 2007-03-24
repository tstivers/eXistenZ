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
	
	private:
		NxTriangleMeshDesc desc;
		MemoryStream stream;
	};
	
	class BSPMeshDescImpl : public MeshDescImpl {
	public:
		BSPMeshDescImpl(const char* name, scene::SceneBSP* scene);
		~BSPMeshDescImpl();
		
	private:
				
	};
}