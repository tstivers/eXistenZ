#pragma once

namespace entity
{
	class Entity;

	Entity* addStaticEntity(string& name, string& meshsys, D3DXMATRIX* transform = NULL);
	Entity* addBoxEntity(string& name, string& texture, D3DXMATRIX* transform = NULL);
	Entity* addSphereEntity(string& name, string& texture, D3DXMATRIX* transform = NULL);

	void removeEntity(Entity* entity);
	Entity* getEntity(string& name);
	void clearEntities();
}