#pragma once

#include "q3bsp/bleh.h"

namespace scene {
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

		// renderpass looks like:
		//	loop through all clusters
		//		test cluster visibility, then frustrum cull
		//		render all faces
		//		render all renderable entities


		//virtual addEntity(std::string name, int type, 
	};
};