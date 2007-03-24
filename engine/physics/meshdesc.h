#pragma once

namespace physics {
	
	typedef enum {
		MESHDESC_NORMAL,
		MESHDESC_BSP
	} mesh_desc_t;
	
	class MeshDesc {
	public:
		MeshDesc(const char* name);
		virtual ~MeshDesc();
		
		virtual bool cook() = 0;
	
	private:
		char* name;
		mesh_desc_t type;	
	};
	
	//MeshDesc* createMeshDesc(char* name, Entity* entity);
	MeshDesc* createMeshDesc(const char* name, const Q3BSP::BSP* bsp);
}