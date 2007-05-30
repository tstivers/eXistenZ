#pragma once

namespace render {

struct IRenderable {
	public:
		virtual void render() = 0;
		virtual const AABB& getAABB() = 0;
}
}