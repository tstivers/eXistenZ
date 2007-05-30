#pragma once

namespace mesh {
	class MeshSystem;
};

namespace entity {	
	class StaticEntity : public Entity {
	public:
		StaticEntity(std::string name);
		~StaticEntity();
		void acquire();
		void release();
		void render();
		void calcAABB();

		static StaticEntity* create(std::string name, std::string meshname);

		std::string meshname;
		mesh::MeshSystem* meshsys;
	};
};