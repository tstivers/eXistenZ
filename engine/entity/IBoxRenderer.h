#pragma once

namespace entity {
	class BoxEntity;
	
	struct IBoxRenderer : public render::IRenderable {
		IBoxRenderer(BoxEntity* box) : IRenderable() { this->box = box; }
		void render();
		const AABB& getAABB();
		
		BoxEntity* box;
		AABB aabb;
	}
}