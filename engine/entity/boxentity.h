#pragma once

class NxActor;

namespace texture {;
	class DXTexture;
	struct Material;
}

namespace entity {
	class BoxEntity : public Entity {
	public:
		BoxEntity(string name, string texture);
		~BoxEntity();
		void acquire();
		void release();
		void render(texture::Material* lighting);
		void calcAABB();
		void update();
		void doTick();
		void applyForce(const D3DXVECTOR3 &force);
		void setPos(const D3DXVECTOR3& pos);
		void setRot(const D3DXVECTOR3& rot);
		D3DXVECTOR3& getRot();
		D3DXVECTOR3& getPos();
		bool getSleeping();
		void setSleeping(bool asleep);

		void setSize(const D3DXVECTOR3& size);
		D3DXVECTOR3 getSize() { return size; }

		NxActor* actor;
		texture::DXTexture* texture;
		D3DXVECTOR3 size;
	};	
}