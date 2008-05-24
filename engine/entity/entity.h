#pragma once

namespace entity {

	enum SceneType
	{
		SCENE_LEVEL,
		SCENE_MENU,
		SCENE_CUSCENE
	};

	class EntityDesc;
	class ComponentDesc;

	class Scene: boost::noncopyable
	{
	public:
		virtual const string& getName();
		virtual bool acquireResources();
		virtual bool releaseResources();

		virtual Entity& createEntity(const EntityDesc& desc);
	};

	enum ComponentType
	{
		COMP_TRANSFORM,
		COMP_ACTOR,
		COMP_CAMERAMANAGER,
		COMP_CONTROLLER,
		COMP_BSP,
		COMP_MESH,
		COMP_SPHEREMESH,
		COMP_BOXMESH
	};

	class Component : boost::noncopyable
	{
	public:
		virtual const string& getName();
		virtual ComponentType getType();
		virtual Entity& getEntity();
		virtual bool acquireResources();
		virtual bool releaseResources();
	};

	class Entity : boost::noncopyable 
	{
	public:
		virtual const string& getName();
		virtual const JSObject* getScriptObject();
		
		virtual bool acquireResources();
		virtual bool releaseResources();
		virtual bool serializeState();
		virtual uint32 getNbComponents();
		virtual Component& getComponent(const string& name);
		virtual Component& getComponent(uint32);
		virtual Scene& getScene();
	}
};