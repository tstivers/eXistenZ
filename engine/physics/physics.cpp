#include "precompiled.h"
#include "physics/physics.h"
#include "settings/settings.h"
#include "timer/timer.h"
#include "render/render.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "render/rendergroup.h"
#include "texture/texture.h"
#include "physics/hkOneFixedMoppUtil.h"

namespace physics 
{
	int debug = 0;
	float scale = 1.0;
	float gravity = -9.8;
	float maxtimestep = 1.0 / 60.0;
	int maxiter = 16;
	bool acquired = false;

	hkpWorld* m_world;
	hkMemory* m_memory;
	

	bool setGravity(settings::Setting* setting, void* value);
	bool setTimestep(settings::Setting* setting, void* value);
	bool setMaxIter(settings::Setting* setting, void* value);

	void reportError(const char* message, void* user);
}

using namespace physics;

REGISTER_STARTUP_FUNCTION(physics, physics::init, 10);

void physics::init() 
{
	settings::addsetting("system.physics.debug", settings::TYPE_INT, 0, NULL, NULL, &physics::debug);
	settings::addsetting("system.physics.scale", settings::TYPE_FLOAT, 0, NULL, NULL, &physics::scale);
	settings::addsetting("system.physics.gravity", settings::TYPE_FLOAT, 0, setGravity, NULL, &physics::gravity);
	settings::addsetting("system.physics.maxtimestep", settings::TYPE_FLOAT, 0, setTimestep, NULL, &physics::maxtimestep);
	settings::addsetting("system.physics.maxiter", settings::TYPE_INT, 0, setMaxIter, NULL, &physics::maxiter);

	hkPoolMemory* memoryManager = new hkPoolMemory();
	hkThreadMemory* threadMemory = new hkThreadMemory(memoryManager, 16);
	hkBaseSystem::init( memoryManager, threadMemory, reportError );

	char* stackBuffer;
	{
		int stackSize = 1024 * 1024 * 12;
		stackBuffer = hkAllocate<char>( stackSize, HK_MEMORY_CLASS_BASE);
		hkThreadMemory::getInstance().setStackArea( stackBuffer, stackSize);
	}

	{
		hkMemory::getInstance().preAllocateRuntimeBlock(512000, HK_MEMORY_CLASS_BASE);
		hkMemory::getInstance().preAllocateRuntimeBlock(256000, HK_MEMORY_CLASS_BASE);
		hkMemory::getInstance().preAllocateRuntimeBlock(128000, HK_MEMORY_CLASS_BASE);
		hkMemory::getInstance().preAllocateRuntimeBlock(64000, HK_MEMORY_CLASS_BASE);
		hkMemory::getInstance().preAllocateRuntimeBlock(32000, HK_MEMORY_CLASS_BASE);
		hkMemory::getInstance().preAllocateRuntimeBlock(16000, HK_MEMORY_CLASS_BASE);
		hkMemory::getInstance().preAllocateRuntimeBlock(16000, HK_MEMORY_CLASS_BASE);
	}
}

bool physics::setGravity(settings::Setting* setting, void* value)
{
	settings::float_setter(setting, value);
	return true;
}

void physics::acquire() 
{
	if(acquired)
		return;

	hkpWorldCinfo info;
	info.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_DISCRETE;
	info.m_gravity.set( 0,-9.8f,0);
	info.m_collisionTolerance = 0.1f; 
	info.setBroadPhaseWorldSize( 150.0f );
	info.setupSolverInfo( hkpWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM );
	info.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_DO_NOTHING;


	m_world = new hkpWorld( info );

	hkpAgentRegisterUtil::registerAllAgents( m_world->getCollisionDispatcher() );

	acquired = true;

	startSimulation(); // prime the pump
}

void physics::startSimulation() 
{
	if(!acquired)
		return;

	int iter = 0;

	static float last_step = 0.0;
	while ((timer::game_ms >= last_step + (maxtimestep * 1000.0)) && (iter < maxiter))
	{
		last_step += (maxtimestep * 1000.0);
		m_world->stepDeltaTime(maxtimestep);
		iter++;
	}
}

void physics::getResults() 
{
	if(!acquired)
		return;
}

void physics::release() 
{
	if(!acquired)
		return;

	delete m_world;
	m_world = NULL;

	acquired = false;
}

void physics::addStaticMesh(string name, scene::SceneBSP* scene) 
{
	hkpExtendedMeshShape* mesh = new hkpExtendedMeshShape();
	hkpExtendedMeshShape::TrianglesSubpart part;

	static vector<D3DXVECTOR3> vertices;
	static vector<short> indices;

	for(int i = 0; i < scene->num_faces; i++) 
	{
		if(scene->faces[i].type != 1 && scene->faces[i].type != 3)
			continue;

		if(!scene->faces[i].rendergroup)
			continue;

		if(scene->faces[i].rendergroup->texture->is_transparent)
			continue;

		if(!scene->faces[i].rendergroup->texture->draw)
			continue;

		unsigned int offset = vertices.size();
		for(int j = 0;j < scene->faces[i].num_vertices; j++)
			vertices.push_back(scene->faces[i].vertices[j].pos / physics::scale);

		for(int j = 0; j < scene->faces[i].num_indices; j++)
			indices.push_back(scene->faces[i].indices[j] + offset);
	}

	part.m_vertexBase = (float*)&vertices[0];
	part.m_vertexStriding = sizeof(D3DXVECTOR3);
	part.m_numVertices = vertices.size();

	part.m_indexBase = (void*)&indices[0];
	part.m_indexStriding = 3 * sizeof(hkUint16);
	part.m_stridingType = hkpExtendedMeshShape::INDICES_INT16;
	part.m_numTriangleShapes = indices.size() / 3;

	mesh->addTrianglesSubpart(part);

	hkpMoppCompilerInput mci;
	hkpMoppCode* code = hkpMoppUtility::buildCode( mesh , mci);
	hkpMoppBvTreeShape* moppShape = new hkpMoppBvTreeShape(mesh, code);
	code->removeReference();
	
	hkpRigidBodyCinfo rbCi;
	rbCi.m_shape = moppShape;
	rbCi.m_motionType = hkpMotion::MOTION_FIXED;

	hkpRigidBody* rb = new hkpRigidBody(rbCi);
	rb->setName(name.c_str());
	m_world->addEntity(rb);
}

bool physics::setTimestep( settings::Setting* setting, void* value )
{
	settings::float_setter(setting, value);
	return true;
}

bool physics::setMaxIter( settings::Setting* setting, void* value )
{
	settings::int_setter(setting, value);
	return true;
}

void physics::reportError( const char* message, void* user )
{
	INFO(message);
}

World* physics::getWorld()
{
	return m_world;
}