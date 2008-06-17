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
	NxMat34 axis_convert;
}

namespace xmlloader
{
	typedef map<string, function<bool(xml_node<>*, ShapeEntry)>> parse_map_type;
	parse_map_type parse_map;
	void init();

	byte parseNibble(const char c);
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
	desc->localPose = axis_convert * parseMatrix(shape->first_node("localPose")->value());
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

	MemoryWriteBuffer mwBuf;

	if(node->first_node("cookedDataSize"))
	{
		int size = lexical_cast<int>(node->first_node("cookedDataSize")->value());
		string data = node->first_node("cookedData")->value();

		bool havenibble = false;
		byte b = 0;
		for(string::const_iterator it = data.begin(); it != data.end(); it++)
		{
			if((*it >= '0' && *it <= '9') || (*it >= 'A' && *it <= 'F'))
				if(havenibble)
				{
					mwBuf.storeByte(b + parseNibble(*it));
					havenibble = false;
				}
				else
				{
					b = parseNibble(*it) << 4;
					havenibble = true;
				}
		}
		assert(havenibble == false);
		assert(mwBuf.currentSize == size);
	}
	else
	{
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

		bool cooked = gCooking->NxCookConvexMesh(md, mwBuf);
		ASSERT(cooked);
	}

	NxConvexMesh* mesh = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(mwBuf.data));
	assert(mesh);

	//MemoryWriteBuffer buf2;
	//if(NxScaleCookedConvexMesh(MemoryReadBuffer(mwBuf.data), 0.080693, buf2)) //Resize the mesh by a factor 0.5
	//{
	//	NxConvexShapeDesc convexShapeDesc2;
	//	mesh = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf2.data));
	//}	

	return mesh;
}

byte xmlloader::parseNibble( const char c )
{
	if(c >= '0' && c <= '9')
		return c - '0';
	else
		return 10 + c - 'A';
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
	xml_node<>* scene = doc.first_node()->first_node("NxuPhysicsCollection")->first_node("NxSceneDesc");
	xml_node<>* axis = scene->first_node("upAxis");
	NxMat33 rot;
	rot.id();
	NxVec3 trans(0.0, 0.0, 0.0);
	if(axis)
	{
		switch(lexical_cast<int>(axis->value()))
		{
		case 0: // z-axis? whee
			rot.rotX(D3DXToRadian(-90.0));
			break;
		default:
			INFO("ERROR: unknown up axis in dynamics file");
			break;
		}
	}
	axis_convert = NxMat34(rot, trans);
	xml_node<>* shape = scene->first_node("NxActorDesc")->first_node();

	while(shape)
	{
		if(parse_map.find(shape->name()) != parse_map.end())
		{
			if(!parse_map[shape->name()](shape, sl))
			{
				INFO("ERROR: unable to parse shape \"%s\"", shape->name());
				return ShapeEntry();
			}
			INFO("parsed shape %s", shape->name());
		}

		shape = shape->next_sibling();
	}

	if(sl->size() == 0)
	{
		INFO("ERROR: no shapes found");
		return ShapeEntry();
	}

	return sl;
}