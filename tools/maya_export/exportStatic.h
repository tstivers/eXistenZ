#pragma once

typedef std::vector<D3DXVECTOR2> uvList;

class xVertex {
public:
	D3DXVECTOR3 pos;
	D3DXVECTOR3	nrm;
	D3DXCOLOR	color;
	uvList uvs;

	bool operator==(const xVertex& rhs) {
		return (pos == rhs.pos) &&
			(nrm == rhs.nrm) &&
			(color == rhs.color) &&
			(uvs == rhs.uvs);
	}
};

typedef std::vector<xVertex> xVerticeList;
typedef std::vector<unsigned short> xIndiceList;

class xMesh {
public:
	std::string name;
	std::string dagPath;
	std::string filepath;
	std::string material;	
	xVerticeList vertices;
	xIndiceList indices;

	bool export_normals;
	bool export_colors;
	bool export_uvs;
	bool export;

	void dump(FILE* file);
};

typedef std::vector<xMesh*> xMeshList;

class xMeshSystem {
public:
	std::string name;
	std::string filepath;
	std::string dagPath;
	bool export;
	xMeshList meshes;
	void dump(FILE* file);
};

typedef struct {
	MFloatArray u;
	MFloatArray v;
} uvSet;

typedef std::map<unsigned int, uvSet> uvMap;
typedef std::map<int, std::string> shaderMap;

class exportStatic: public MPxCommand
{
public:
	exportStatic();
	virtual	~exportStatic();
	static void* creator();
	virtual MStatus	doIt(const MArgList& args);

private:
	MStatus status;
	void exportMesh(const MDagPath& dagPath);
	MIntArray GetLocalIndex( MIntArray & getVertices, MIntArray & getTriangle );
	std::string GetTexture(MObject& shader);
	xMeshSystem meshsystem;
	int unknown_materials;
};