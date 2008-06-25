#include "precompiled.h"
#include "q3shader/q3shadercache.h"
#include "q3shader/q3shader.h"

using namespace q3shader;

Q3ShaderCache::Q3ShaderCache(/* scene::Scene* scene */)
{

}

Q3ShaderCache::~Q3ShaderCache()
{

}


void Q3ShaderCache::loadShaders(vfs::File file)
{
	if(!file)
		return;

	char buffer[512];
	int parselevel = 0;
	vector<string> shadertext;
	
	string name;
	bool skip = false;
	while(skip || file->readLine(buffer, 512))
	{
		skip = false;
		char* comment = strstr(buffer, "//");
		if (comment) *comment = 0;
		strip(buffer);
		to_lower(buffer);

		if(!buffer[0])  // ignore blank lines
			continue;

		if(buffer[0] == '/' && buffer[1] == '/') // ignore comments
			continue;

		//INFO("[%i] read line \"%s\"", parselevel, buffer);

		if(buffer[0] == '{')
		{
			skip = true;
			buffer[0] = ' ';
			strip(buffer);
			switch(parselevel)
			{
			case 0: // opening a new shader
				ASSERT(!name.empty());
				parselevel++;
				break;
			case 1: // opening a pass
				shadertext.push_back("{");
				parselevel++;
				break;
			default: // shouldn't happen
				ASSERT(false);
				break;
			}
		}
		else if(buffer[0] == '}')
		{
			skip = true;
			buffer[0] = ' ';
			strip(buffer);
			switch(parselevel)
			{
			case 0: // shouldn't happen
				ASSERT(false);
				break;
			case 1: // closing shader
				m_shaders.insert(shader_map::value_type(name, shared_ptr<Q3Shader>(new Q3Shader(this, shadertext))));
				shadertext.clear();
				name.clear();
				parselevel--;
				break;
			case 2: // closing a pass
				shadertext.push_back("}");
				parselevel--;
				break;
			default: // shouldn't happen
				ASSERT(false);
				break;
			}
		}
		else
		{
			switch(parselevel)
			{
			case 0: // at global level
				name = buffer;
				//INFO("found shader \"%s\"", buffer);
				break;
			case 1: // in shader
			case 2: // in pass
				shadertext.push_back(buffer);
				break;
			default: // shouldn't happen
				ASSERT(false);
			}
		}
	}
}

Q3Shader* Q3ShaderCache::getShader(const string& name)
{
	shader_map::iterator it = m_shaders.find(name);
	if(it == m_shaders.end())
		return NULL;

	return it->second.get();
}