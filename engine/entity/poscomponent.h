#pragma once

#include "entity/component.h"

namespace entity
{
	class PosComponent : public Component
	{
	public:
		// typedefs
		typedef function<void(D3DXMATRIX&, const D3DXMATRIX&)> get_set_type;

		// constructor/destructor
		PosComponent(Entity* entity, const string& name);
		~PosComponent();

		// methods
		virtual void setPos(const D3DXVECTOR3& pos);
		virtual D3DXVECTOR3 getPos();
		virtual void setRot(const D3DXVECTOR3& vec);
		virtual void setRot(const D3DXQUATERNION& quat);
		virtual D3DXVECTOR3 getRot();
		virtual D3DXQUATERNION getRotQuat();
		virtual void setScale(const D3DXVECTOR3& scl);
		virtual D3DXVECTOR3 getScale();
		virtual void setTransform(const D3DXMATRIX& m);
		virtual D3DXMATRIX getTransform();

		get_set_type setSetFunction(const get_set_type& setter);
		get_set_type setGetFunction(const get_set_type& getter);

	protected:
		// members
		D3DXMATRIX m_transform;
		get_set_type m_getter;
		get_set_type m_setter;
	}
}