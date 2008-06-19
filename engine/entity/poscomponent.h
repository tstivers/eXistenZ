#pragma once

#include "entity/entity.h"
#include "entity/component.h"

namespace entity
{
	struct PosComponentDesc : public ComponentDesc
	{
		D3DXVECTOR3 position;
		D3DXVECTOR3 rotation;
		D3DXVECTOR3 scale;
		string parentName;
		PosComponentDesc()
			: position(0.0, 0.0, 0.0), rotation(0.0, 0.0, 0.0), scale(1.0, 1.0, 1.0) {}
	};

	class PosComponent : public Component
	{
	public:
		// typedefs
		typedef PosComponentDesc desc_type;
		typedef function<void(D3DXMATRIX&, const D3DXMATRIX&)> get_set_type;

		// constructor/destructor
		PosComponent(Entity* entity, const string& name, const desc_type& desc);
		~PosComponent();

		// overloads
		int getType() { return 1; }
		void acquire();
		void release();

		// methods
		virtual void setPos(const D3DXVECTOR3& new_pos);
		virtual D3DXVECTOR3 getPos();
		virtual void setRot(const D3DXVECTOR3& new_rot);
		virtual void setRot(const D3DXQUATERNION& new_rot);
		virtual D3DXVECTOR3 getRot();
		virtual D3DXQUATERNION getRotQuat();
		virtual void setScale(const D3DXVECTOR3& new_scale);
		virtual D3DXVECTOR3 getScale();
		virtual void setTransform(const D3DXMATRIX& new_transform);
		virtual D3DXMATRIX getTransform();

		virtual string setParent(const string& name);
		virtual string setParent(PosComponent* parent);
		virtual string getParentName() { return m_parentName; }
		virtual PosComponent* getParent() { return dynamic_cast<PosComponent*>(m_entity->getComponent(m_parentName)); }

		get_set_type setSetFunction(const get_set_type& setter);
		get_set_type setGetFunction(const get_set_type& getter);

	protected:
		// script functions
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members
		D3DXMATRIX m_transform;
		get_set_type m_getter;
		get_set_type m_setter;
		string m_parentName;
		PosComponent* m_parent;
	};
}