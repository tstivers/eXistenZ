#include "stdafx.h"
#include "exportStatic.h"
#include "export_dialog.h"
#include "export_options.h"

exportStatic::exportStatic()
{
	unknown_materials = 0;
}

exportStatic::~exportStatic()
{
}

void* exportStatic::creator()
{
	return new exportStatic();
}

MStatus exportStatic::doIt( const MArgList& args )
{
	MSelectionList selected;
	MGlobal::getActiveSelectionList(selected);
	
	if(selected.length() < 1) {
		MessageBox(NULL, "Nothing selected!", "Export Error", MB_OK);
		return status;
	}

	if((selected.length() > 1) && !options.allow_multi) {
		MessageBox(NULL, "More than one object selected!", "Export Error", MB_OK);
		return status;
	}

	MItSelectionList iter(selected, MFn::kMesh);
	for(;!iter.isDone(); iter.next()) {
		
		// get dag path and node
		MDagPath path;
		iter.getDagPath(path);
		MFnDagNode node(path);

		// make sure we can export this object
		if ( path.hasFn(MFn::kTransform)) continue;
		if (!path.hasFn(MFn::kMesh))      continue;
		if (node.isIntermediateObject())  continue;

		// set system name
		std::string filename = path.fullPathName().asChar();
		if(filename.rfind("|"))
			filename = filename.substr(filename.rfind("|") + 1); 
		meshsystem.name = filename;

		// set system path				
		meshsystem.filepath = "";

		// set dag pathname
		meshsystem.dagPath += (meshsystem.dagPath.size() ? ", " : "") + std::string(path.fullPathName().asChar());
		
		// call our export function
		exportMesh(path);
	}

	meshsystem.export = true;

	ExportDialog dialog(M3dView::applicationShell(), &meshsystem);
	if(!dialog.show())
		return status;

	std::string filepath = options.export_path + "\\" + meshsystem.filepath + "\\" + meshsystem.name + MESHSYSTEM_EXTENSION;
	FILE* file;

	if(meshsystem.export) {
		file = fopen(filepath.c_str(), "w");
		meshsystem.dump(file);
		fclose(file);
		printf("Wrote file \"%s\"", filepath.c_str());
	}

	for(unsigned i = 0; i < meshsystem.meshes.size(); i++) {
		if(!meshsystem.meshes[i]->export)
			continue;

		filepath = options.export_path + "\\" + meshsystem.meshes[i]->filepath + "\\" + meshsystem.meshes[i]->name + MESH_EXTENSION;
		FILE* file = fopen(filepath.c_str(), "w");
		meshsystem.meshes[i]->dump(file);
		fclose(file);
		printf("Wrote file \"%s\"", filepath.c_str());
	}

	return status;
}

void exportStatic::exportMesh(const MDagPath& dagPath)
{
	MFnMesh  fnMesh( dagPath );
	xMeshSystem meshsys;
	
	//  Cache positions for each vertex
	MPointArray meshPoints;
	fnMesh.getPoints( meshPoints, MSpace::kObject );

	//  Cache normals for each vertex
	MFloatVectorArray  meshNormals;
	// Normals are per-vertex per-face..
	// use MItMeshPolygon::normalIndex() for index
	fnMesh.getNormals( meshNormals );

	// Get UVSets for this mesh
	MStringArray  UVSets;
	status = fnMesh.getUVSetNames( UVSets );

	uvMap uvmap;
	for(unsigned i = 0; i < UVSets.length(); i++) {		
		uvSet set;
		fnMesh.getUVs( set.u, set.v, &UVSets[i] );
		uvmap.insert(uvMap::value_type(i, set));
	}


	// Get Shaders for this mesh
	MObjectArray shaders;
	MIntArray polyshader;
	shaderMap shadermap;
	status = fnMesh.getConnectedShaders( dagPath.instanceNumber(), shaders, polyshader );

	if(status == MS::kSuccess ) {		
		for(unsigned i = 0; i < shaders.length(); i++) {
			char material[MAX_PATH];
			char strip[MAX_PATH];
			std::string bleh;
			strcpy(strip, options.mat_strip_path.c_str());
			strcpy(material, GetTexture(shaders[i]).c_str());
			if(!strcmp(material, "unknown")) {
				itoa(unknown_materials, &material[strlen(material)], MAX_PATH);
				unknown_materials++;
			}

			if((strlen(strip) < strlen(material)) && !memicmp(strip, material, strlen(strip)))
				bleh = &(material[strlen(strip)]);
			else
				bleh = material;

			shadermap.insert(shaderMap::value_type(i, bleh));
		}
	}

	MItMeshPolygon  itPolygon( dagPath, MObject::kNullObj );
	for (; !itPolygon.isDone(); itPolygon.next() )
	{
		// Get object-relative indices for the vertices in this face.
		MIntArray polygonVertices;
		itPolygon.getVertices( polygonVertices );

		// Get the shader for this face
		std::string shadername;
		unsigned int shaderid = polyshader[itPolygon.index()];
		if(shaderid < 0)
			shadername = "none";
		else
			shadername = shadermap[shaderid];

		// find the mesh which has this shader or make a new mesh
		xMesh* mesh = NULL;
		for(unsigned i = 0; i < meshsystem.meshes.size(); i++)
			if(shadername == meshsystem.meshes[i]->material)
				if(options.combine_meshes)
					mesh = meshsystem.meshes[i];
				else
					if(meshsystem.meshes[i]->dagPath == std::string(dagPath.fullPathName().asChar()))
						mesh = meshsystem.meshes[i];

		if(mesh == NULL) {
			mesh = new xMesh;
			mesh->material = shadername;
			mesh->filepath = meshsystem.filepath;
			// TODO: this don't work right
			mesh->dagPath += (mesh->dagPath.size() ? ", " : "") + std::string(dagPath.fullPathName().asChar());
			mesh->export = true;
			mesh->export_normals = true;
			mesh->export_colors = false;
			mesh->export_uvs = true;
			meshsystem.meshes.push_back(mesh);
		}

		// Get triangulation of this poly.
		int numTriangles;
		itPolygon.numTriangles(numTriangles);

		while ( numTriangles-- )
		{
			MPointArray nonTweaked;
			MIntArray triangleVertices;			
			MIntArray localIndex;

			status = itPolygon.getTriangle( numTriangles, nonTweaked, triangleVertices,	MSpace::kObject );
						
			if ( status == MS::kSuccess )
			{
				xVertex mesh_vertices[3];				

				// Get face-relative vertex indices for this triangle
				localIndex = GetLocalIndex( polygonVertices, triangleVertices );

				for(int i = 0; i < 3; i++) {
					// --------  Get Positions  --------
					mesh_vertices[i].pos.x = (float)meshPoints[triangleVertices[i]].x;
					mesh_vertices[i].pos.y = (float)meshPoints[triangleVertices[i]].y;
					mesh_vertices[i].pos.z = (float)meshPoints[triangleVertices[i]].z;

					// --------  Get Normals  --------
					mesh_vertices[i].nrm.x = meshNormals[itPolygon.normalIndex(localIndex[i])][0];
					mesh_vertices[i].nrm.y = meshNormals[itPolygon.normalIndex(localIndex[i])][1];
					mesh_vertices[i].nrm.z = meshNormals[itPolygon.normalIndex(localIndex[i])][2];

					// --------  Get UVs  --------
					for(unsigned j = 0; j < UVSets.length(); j++) {
						int uvID;

						// Get UV values for each vertex within this polygon
						itPolygon.getUVIndex( localIndex[i], uvID, &UVSets[j] );						

						mesh_vertices[i].uvs.push_back(D3DXVECTOR2(uvmap[j].u[uvID], uvmap[j].v[uvID]));
					}

					// --------  Get Colours  --------
					MColor color;
					itPolygon.getColor( color, localIndex[i] );
					mesh_vertices[i].color.r = color.r;
					mesh_vertices[i].color.g = color.g;
					mesh_vertices[i].color.b = color.b;
					mesh_vertices[i].color.a = color.a;

					// add vertice to object
					if(options.optimize_meshes) {
						bool found = false;
						for(unsigned j = 0; j < mesh->vertices.size(); j++) {
							if(mesh_vertices[i] == mesh->vertices[j]) {
								found = true;
								mesh->indices.push_back(j);
								break;
							}
						}
						
						if(!found) {
							mesh->vertices.push_back(mesh_vertices[i]);
							mesh->indices.push_back(mesh->vertices.size() - 1);
						}
					}
					else {
						mesh->vertices.push_back(mesh_vertices[i]);
						mesh->indices.push_back(mesh->vertices.size() - 1);
					}
				}
			}
		}		
	}
}

MIntArray exportStatic::GetLocalIndex( MIntArray & getVertices, MIntArray & getTriangle )
{
	MIntArray   localIndex;
	unsigned    gv, gt;

	for ( gt = 0; gt < getTriangle.length(); gt++ )
	{
		for ( gv = 0; gv < getVertices.length(); gv++ )
		{
			if ( getTriangle[gt] == getVertices[gv] )
			{
				localIndex.append( gv );
				break;
			}
		}

		// if nothing was added, add default "no match"
		if ( localIndex.length() == gt )
			localIndex.append( -1 );
	}

	return localIndex;
}

std::string exportStatic::GetTexture(MObject& shader)
{
	MFnDependencyNode shadernode(shader);
	MPlug shaderplug = shadernode.findPlug("surfaceShader");
	MPlugArray plugarray;
	shaderplug.connectedTo(plugarray, true, false);
		
	for (unsigned i = 0; i < plugarray.length(); i++) {
		MFnDependencyNode plugnode(plugarray[i].node());
		MItDependencyGraph ItShaderGraph(plugarray[i], 	MFn::kFileTexture, MItDependencyGraph::kUpstream);		
		while (!ItShaderGraph.isDone()) {
			MObject ShaderTexture = ItShaderGraph.thisNode();
			MFnDependencyNode FnTexture(ShaderTexture);

			MString textureFile;
			FnTexture.findPlug("fileTextureName").getValue(textureFile);
			return std::string(textureFile.asChar());
		}
	}

	return std::string("unknown");
}

void xMesh::dump(FILE* file)
{
	std::string verticeformat;
	verticeformat = "D3DFVF_XYZ";
	if(export_normals) verticeformat += " | D3DFVF_NORMAL";
	if(export_colors) verticeformat += " | D3DFVF_DIFFUSE";
	if(export_uvs && vertices[0].uvs.size()) {
		verticeformat += " | D3DFVF_TEX";
		char buf[3];
		verticeformat += itoa(vertices[0].uvs.size(), buf, 3);
		for(unsigned i = 0; i < vertices[0].uvs.size(); i++)
			verticeformat += std::string(" | D3DFVF_TEXCOORDSIZE2(") + itoa(i, buf, 3) + std::string(")");
	}
	fprintf(file, "NAME: %s\n", name.c_str());
	fprintf(file, "DAGPATH: %s\n", dagPath.c_str());
	fprintf(file, "MATERIAL: %s\n", material.c_str());
	fprintf(file, "VERTICEFORMAT: %s\n", verticeformat.c_str());
	fprintf(file, "VERTICECOUNT: %i\n", vertices.size());
	fprintf(file, "INDICECOUNT: %i\n", indices.size());
	for(unsigned i = 0; i < vertices.size(); i++) {
		fprintf(file, "VERTEX[%i]: POS: %f, %f, %f", i,
			vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z);
		
		if(export_normals)
			fprintf(file, " NRM: %f, %f, %f", 
				vertices[i].nrm.x, vertices[i].nrm.y, vertices[i].nrm.z);

		if(export_colors)
			fprintf(file, " COL: %f, %f, %f, %f",
				vertices[i].color.r, vertices[i].color.g, vertices[i].color.b, vertices[i].color.a);

		if(export_uvs && vertices[i].uvs.size())
			for(unsigned j = 0; j < vertices[i].uvs.size(); j++) 
				fprintf(file, " UV[%i]: %f, %f", j, vertices[i].uvs[j].x, vertices[i].uvs[j].y);

		fprintf(file, "\n");
	}

	for(unsigned i = 0; i < indices.size(); i++)
		fprintf(file, "INDICE[%i]: %i\n", i, indices[i]);

	fflush(file);
}

void xMeshSystem::dump(FILE* file)
{
	fprintf(file, "NAME: %s\n", name.c_str());
	fprintf(file, "DAGPATH: %s\n", dagPath.c_str());
	for(unsigned i = 0; i < meshes.size(); i++)
		if(meshes[i]->export)
			fprintf(file, "MESH: %s%s%s%s\n", 
			meshes[i]->filepath.size() ? meshes[i]->filepath.c_str() : "", 
			meshes[i]->filepath.size() ? "\\" : "",
			meshes[i]->name.c_str(), 
			MESH_EXTENSION);
}