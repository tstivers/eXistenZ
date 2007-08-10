#pragma once

namespace entity {
	class Entity;

	Entity* addStaticEntity(std::string& name, std::string& meshsys, D3DXMATRIX* transform = NULL);
	Entity* addBoxEntity(std::string& name, std::string& texture, D3DXMATRIX* transform = NULL);
	Entity* addSphereEntity(std::string& name, std::string& texture, D3DXMATRIX* transform = NULL);

	void delEntity(std::string& name);
	Entity* getEntity(std::string& name);	
	void clearEntities();
}