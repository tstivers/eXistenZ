#include "precompiled.h"
#include "entity/poscomponent.h"
#include "entity/jscomponent.h"

using namespace entity;

REGISTER_COMPONENT_TYPE(PosComponent, 1);

#pragma warning(disable: 4355) // disable warning for using 'this' as an initializer

PosComponent::PosComponent(Entity* entity, const string& name, const desc_type& desc)
: Component(entity, name, desc), parent(this)
{
	D3DXQUATERNION q;
	D3DXQuaternionRotationYawPitchRoll(&q, D3DXToRadian(desc.rotation.y), D3DXToRadian(desc.rotation.x), D3DXToRadian(desc.rotation.z));
	D3DXMatrixTransformation(&m_transform, NULL, NULL, &desc.scale, NULL, &q, &desc.position);
	parent = desc.parentName;
}

PosComponent::~PosComponent()
{
	if(m_scriptObject)
		destroyScriptObject();
}

void PosComponent::acquire()
{
	Component::acquire();
}

void PosComponent::release()
{
	Component::release();
	parent.release();
}

// TODO: optimization - store pos/rot/scale so you don't have to decompose on every call
void PosComponent::setPos(const D3DXVECTOR3& new_pos)
{
	if(parent)
	{
		INFO("WARNING: tried to set position on an acquired child PosComponent");
		return;
	}

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
	if(parent)
	{
		INFO("WARNING: tried to set rotation on an acquired child PosComponent");
		return;
	}

	D3DXVECTOR3 scale, pos;
	D3DXQUATERNION rot, new_qrot;
	D3DXMatrixDecompose(&scale, &rot, &pos, &getTransform());
	D3DXQuaternionRotationYawPitchRoll(&new_qrot, D3DXToRadian(new_rot.y), D3DXToRadian(new_rot.x), D3DXToRadian(new_rot.z));

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
	if(parent)
	{
		INFO("WARNING: tried to set rotation on an acquired child PosComponent");
		return;
	}

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
	if(parent)
	{
		INFO("WARNING: tried to set scale on an acquired child PosComponent");
		return;
	}

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
	if(parent)
	{
		INFO("WARNING: tried to set transform on an acquired child PosComponent");
		return;
	}

	if(m_setter)
	{
		//getTransform(); // maybe update the current transform?
		m_setter(m_transform, new_transform);
	}
	else
		m_transform = new_transform;
}

D3DXMATRIX PosComponent::getTransform()
{
	if(m_getter)
	{
		D3DXMATRIX new_transform;
		m_getter(new_transform, m_transform);
		m_transform = new_transform;
	}

	if(parent)
	{
		return m_transform * parent->getTransform();
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
	return jsentity::createComponentScriptObject(this);
}

void PosComponent::destroyScriptObject()
{
	jsentity::destroyComponentScriptObject(this);
	m_scriptObject = NULL;
}