#pragma once

namespace scene
{
	class SceneBSP;
}

namespace physics
{

	typedef enum
	{
		MESHDESC_NORMAL,
		MESHDESC_BSP
	} mesh_desc_t;

	class MeshDesc
	{
	public:
		MeshDesc(const char* name);
		virtual ~MeshDesc();

		virtual bool cook() = 0;

	protected:
		char* name;
		mesh_desc_t type;
	};

	//MeshDesc* createMeshDesc(char* name, Entity* entity);
	MeshDesc* createMeshDesc(const char* name, scene::SceneBSP* scene);
}