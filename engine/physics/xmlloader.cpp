#include "precompiled.h"
#include "physics/physics.h"
#include "physics/shapecache.h"
#include "physics/xmlloader.h"
#include "vfs/vfs.h"
#include "rapidxml/rapidxml.hpp"
#include "stream.h"
#include <NxCooking.h>

using namespace rapidxml;
using namespace physics;

namespace physics
{
	extern NxPhysicsSDK* gPhysicsSDK;
	extern NxCookingInterface *gCooking;
	extern NxScene* gScene;
}

namespace xmlloader
{
	typedef map<string, function<bool(xml_node<>*, ShapeEntry)>> parse_map_type;
	parse_map_type parse_map;
	void init();

	NxVec3 parseVector(const string& str);
	NxMat34 parseMatrix(const string& str);

	void parseShapeDesc(xml_node<>* shape, NxShapeDesc* desc);
	NxConvexMesh* parseConvexMeshData(const string& id, xml_document<>* doc);

	bool parseBoxShape(xml_node<>* shape, ShapeEntry sl);
	bool parseConvexShape(xml_node<>* shape, ShapeEntry sl);
}

using namespace xmlloader;
using namespace boost;

REGISTER_STARTUP_FUNCTION(xmlloader, xmlloader::init, 10);

void xmlloader::init()
{
	parse_map["NxBoxShapeDesc"] = parseBoxShape;
	parse_map["NxConvexShapeDesc"] = parseConvexShape;
}

NxVec3 xmlloader::parseVector(const string& str)
{
	vector<string> v;
	split(v, str, is_space());
	assert(v.size() == 3);

	return NxVec3(lexical_cast<double>(v[0]), lexical_cast<double>(v[1]), lexical_cast<double>(v[2]));
}

NxMat34 xmlloader::parseMatrix(const string& str)
{
	vector<string> v;
	split(v, str, is_space(), token_compress_on);
	
	assert(v.size() == 12);

	float values[12];
	for(int i = 0; i < 12; i++)
		values[i] = lexical_cast<float>(v[i]);

	return NxMat34(NxMat33(NxVec3(&values[0]), NxVec3(&values[3]), NxVec3(&values[6])), NxVec3(&values[9]));
}

void xmlloader::parseShapeDesc( xml_node<>* shape, NxShapeDesc* desc)
{
	desc->localPose = parseMatrix(shape->first_node("localPose")->value());
}

bool xmlloader::parseBoxShape(xml_node<>* shape, ShapeEntry sl)
{
	NxBoxShapeDesc* d = new NxBoxShapeDesc();
	d->dimensions = parseVector(shape->first_attribute("dimensions")->value());
	parseShapeDesc(shape->first_node("NxShapeDesc"), d);

	sl->push_back(ShapeList::value_type(d));

	return true;
};

bool xmlloader::parseConvexShape( xml_node<>* shape, ShapeEntry sl )
{
	NxConvexShapeDesc* d = new NxConvexShapeDesc();
	d->meshData = parseConvexMeshData(shape->first_attribute("meshData")->value(), shape->document());
	parseShapeDesc(shape->first_node("NxShapeDesc"), d);
	
	sl->push_back(ShapeList::value_type(d));

	return true;
}

NxConvexMesh* xmlloader::parseConvexMeshData( const string& id, xml_document<>* doc )
{
	xml_node<>* node = doc->first_node()->first_node("NxuPhysicsCollection")->first_node("NxConvexMeshDesc");
	while(node && (string(node->first_attribute("id")->value()) != id))
		node = node->next_sibling("NxConvexMeshDesc");
	assert(node);

	vector<string> pts;
	split(pts, string(node->first_node("points")->value()), is_space(), token_compress_on);
	assert(pts.size() % 3 == 0);

	vector<float> fpts;
	for(int i = 0; i < pts.size(); i++)
		fpts.push_back(lexical_cast<float>(pts[i]));
	
	vector<string> tris;
	split(tris, string(node->first_node("triangles")->value()), is_space(), token_compress_on);
	assert(tris.size() % 3 == 0);

	vector<int> itris;
	for(int i = 0; i < tris.size(); i++)
		itris.push_back(lexical_cast<int>(tris[i]));

	NxConvexMeshDesc md;
	md.numVertices = pts.size() / 3;
	md.points = &fpts.front();
	md.pointStrideBytes = sizeof(float) * 3;
	md.numTriangles = tris.size() / 3;
	md.triangles = &itris.front();
	md.triangleStrideBytes = sizeof(int) * 3;
	md.flags = 0;

	MemoryWriteBuffer mwBuf;

	bool cooked = gCooking->NxCookConvexMesh(md, mwBuf);
	ASSERT(cooked);

	NxConvexMesh* mesh = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(mwBuf.data));
	assert(mesh);

	return mesh;
}

ShapeEntry physics::loadDynamicsXML(string filename)
{
	vfs::File f = vfs::getFile(filename);
	if(!f)
		return ShapeEntry();

	ShapeEntry sl(new ShapeList());

	xml_document<> doc;

	vector<char> buffer;
	buffer.resize(f->size + 1);
	f->read(&buffer.front(), f->size);
	buffer[f->size] = 0;

	doc.parse<parse_trim_whitespace>(&buffer.front());
	
	xml_node<>* shape = doc.first_node()->first_node("NxuPhysicsCollection")->first_node("NxSceneDesc")->first_node("NxActorDesc")->first_node("name")->next_sibling();

	while(shape)
	{
		INFO("found shape: %s id=\"%s\"", shape->name(), shape->first_attribute("id")->value());

		if(parse_map.find(shape->name()) == parse_map.end())
		{
			INFO("ERROR: found unknown shape \"%s\"", shape->name());
			return ShapeEntry();
		}

		if(!parse_map[shape->name()](shape, sl))
		{
			INFO("ERROR: unable to parse shape \"%s\"", shape->name());
			return ShapeEntry();
		}

		shape = shape->next_sibling();
		if(shape && !shape->first_node("NxShapeDesc"))
			shape = NULL;
	}

	return sl;
}