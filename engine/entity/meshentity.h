#pragma once

#include "entity/entity.h"
#include "texture/texture.h"
#include "texture/material.h"
#include "physics/physics.h"
#include "mesh/mesh.h"

namespace entity
{
	class MeshEntity : public Entity
	{
	public:
		MeshEntity(string name, string mesh);
		~MeshEntity();
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
		D3DXMATRIX getTransform();
		bool getSleeping();
		void setSleeping(bool asleep);

		NxActor* actor;
		texture::DXTexture* texture;
		mesh::Mesh* mesh;
	};
}