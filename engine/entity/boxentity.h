#pragma once

class NxActor;

namespace texture {;
	class DXTexture;
}

namespace entity {
	class BoxEntity : public Entity {
	public:
		BoxEntity(std::string name, std::string texture);
		~BoxEntity();
		void acquire();
		void release();
		void render();
		void calcAABB();
		void update();
		void doTick();
		void applyForce(const D3DXVECTOR3 &force);

		NxActor* actor;
		texture::DXTexture* texture;
	};	
}