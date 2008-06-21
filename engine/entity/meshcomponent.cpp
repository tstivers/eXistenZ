#include "precompiled.h"
#include "entity/meshcomponent.h"
#include "entity/jsmeshcomponent.h"
#include "entity/entitymanager.h"
#include "scene/scene.h"
#include "render/render.h"
#include "render/shapes.h"
#include "render/rendergroup.h"

using namespace entity;

REGISTER_COMPONENT_TYPE(MeshComponent, 2);

#pragma warning(disable: 4355) // disable warning for using 'this' as an initializer

MeshComponent::MeshComponent(Entity* entity, const string& name, const desc_type& desc)
: Component(entity, name, desc), m_meshName(desc.mesh), transform(this), m_mesh(NULL)
{
	transform = desc.transformComponent;
}

MeshComponent::~MeshComponent()
{
}

void MeshComponent::acquire()
{
	Component::acquire();
	m_mesh = mesh::getMesh(m_meshName);
	m_mesh->acquire();
	m_entity->getManager()->getScene()->addRenderable(this);
}

void MeshComponent::release()
{
	Component::release();
	m_mesh = NULL;
	m_entity->getManager()->getScene()->removeRenderable(this);
}

D3DXVECTOR3 MeshComponent::getRenderOrigin() const
{
	D3DXMATRIX m = m_mesh->mesh_offset * transform->getTransform();
	D3DXVECTOR3 scale, pos;
	D3DXQUATERNION rot;
	D3DXMatrixDecompose(&scale, &rot, &pos, &m);
	return pos;
}

void MeshComponent::render(texture::Material* lighting)
{
	render::RenderGroup* rg = m_mesh->rendergroup;
	rg->material = lighting;
	D3DXMATRIX m = m_mesh->mesh_offset * transform->getTransform();
	render::drawGroup(rg, &m);

	if(render::visualizeFlags & render::VIS_AXIS)
		render::drawAxis(transform->getPos(), transform->getRot());
}

JSObject* MeshComponent::createScriptObject()
{
	return jsentity::createMeshComponentObject(this);
}

void MeshComponent::destroyScriptObject()
{
	jsentity::destroyMeshComponentObject(this);
}
