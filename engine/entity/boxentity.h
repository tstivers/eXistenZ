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
		void setPos(const D3DXVECTOR3& pos);
		void setRot(const D3DXVECTOR3& rot);
		D3DXVECTOR3& getRot();
		D3DXVECTOR3& getPos();

		NxActor* actor;
		texture::DXTexture* texture;
	};	
}