#include "precompiled.h"
#include "shader/shadercache.h"
#include "shader/shader.h"
#include "settings/settings.h"
#include "texture/texturecache.h"
#include "vfs/vfs.h"
#include "shader/simpleshader/simpleshader.h"
#include "shader/stdshader/shader.h"
#include "misc/alias.h"

namespace shader
{
	typedef stdext::hash_map<const char*, Shader*, hash_char_ptr_traits> shader_hash_map;

	shader_hash_map shader_cache;
	misc::AliasList shader_alias;
	int debug;

	Shader* loadShader(const char* name);
}

REGISTER_STARTUP_FUNCTION(shader, shader::init, 10);

void shader::init()
{
	settings::addsetting("system.render.shader.shaderpath", settings::TYPE_STRING, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.shader.shader_map_file", settings::TYPE_STRING, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.shader.debug", settings::TYPE_INT, 0, NULL, NULL, &debug);
	settings::setstring("system.render.shader.shaderpath", "shaders");
	settings::setstring("system.render.shader.shader_map_file", "shaders/shader_map.txt");

	debug = 1;
}

void shader::release()
{
}

void shader::acquire()
{
	// load alias file
	shader_alias.load(settings::getstring("system.render.texture.shader_map_file"));
}

shader::Shader* shader::getShader(const char* name)
{
	char shader_name[MAX_PATH];


	// check to see if the shader is aliased
	if (!*(strcpy(shader_name, shader_alias.findAlias(name))))
		strcpy(shader_name, name);

	if (debug) LOG("retrieving \"%s\" (%s)", shader_name, name);

	// check to see if the shader is cached
	shader_hash_map::iterator iter = shader_cache.find(shader_name);
	if (iter != shader_cache.end())  // found it
	{
		if (debug) LOG("shader cached");
		return (*iter).second;
	}

	// shader wasn't cached, needs to be loaded
	Shader* shader = loadShader(shader_name);
	if (shader)  // shader loaded, cache it
	{
		if (debug) LOG("shader loaded");
		shader_cache.insert(shader_hash_map::value_type(shader->name, shader));
		return shader;
	}

	return NULL;
}

shader::Shader* shader::loadShader(const char* name)
{
	char shader_file[MAX_PATH];
	vfs::File file;
	Shader* shader = NULL;

	strcpy(shader_file, name);
	char* endptr = &shader_file[strlen(name)];

	// try standard shader (.shader)
	strcpy(endptr, ".shader");
	file = vfs::getFile(shader_file);
	if (file)
	{
		shader = new shader::StdShader(name);
		if (shader->load(shader_file))
			return shader;
		else
			delete shader;
	}

	// try shader program (.hws)

	// try simple texture shader
	if (texture::getTexture(name))
	{
		shader = new shader::SimpleShader(name);
		if (shader->load(shader_file))
			return shader;
		else
			delete shader;
	}

	return NULL;
}