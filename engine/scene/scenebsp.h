#pragma once

#include "q3bsp/bleh.h"
#include "render/aabb.h"

namespace render {
	class RenderGroup;
};

namespace scene {

	typedef q3bsp::BSPVertex BSPVertex; // TODO: fix

	class BSPFace {
	public:
		int texture;
		int lightmap;
		int type; // remove?
		unsigned int num_vertices;
		unsigned int num_indices;
		q3bsp::BSPVertex* vertices;
		unsigned short* indices;
		unsigned int frame;
		render::RenderGroup* rendergroup;
	};	

	typedef std::vector<BSPFace*> BSPFacePtrList;

	class BSPCluster {
	public:
		render::AABB aabb;
		unsigned int num_faces;
		//BSPFace* faces;
		BSPFacePtrList faces;
		// Entity Lists
	};

	class SceneBSP : public Scene {
	public:

		// construction
		SceneBSP();
		~SceneBSP();

		// overloads				
		void init();
		void acquire();
		void release();
		void reload(unsigned int flags = 0);
		void render();
		
		// new stuff
		static SceneBSP* loadBSP(const std::string& name);
		q3bsp::BSP *bsp;

		// what i need here:
		// list of clusters:
		//		aabb
		//		faces (RenderGroups?)
		//		list of entities (RenderGroups?)
		//		list of static 
		unsigned int num_clusters;
		BSPCluster* clusters;

		unsigned int num_faces;
		BSPFace* faces;

		//virtual addEntity(std::string name, int type, 

		// stats
		size_t vis_size;
	};
};