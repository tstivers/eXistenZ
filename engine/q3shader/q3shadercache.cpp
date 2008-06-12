#include "precompiled.h"
#include "q3shader/q3shadercache.h"
#include "q3shader/q3shader.h"
#include "settings/settings.h"
#include "vfs/vfs.h"
#include "vfs/file.h"
#include "texture/texture.h"
#include "texture/texturecache.h"
#include "misc/alias.h"

namespace q3shader
{
	struct eqstr
	{
		bool operator()(const char* s1, const char* s2) const
		{
			return _stricmp(s1, s2) == 0;
		}
	};

	typedef stdext::hash_map<const char*, Q3Shader*, hash_char_ptr_traits> shader_hash_map;
	typedef stdext::hash_map<const char*, char*, hash_char_ptr_traits> alias_hash_map;

	shader_hash_map shader_cache;
	alias_hash_map shader_file_map;
	misc::AliasList shader_alias;
	int debug;

	void loadShaderList();
	void loadShaderMap();
	void parseShader(char* filename);
}

void q3shader::init()
{
	settings::addsetting("system.render.shader.shaderpath", settings::TYPE_STRING, 0, NULL, NULL, NULL);
	settings::setstring("system.render.shader.shaderpath", "shaders");
	settings::addsetting("system.render.shader.debug", settings::TYPE_INT, 0, NULL, NULL, &debug);

	debug = 1;
}

void q3shader::release()
{
}

void q3shader::acquire()
{
	loadShaderList();
	loadShaderMap();
}

void q3shader::loadShaderList()
{
	vfs::file_list_t shader_list;
	vfs::getFileList(shader_list, settings::getstring("system.render.shader.shaderpath"), "*.shader");
	for (vfs::file_list_t::iterator it = shader_list.begin(); it != shader_list.end(); ++it)
	{
		char filename[256];
		sprintf(filename, "%s/%s",  settings::getstring("system.render.shader.shaderpath"), (*it));
		parseShader(filename);
	}
}

void q3shader::parseShader(char* filename)
{
	vfs::File file = vfs::getFile(filename);
	if (!file)
		return;

	char buf[1024];
	int line = 0;
	int level = 0;

	//LOG("parsing %s...", filename);

	while (file->readLine(buf, 1024))
	{

		line++;

		char* comment = strstr(buf, "//");
		if (comment) *comment = 0;

		strip(buf);

		if (!buf[0]) continue;
		//LOG("processing \"%s\"", buf);
		char* token = buf;
		char* this_token;
		while (this_token = getToken(&token, " \t"))
		{
			if (this_token[0] == '{')
				level++;
			else if (this_token[0] == '}')
				level--;
			else if (this_token[0] && level == 0)
			{
				LOG("adding \"%s\"", this_token);
				shader_file_map.insert(alias_hash_map::value_type(_strdup(this_token), _strdup(filename)));
			}
		}
	}
}

void q3shader::loadShaderMap()
{

}

q3shader::Q3Shader* q3shader::getShader(const char* name)
{
	if (!name || !*name)
		return NULL;

	//LOG("looking for \"%s\"", name);

	// check cache
	shader_hash_map::iterator cache_iter = shader_cache.find(name);
	if (cache_iter != shader_cache.end())
	{
		//LOG("found in cache");
		return ((*cache_iter).second);
	}

	// wasn't in the cache, check our shader list
	alias_hash_map::iterator map_iter = shader_file_map.find(name);
	if (map_iter != shader_file_map.end())
	{
		// shader was in list, create it
		//LOG("found in list");
		Q3Shader* shader = new Q3Shader(name);
		LOG("loading %s from file %s", name, (*map_iter).second);
		shader->load((*map_iter).second);
		shader_cache.insert(shader_hash_map::value_type(_strdup(name), shader));
		return shader;
	}

	// wasn't in cache or list, see if the texture exists and if so create the default shader
	texture::DXTexture* texture = texture::getTexture(name);
	if (texture)
	{
		//LOG("found texture");
		Q3Shader* shader = new Q3Shader(name);
		shader->texture.push_back(texture);
		shader->flags = FLAG_STD_TEXTURE;
		shader_cache.insert(shader_hash_map::value_type(_strdup(name), shader));
		return shader;
	}

	// just plain couldn't find it
	//LOG("couldn't find it");
	return NULL;
}

