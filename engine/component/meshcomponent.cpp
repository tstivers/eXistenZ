#include "precompiled.h"
#include "component/meshcomponent.h"
#include "component/jscomponent.h"
#include "entity/entitymanager.h"
#include "scene/scene.h"
#include "render/render.h"
#include "render/shapes.h"
#include "render/rendergroup.h"
#include "mesh/meshcache.h"

using namespace component;

REGISTER_COMPONENT_TYPE(MeshComponent, 2);

#pragma warning(disable: 4355) // disable warning for using 'this' as an initializer

MeshComponent::MeshComponent(entity::Entity* entity, const string& name, const desc_type& desc)
: Component(entity, name, desc), m_meshName(desc.mesh), transform(this), m_mesh(NULL)
{
	transform = desc.transformComponent;
}

MeshComponent::~MeshComponent()
{
	if(m_acquired)
		release();

	if(m_scriptObject)
		destroyScriptObject();
}

void MeshComponent::acquire()
{
	if(m_acquired)
		return;

	ASSERT(m_mesh == NULL);

	m_mesh = mesh::getMesh(m_meshName);
	if(!m_mesh)
	{
		INFO("WARNING: unable to acquire mesh \"%s\" on component \"%s.%s\"", m_meshName.c_str(),
			m_entity->getName().c_str(), m_name.c_str());
		return;
	}

	if(!transform)
	{
		INFO("WARNING: unable to acquire transform for \"%s.%s\"",
			m_entity->getName().c_str(), m_name.c_str());
		m_mesh = NULL;
		return;
	}

	m_mesh->acquire();
	m_entity->getManager()->getScene()->addRenderable(this);
	Component::acquire();
}

void MeshComponent::release()
{
	Component::release();
	transform.release();
	m_mesh = NULL;
	m_entity->getManager()->getScene()->removeRenderable(this);
}

D3DXVECTOR3 MeshComponent::getRenderOrigin() const
{
	ASSERT(transform);
	D3DXMATRIX m = m_mesh->mesh_offset * transform->getTransform();
	D3DXVECTOR3 scale, pos;
	D3DXQUATERNION rot;
	D3DXMatrixDecompose(&scale, &rot, &pos, &m);
	return pos;
}

void MeshComponent::render(texture::Material* lighting)
{
	ASSERT(m_acquired);
	ASSERT(transform);
	render::RenderGroup* rg = m_mesh->rendergroup;
	rg->material = lighting;
	D3DXMATRIX m = m_mesh->mesh_offset * transform->getTransform();
	render::drawGroup(rg, &m);

	if(render::visualizeFlags & render::VIS_AXIS)
		render::drawAxis(transform->getPos(), transform->getRot());
}

JSObject* MeshComponent::createScriptObject()
{
	return jscomponent::createComponentScriptObject(this);
}

void MeshComponent::destroyScriptObject()
{
	jscomponent::destroyComponentScriptObject(this);
	m_scriptObject = NULL;
}

