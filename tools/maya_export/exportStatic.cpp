#include "stdafx.h"
#include "exportStatic.h"
#include "export_dialog.h"
#include "export_options.h"

exportStatic::exportStatic()
{
	printf("[exportStatic::exportStatic] object constructed\n");
}

exportStatic::~exportStatic()
{
	printf("[exportStatic::~exportStatic] object destroyed\n");
	fflush(stdout);
}

void* exportStatic::creator()
{
	return new exportStatic();
}

MStatus exportStatic::doIt( const MArgList& args )
{
	printf("[exportStatic::doit] exportStatic called\n");

	OptionsDialog options_dialog(M3dView::applicationShell());
	if(!options_dialog.show())
		return status;

	MSelectionList selected;
	MGlobal::getActiveSelectionList(selected);
	MItSelectionList iter(selected, MFn::kMesh);
	for(;!iter.isDone(); iter.next()) {
		
		// get dag path and node
		MDagPath path;
		iter.getDagPath(path);
		MFnDagNode node(path);

		printf("[exportStatic::doit] checking node %s\n", path.fullPathName().asChar());

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
		meshsystem.filepath = options.export_path;

		// set dag pathname
		meshsystem.dagPath += (meshsystem.dagPath.size() ? ", " : "") + std::string(path.fullPathName().asChar());
		
		// call our export function
		exportMesh(path);
	}

	meshsystem.export = true;

	ExportDialog dialog(M3dView::applicationShell(), &meshsystem);
	if(!dialog.show())
		return status;

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
		printf("[exportStatic::exportMesh] getting UVs for  %s\n", UVSets[i].asChar());
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
		printf("%i shaders, %i shader indices\n", shaders.length(), polyshader.length());
		for(unsigned i = 0; i < shaders.length(); i++) {			
			shadermap.insert(shaderMap::value_type(i, GetTexture(shaders[i])));
			printf("shader[%i] = %s\n", i, shadermap[i].c_str());
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
				mesh = meshsystem.meshes[i];

		if(mesh == NULL) {
			mesh = new xMesh;
			mesh->material = shadername;
			mesh->filepath = meshsystem.filepath;
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
				int current_indice = mesh->indices.size();

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
					// TODO: check if this vertice already exists and use it if it does
					mesh->vertices.push_back(mesh_vertices[i]);

					// add indice to object
					mesh->indices.push_back(current_indice + i);
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
	fprintf(file, "NAME: %s\n", name.c_str());
	fprintf(file, "MATERIAL: %s\n", material.c_str());
	fprintf(file, "VERTICECOUNT: %i\n", vertices.size());
	fprintf(file, "INDICECOUNT: %i\n", indices.size());
	for(unsigned i = 0; i < vertices.size(); i++) {
		fprintf(file, "VERTEX[%i]: POS: %f, %f, %f NRM: %f, %f, %f COL: %f, %f, %f, %f",
			i,
			vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z,
			vertices[i].nrm.x, vertices[i].nrm.y, vertices[i].nrm.z,
			vertices[i].color.r, vertices[i].color.g, vertices[i].color.b, vertices[i].color.a);
		for(unsigned j = 0; j < vertices[i].uvs.size(); j++) 
			fprintf(file, " UV[%i]: %f, %f", j, vertices[i].uvs[j].x, vertices[i].uvs[j].y);
		fprintf(file, "\n");
	}
	for(unsigned i = 0; i < indices.size(); i++)
		fprintf(file, "INDICE[%i]: %i\n", i, indices[i]);
	fflush(file);
}