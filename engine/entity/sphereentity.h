#pragma once

#include "physics/physics.h"

namespace texture {
	class DXTexture;
	struct Material;
}

namespace entity {
	class SphereEntity : public Entity {
	public:
		SphereEntity(string name, string texture);
		~SphereEntity();
		void acquire();
		void release();
		void render(texture::Material* lighting);
		void calcAABB();
		void update();
		void doTick();
		void applyForce(const D3DXVECTOR3 &force);
		void setPos(const D3DXVECTOR3& pos);
		void setRot(const D3DXVECTOR3& rot);

		void setRadius(const float radius);
		float getRadius() { return radius; }

		D3DXVECTOR3& getRot();
		D3DXVECTOR3& getPos();
		D3DXVECTOR3& getVelocity();

		D3DXVECTOR3 velocity;
		hkpRigidBody* pentity;
		hkpSphereShape* shape;
		texture::DXTexture* texture;
		float radius;
	};	
}