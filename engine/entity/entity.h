#pragma once

namespace entity {
	enum ENTITY_FLAGS {		
		EF_RENDERABLE =		1<<1,	// entity can be rendered		
		EF_TICK =			1<<3,	// needs to be called every frame
		EF_DYNAMIC = 		1<<5,	// can change position
		EF_HASACTOR =		1<<6,	// has physics backing
		EF_HASEVENTS = 		1<<2,	// supports events
		EF_SUPPORTSINPUT = 	1<<7,	// can accept user input		
		EF_END = 0xffff
	};

	enum ENTITY_TYPE {
		ET_BOX,
		ET_PLAYER,
		ET_CAMERA,
		ET_TRIGGER,
		ET_END = 0xffff
	};

	class Entity {
	public:
		Entity(std::string name);
		virtual ~Entity();
		virtual bool acquire();
		virtual bool release();
		virtual ENTITY_FLAGS getFlags();
		virtual bool setFlag(ENTITY_FLAGS flag, bool set);
		ENTITY_TYPE getType() { return type; }		
		virtual const D3DXVECTOR3& getPos() { return pos; }
		virtual const D3DXVECTOR3& getRot() { return rot; }
		virtual bool setPos(const D3DXVECTOR3& pos) { this->pos = pos; }
		virtual bool setRot(const D3DXVECTOR3& rot) { this->rot = rot; }
		virtual bool doEvent(const Event& e);
		virtual NxActor* getActor() { return NULL; }
		virtual IRenderable* getRenderer() { return NULL; }
		virtual IInputHandler* getInputHandler() { return NULL; }
		virtual IEntityEventHandler* getEventHandler() { return NULL; }
		
		D3DXVECTOR3 pos, rot;
		
	private:
		std::string name;
		ENTITY_TYPE type;
		unsigned int flags;		
	};
};