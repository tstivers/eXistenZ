#pragma once

namespace physics {
	
	class MeshDescImpl : public MeshDesc {
	public:
		MeshDescImpl(const char* name);
		~MeshDescImpl();
		
		bool cook();
	
	private:
		NxTriangleMeshDesc desc;
	}
	
	class BSPMeshDescImpl : public MeshDescImpl {
	public:
		BSPMeshDescImpl(const char* name, const Q3BSP::BSP* bsp);
		~BSPMeshDescImpl();
		
	private:
				
	};
}