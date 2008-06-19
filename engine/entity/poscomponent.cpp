#include "precompiled.h"
#include "entity/poscomponent.h"
#include "entity/jsposcomponent.h"

using namespace entity;

REGISTER_COMPONENT_TYPE(PosComponent, 1);

PosComponent::PosComponent(Entity* entity, const string& name, const desc_type& desc)
: Component(entity, name, desc)
{
	D3DXMatrixIdentity(&m_transform);
}

PosComponent::~PosComponent()
{

}

// TODO: optimization - store pos/rot/scale so you don't have to decompose on every call
void PosComponent::setPos(const D3DXVECTOR3& new_pos)
{
	D3DXVECTOR3 scale, pos;
	D3DXQUATERNION rot;
	D3DXMatrixDecompose(&scale, &rot, &pos, &getTransform());
	if(m_setter)
	{
		D3DXMATRIX new_transform;
		D3DXMatrixTransformation(&new_transform, NULL, NULL, &scale, NULL, &rot, &new_pos);
		m_setter(m_transform, new_transform);
	}
	else
		D3DXMatrixTransformation(&m_transform, NULL, NULL, &scale, NULL, &rot, &new_pos);
}

D3DXVECTOR3 PosComponent::getPos()
{
	D3DXVECTOR3 scale, pos;
	D3DXQUATERNION rot;
	D3DXMatrixDecompose(&scale, &rot, &pos, &getTransform());
	
	return pos;
}

void PosComponent::setRot(const D3DXVECTOR3& new_rot)
{
	D3DXVECTOR3 scale, pos;
	D3DXQUATERNION rot, new_qrot;
	D3DXMatrixDecompose(&scale, &rot, &pos, &getTransform());
	D3DXQuaternionRotationYawPitchRoll(&new_qrot, new_rot.y, new_rot.x, new_rot.z);

	if(m_setter)
	{
		D3DXMATRIX new_transform;
		D3DXMatrixTransformation(&new_transform, NULL, NULL, &scale, NULL, &new_qrot, &pos);
		m_setter(m_transform, new_transform);
	}
	else
		D3DXMatrixTransformation(&m_transform, NULL, NULL, &scale, NULL, &new_qrot, &pos);
}

void PosComponent::setRot(const D3DXQUATERNION& new_qrot)
{
	D3DXVECTOR3 scale, pos;
	D3DXQUATERNION rot;
	D3DXMatrixDecompose(&scale, &rot, &pos, &getTransform());

	if(m_setter)
	{
		D3DXMATRIX new_transform;
		D3DXMatrixTransformation(&new_transform, NULL, NULL, &scale, NULL, &new_qrot, &pos);
		m_setter(m_transform, new_transform);
	}
	else
		D3DXMatrixTransformation(&m_transform, NULL, NULL, &scale, NULL, &new_qrot, &pos);
}

D3DXVECTOR3 PosComponent::getRot()
{
	D3DXVECTOR3 rot;
	MatrixToYawPitchRoll(&getTransform(), &rot);
	return rot;
}

D3DXQUATERNION PosComponent::getRotQuat()
{
	D3DXVECTOR3 scale, pos;
	D3DXQUATERNION rot;
	D3DXMatrixDecompose(&scale, &rot, &pos, &getTransform());

	return rot;
}

void PosComponent::setScale(const D3DXVECTOR3& new_scale)
{
	D3DXVECTOR3 scale, pos;
	D3DXQUATERNION rot;
	D3DXMatrixDecompose(&scale, &rot, &pos, &getTransform());
	if(m_setter)
	{
		D3DXMATRIX new_transform;
		D3DXMatrixTransformation(&new_transform, NULL, NULL, &new_scale, NULL, &rot, &pos);
		m_setter(m_transform, new_transform);
	}
	else
		D3DXMatrixTransformation(&m_transform, NULL, NULL, &new_scale, NULL, &rot, &pos);
}

D3DXVECTOR3 PosComponent::getScale()
{
	D3DXVECTOR3 scale, pos;
	D3DXQUATERNION rot;
	D3DXMatrixDecompose(&scale, &rot, &pos, &getTransform());

	return scale;
}

void PosComponent::setTransform(const D3DXMATRIX& new_transform)
{
	if(m_setter)
	{
		//getTransform(); // maybe update the current transform?
		m_setter(m_transform, new_transform);
	}
	else
		m_transform = new_transform;
}

const D3DXMATRIX& PosComponent::getTransform()
{
	if(m_getter)
	{
		D3DXMATRIX new_transform;
		m_getter(new_transform, m_transform);
		m_transform = new_transform;
	}

	return m_transform;
}

PosComponent::get_set_type PosComponent::setGetFunction(const entity::PosComponent::get_set_type &getter)
{
	get_set_type old_getter = m_getter;
	m_getter = getter;
	return old_getter;
}

PosComponent::get_set_type PosComponent::setSetFunction(const entity::PosComponent::get_set_type &setter)
{
	get_set_type old_setter = m_setter;
	m_setter = setter;
	return old_setter;
}

JSObject* PosComponent::createScriptObject()
{
	return jsentity::createPosComponentObject(this);
}

void PosComponent::destroyScriptObject()
{
	jsentity::destroyPosComponentObject(this);
}