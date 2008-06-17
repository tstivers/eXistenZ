#pragma once

#include "entity/component.h"

namespace entity
{
	class PosComponent : public Component
	{
	public:
		PosComponent(Entity* entity, const string& name);
		~PosComponent();

		virtual void setPos(const D3DXVECTOR3& pos);
		virtual D3DXVECTOR3 getPos();
		virtual void setRot(const D3DXVECTOR3& vec);
		virtual void setRot(const D3DXQUATERNION& quat);
		virtual D3DXVECTOR3 getRot();
		virtual D3DXQUATERNION getRotQuat();
		virtual void setTransform(const D3DXMATRIX& m);
		virtual D3DXMATRIX getTransform();

	protected:
		D3DXMATRIX m_transform;
	}
}