/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: textloader.h,v 1.1 2003/12/04 12:33:48 tstivers Exp $
//

#pragma once

namespace mesh {

	class Mesh;
	class MeshSystem;

	Mesh* loadTextMesh(const std::string& filename);
	MeshSystem* loadTextMeshSystem(const std::string& filename);
}