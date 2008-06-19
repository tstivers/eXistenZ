#pragma once

#include "entity/component.h"

namespace entity
{
	struct PosComponentDesc : public ComponentDesc
	{
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
		virtual const D3DXMATRIX& getTransform();

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
	};
}