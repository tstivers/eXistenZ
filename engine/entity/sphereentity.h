#pragma once

class NxActor;

namespace texture
{
	class DXTexture;
	struct Material;
}

namespace entity
{
	class SphereEntity : public Entity
	{
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
		void setVelocity(const D3DXVECTOR3& velocity);
		D3DXVECTOR3 getVelocity();

		NxActor* actor;
		texture::DXTexture* texture;
		float radius;
	};
}