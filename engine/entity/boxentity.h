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
		ENTITY_FLAGS getFlags() { return EF_RENDERABLE | EF_DYNAMIC | EF_HASACTOR | EF_HASEVENTS; }
		void setPos(const D3DXVECTOR3& pos);
		void setRot(const D3DXVECTOR3& rot);
		const D3DXVECTOR3& getRot();
		const D3DXVECTOR3& getPos();
		NxActor* getActor() { return actor; }
		IRenderable* getRenderer() { return &renderer };
		
	private:
		NxActor* actor;
		texture::DXTexture* texture;
		IBoxRenderer renderer;
	};	
}