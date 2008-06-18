#include "precompiled.h"
#include "mesh/fbxloader.h"
#include "mesh/mesh.h"
#include "mesh/meshcache.h"
#include "vfs/vfs.h"
#include "math/vertex.h"
#include "texture/texturecache.h"
#include <fbxsdk.h>
#include <kfbxio/kfbxstreamoptionsfbx.h>
#include <kfbxio/kfbxiosettings.h>
#include <kfbxio/kfbximporter.h>
#include <kfbxplugins/kfbxsdkmanager.h>
#include <kfbxplugins/kfbxscene.h>
#include <fbxfilesdk_nsuse.h>

namespace mesh
{
	KFbxSdkManager* manager = NULL;
	KFbxScene* scene = NULL;

	bool LoadScene(KFbxSdkManager* pSdkManager, KFbxScene* pScene, const char* pFilename);
	Mesh* extractMesh(KFbxNode* node, const string& filename, const string& meshname);
	D3DXMATRIX conv_mat;
}
using namespace mesh;

Mesh* mesh::loadFBXMesh(const string& name)
{
	Mesh* ret = NULL;

	std::vector<std::string> bleh;
	boost::split(bleh, name, boost::is_any_of("#|"));
	ASSERT(bleh.size() == 2);

	string filename = bleh[0];
	string meshname = bleh[1];

	vfs::File f = vfs::getFile(filename.c_str());
	if (!f)
		return NULL;

	manager = KFbxSdkManager::Create();
	scene = KFbxScene::Create(manager, filename.c_str());


	if(LoadScene(manager, scene, f->filename))
	{
		KFbxNode* node = scene->GetRootNode();

		int sign;
		D3DXVECTOR3 conv_rot;
		KFbxAxisSystem::eUpVector up = scene->GetGlobalSettings().GetAxisSystem().GetUpVector(sign);
		switch(up)
		{
		case KFbxAxisSystem::XAxis:
			conv_rot = D3DXVECTOR3(0.0, 0.0, 0.0);
			break;
		case KFbxAxisSystem::YAxis:
			conv_rot = D3DXVECTOR3(0.0, 0.0, 0.0);
			break;
		case KFbxAxisSystem::ZAxis:
			conv_rot = D3DXVECTOR3(-90.0, 0.0, 0.0);
			break;
		}

		D3DXMatrixRotationYawPitchRoll(&conv_mat, D3DXToRadian(conv_rot.y), D3DXToRadian(conv_rot.x), D3DXToRadian(conv_rot.z));

		INFO("up vector: %s (%i, %i)", up == KFbxAxisSystem::XAxis ? "XAxis" : (up == KFbxAxisSystem::YAxis ? "YAxis" : "ZAxis"), up, sign);

		if (node)
			ret = extractMesh(node, filename, meshname);
	}

	manager->Destroy();
	return ret;
}

#pragma region [LoadScene]
// Creates an importer object, and uses it to
// import a file into a scene.
bool mesh::LoadScene(
	KFbxSdkManager* pSdkManager,  // Use this memory manager...
	KFbxScene* pScene,            // to import into this scene
	const char* pFilename         // the data from this file.
)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;
	int lFileFormat = -1;
	int i, lTakeCount;
	KString lCurrentTakeName;
	bool lStatus;
	char lPassword[1024];

	// Use memory manager to create an object
	// to store the options for the import of FBX files only.
	KFbxStreamOptionsFbxReader* lImportOptions =
		KFbxStreamOptionsFbxReader::Create(pSdkManager, "");

	// Get the version number of the FBX files generated by the
	// version of FBX SDK that you are using.
	KFbxIO::GetCurrentVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	KFbxImporter* lImporter = KFbxImporter::Create(pSdkManager, "");

	// Detect the file format of the file to be imported
	if (!pSdkManager->GetIOPluginRegistry()->DetectFileFormat(
				pFilename,
				lFileFormat
			))
	{
		// Unrecognizable file format.
		// Try to fall back to SDK's native file format (an FBX binary file).
		lFileFormat =
			pSdkManager->GetIOPluginRegistry()->GetNativeReaderFormat();
	}
	lImporter->SetFileFormat(lFileFormat);

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename);

	// Get the version number of the FBX file format.
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus)   // Problem with the file to be imported
	{
		INFO("Call to KFbxImporter::Initialize() failed.");
		INFO("Error returned: %s", lImporter->GetLastErrorString());

		if (lImporter->GetLastErrorID() ==
				KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
				lImporter->GetLastErrorID() ==
				KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
		{
			INFO("FBX version number for this FBX SDK is %d.%d.%d",
				 lSDKMajor, lSDKMinor, lSDKRevision);
			INFO("FBX version number for file %s is %d.%d.%d",
				 pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	INFO("FBX version number for this FBX SDK is %d.%d.%d",
		 lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		INFO("FBX version number for file %s is %d.%d.%d",
			 pFilename, lFileMajor, lFileMinor, lFileRevision);

		// In FBX, a scene can have one or more "takes". A take is a
		// container for animation data.
		// You can access a file's take information without
		// the overhead of loading the entire file into the scene.

		INFO("Take Information");

		lTakeCount = lImporter->GetTakeCount();

		INFO("    Number of takes: %d", lTakeCount);
		INFO("    Current take: \"%s\"",
			 lImporter->GetCurrentTakeName());

		for (i = 0; i < lTakeCount; i++)
		{
			KFbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			INFO("    Take %d", i);
			INFO("         Name: \"%s\"", lTakeInfo->mName.Buffer());
			INFO("         Description: \"%s\"",
				 lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the take should
			// be imported under a different name.
			INFO("         Import Name: \"%s\"", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false
			// if the take should be not be imported.
			INFO("         Import State: %s", lTakeInfo->mSelect ? "true" : "false");
		}

		// Import options determine what kind of data is to be imported.
		// The default is true, but here we set the options explictly.
		lImportOptions->SetOption(KFBXSTREAMOPT_FBX_MATERIAL,        true);
		lImportOptions->SetOption(KFBXSTREAMOPT_FBX_TEXTURE,         true);
		lImportOptions->SetOption(KFBXSTREAMOPT_FBX_LINK,            true);
		lImportOptions->SetOption(KFBXSTREAMOPT_FBX_SHAPE,           true);
		lImportOptions->SetOption(KFBXSTREAMOPT_FBX_GOBO,            true);
		lImportOptions->SetOption(KFBXSTREAMOPT_FBX_ANIMATION,       true);
		lImportOptions->SetOption(KFBXSTREAMOPT_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene, lImportOptions);

	// Destroy the importer.
	if (lImportOptions)
	{
		lImportOptions->Destroy();
		lImportOptions = NULL;
	}

	// Destroy the importer
	lImporter->Destroy();

	return lStatus;
}
#pragma endregion

Mesh* mesh::extractMesh(KFbxNode* node, const string& filename, const string& meshname)
{
	Mesh* ret = NULL;
	string name = node->GetName();
	KFbxVector4 rotation, translation, scale;

	node->GetDefaultR(rotation);
	node->GetDefaultT(translation);
	node->GetDefaultS(scale);

	INFO("found node %s", name.c_str());
	INFO("node rot:   %f, %f, %f", rotation[0], rotation[1], rotation[2]);
	INFO("node pos:   %f, %f, %f", translation[0], translation[1], translation[2]);
	INFO("node scale: %f, %f, %f", scale[0], scale[1], scale[2]);

	if (node->GetNodeAttribute() == NULL)
	{
	}
	else
	{
		double convscale = scene->GetGlobalSettings().GetSystemUnit().GetConversionFactorTo(KFbxSystemUnit::m);

		if (node->GetNodeAttribute()->GetAttributeType() == KFbxNodeAttribute::eMESH)
		{

			KFbxMesh* fbxmesh = (KFbxMesh*)node->GetNodeAttribute();

			if (!fbxmesh->IsTriangleMesh())
			{
				KFbxGeometryConverter c(manager);
				fbxmesh = c.TriangulateMesh(fbxmesh);
				INFO("triangulated the mesh");
			}

			D3DXQUATERNION drot;
			D3DXQuaternionRotationYawPitchRoll(&drot, D3DXToRadian(rotation[1]), D3DXToRadian(rotation[0]), D3DXToRadian(rotation[2]));
			D3DXVECTOR3 dpos(translation[0], translation[1], translation[2]);
			D3DXVECTOR3 dscl(scale[0], scale[1], scale[2]);
			D3DXMATRIX mat;
			D3DXMatrixTransformation(&mat, NULL, NULL, &dscl, NULL, &drot, &dpos);

			Mesh* mesh = new Mesh();
			mesh->name = filename + "#" + name;
			mesh->prim_type = D3DPT_TRIANGLELIST;
			mesh->poly_count = fbxmesh->GetPolygonCount();
			mesh->vertice_count = mesh->poly_count * 3;
			mesh->vertice_format = STDVertex::FVF;
			mesh->vertices = new STDVertex[mesh->vertice_count];
			mesh->indice_count = fbxmesh->GetPolygonVertexCount();
			mesh->indices = new unsigned short[mesh->indice_count];
			mesh->mesh_offset = mat * conv_mat;

			STDVertex* vertices = (STDVertex*)mesh->vertices;
			INFO("loading %i vertices", mesh->vertice_count);

			KFbxLayerElementArrayTemplate<KFbxVector2>* mesh_uvs;
			fbxmesh->GetTextureUV(&mesh_uvs);

			KFbxTexture* lTexture = KFbxCast<KFbxTexture>(node->GetMaterial(0)->FindProperty(KFbxSurfaceMaterial::sDiffuse).GetSrcObject(KFbxTexture::ClassId, 0));

			mesh->texture_name = lTexture->GetFileName();

			for (int i = 0; i < fbxmesh->GetPolygonCount(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					STDVertex& v = vertices[(i * 3) + j];
					int index = fbxmesh->GetPolygonVertex(i, j);
					KFbxVector4 mesh_pos = fbxmesh->GetControlPoints()[index];
					KFbxVector4 mesh_nrm;
					fbxmesh->GetPolygonVertexNormal(i, j, mesh_nrm);
					KFbxVector2 mesh_uv = mesh_uvs->GetAt(fbxmesh->GetTextureUVIndex(i, j));

					v.pos = D3DXVECTOR3(mesh_pos[0], mesh_pos[1], mesh_pos[2]);
					v.nrm = D3DXVECTOR3(mesh_nrm[0], mesh_nrm[1], mesh_nrm[2]);
					v.tex1 = D3DXVECTOR2(mesh_uv[0], mesh_uv[1] * -1.0);
					v.tex2 = D3DXVECTOR2(0.0, 0.0);
					v.diffuse = D3DCOLOR_XRGB(255, 255, 255);
				}
			}

			unsigned short* indices = mesh->indices;
			int* mesh_indices = fbxmesh->GetPolygonVertices();
			INFO("loading %i indices", mesh->indice_count);
			for (int i = 0; i < mesh->indice_count; i++)
			{
				indices[i] = i;
			}

			addMesh(mesh);

			if (meshname == name)
				ret = mesh;
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++)
	{
		Mesh* m = extractMesh(node->GetChild(i), filename, meshname);
		if(m)
			ret = m;
	}

	return ret;
}