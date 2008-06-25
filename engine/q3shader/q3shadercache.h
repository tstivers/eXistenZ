#pragma once

#include "vfs/vfs.h"

namespace scene
{
	class Scene;
}

namespace q3shader
{
	class Q3ShaderCache;
	class Q3Shader;

	class Q3ShaderCache
	{
	public:
		Q3ShaderCache(/* scene::Scene* scene */);
		~Q3ShaderCache();

		void loadShaders(vfs::File file);

		Q3Shader* getShader(const string& name);

	protected:
		typedef map<string, shared_ptr<Q3Shader>> shader_map;
		shader_map m_shaders;
		//scene::Scene* m_scene;
	};
}