#pragma once

#include "math/aabb.h"
#include "entity/entitycache.h"

namespace mesh {
	class MeshSystem;
};

namespace entity {
	enum ENTITY_FLAG {
		EF_MESH = 1<<1,
		EF_DRAW = 1<<2,
		EF_COLLIDE = 1<<3,
		EF_THINKS = 1<<4,
		EF_MOBILE = 1<<5,
		EF_END = 0xffff
	};

	enum ENTITY_TYPE {
		ET_STATIC,
		ET_TRIGGER,
		ET_END = 0xffff
	};

	class Entity {
	public:
		Entity(std::string name);
		virtual ~Entity() = 0;
		virtual void acquire() = 0;
		virtual void release() = 0;
		virtual D3DXVECTOR3 getPos() { return pos; };
		virtual D3DXVECTOR3 getRot() { return rot; };
		virtual D3DXVECTOR3 getScale() { return scale; };
		virtual D3DXMATRIX getTransform() { return transform; };
		virtual void setPos(const D3DXVECTOR3& pos) { this->pos = pos; };
		virtual void setRot(const D3DXVECTOR3& rot) { this->rot = rot; };
		virtual void setScale(const D3DXVECTOR3& scale) { this->scale = scale; };
		virtual void setTransform(const D3DXMATRIX& transform) { this->transform = transform; };
		virtual void activate();
		virtual void deactivate();
		virtual void render() = 0;
		virtual void update();
		
		std::string name;
		ENTITY_TYPE type;
		unsigned int flags;
		bool active;
		bool draw;
		AABB aabb;
		D3DXVECTOR3 pos, rot, scale;
		D3DXMATRIX transform;
	};

	class StaticEntity : public Entity {
	public:
		StaticEntity(std::string name);
		~StaticEntity();
		void acquire();
		void release();
		void render();

		static StaticEntity* create(std::string name, std::string meshname);

		std::string meshname;
		mesh::MeshSystem* meshsys;
	};
};