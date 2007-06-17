#include "precompiled.h"
#include "texture/texturecache.h"
#include "texture/texture.h"
#include "texture/shader.h"
#include "vfs/vfs.h"
#include "console/console.h"
#include "settings/settings.h"
#include "render/render.h"
#include "render/dx.h"
#include "misc/alias.h"

namespace texture {
	int debug;
	int draw_unknown;
	int use_default;
	texture_hash_map texture_cache;
	shader_hash_map shader_cache;
	misc::AliasList texture_alias;
	misc::AliasList shader_alias;
	misc::AliasList shader_map;

	void load_maps();
	DXTexture* active_texture;
	Shader* active_shader;
};


void texture::init()
{
	debug = 1;
	draw_unknown = 0;
	use_default = 0;
	settings::addsetting("system.render.texture.debug", settings::TYPE_INT, 0, NULL, NULL, &debug);
	settings::addsetting("system.render.texture.draw_unknown", settings::TYPE_INT, 0, NULL, NULL, &draw_unknown);
	settings::addsetting("system.render.texture.use_default", settings::TYPE_INT, 0, NULL, NULL, &use_default);
	settings::addsetting("system.render.texture.default_texture", settings::TYPE_STRING, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.texture.texture_alias_file", settings::TYPE_STRING, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.texture.shader_alias_file", settings::TYPE_STRING, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.texture.shader_map_file", settings::TYPE_STRING, 0, NULL, NULL, NULL);
	settings::setstring("system.render.texture.default_texture", "textures/proto/default.bmp");
	settings::setstring("system.render.texture.texture_alias_file", "textures/texture_alias.txt");
	settings::setstring("system.render.texture.shader_alias_file", "textures/shader_alias.txt");
	settings::setstring("system.render.texture.shader_map_file", "textures/shader_map.txt");
	active_texture = NULL;
	active_shader = NULL;
	console::addCommand("list_textures", con_list_textures, NULL);
}

void texture::release()
{
}

void texture::acquire()
{
	load_maps();
}

void texture::load_maps()
{
	shader_map.load(settings::getstring("system.render.texture.shader_map_file"));
	texture_alias.load(settings::getstring("system.render.texture.texture_alias_file"));
	shader_alias.load(settings::getstring("system.render.texture.shader_alias_file"));
}

texture::DXTexture* texture::getTexture(const char* texname, bool use_alias)
{
	char name[MAX_PATH];

	// check to see if texture is aliased	
	if(use_alias && texture_alias.findAlias(texname))
		strcpy(name, texture_alias.findAlias(texname));
	else
		strcpy(name, texname);

	// check to see if texture is in texture_cache
	texture_hash_map::iterator iter = texture_cache.find(name);
	if(iter != texture_cache.end())
		return (*iter).second;

	// texture wasn't in texture_cache, try to load it
	DXTexture* texture = loadTexture(name);
	if(texture) {
		texture_cache.insert(texture_hash_map::value_type(texture->name, texture));
		return texture;
	}

	// couldn't load it, check for default texture
	if(use_default)
	{
		DXTexture* texture = loadTexture(settings::getstring("system.render.texture.default_texture"));		
		if(texture) {
			// set the fake name
			free(texture->name);
			texture->name = _strdup(name);
			texture_cache.insert(texture_hash_map::value_type(texture->name, texture));
			return texture;
		}
	}

	// hand back an empty texture I guess
	if(draw_unknown)
		return new DXTexture();
	else 
		return NULL;
}

void texture::flush()
{
}

texture::DXTexture* texture::loadTexture(const char* name)
{	
	IDirect3DTexture9* texture = NULL;
	Shader* shader = NULL;
	vfs::IFilePtr file;
	char buf[MAX_PATH];	

	strcpy(buf, name);
	char* endptr = &buf[strlen(name)];

	// first try the passed in value
	file = vfs::getFile(buf);
	if(file) goto found;
	
	// try .jpg
	strcpy(endptr, ".jpg");
	file = vfs::getFile(buf);
	if(file) goto found;

	// try .bmp
	strcpy(endptr, ".bmp");
	file = vfs::getFile(buf);
	if(file) goto found;

	// try .tga
	strcpy(endptr, ".tga");
	file = vfs::getFile(buf);
	if(file) goto found; 	

	goto shader;

found:
	//HRESULT hr = D3DXCreateTextureFromFile(render::device, file->real_filename, &texture);
	/*HRESULT hr = D3DXCreateTextureFromFileEx(device(), file->filename, 
		0, 0,
		0, 0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(0, 0, 0),
		NULL,
		NULL,
		&texture); */

	HRESULT hr = D3DXCreateTextureFromFileInMemory(render::device, file->cache(), file->size, &texture);

	if(FAILED(hr))
		goto err;
	
	if(debug) {
		LOG("[texture::loadTexture] loaded %s", name);
	}

shader:
	strcpy(endptr, ".shader");
	file = vfs::getFile(buf);
	if(!file) {
		// check the shader map
		file = vfs::getFile(shader_map.findAlias(name));
	}

	if(!file) 
		goto done;

	shader = new Shader(file);

done:
	if(!texture && !shader)
		goto err;

	DXTexture* dxtex = new DXTexture();
	dxtex->texture = texture;
	dxtex->shader = shader;
	dxtex->name = _strdup(name);
	if(shader)
		shader->init(dxtex);

	return dxtex;

err:	
//	if(debug) 
		LOG("[texture::loadTexture] failed to load %s", name);

	return NULL;
}

texture::DXTexture* texture::genLightmap(tBSPLightmap* data, float gamma, int boost)
{
	IDirect3DTexture9* texture = NULL;
	IDirect3DSurface9* surface = NULL;
	HRESULT hr;

	if(FAILED(render::device->CreateTexture(
		128,	// width
		128,	// height
		1,		// levels
		0,	// flags
		D3DFMT_X8R8G8B8, // format
		D3DPOOL_MANAGED, // pool
		&texture,
		NULL))) {
			LOG("[texture::genLightmap] failed to generate lightmap");
			return NULL;
		}

		if(FAILED(texture->GetSurfaceLevel(0, &surface))) {
			LOG("[texture::genLightmap] failed to get surface");
			return NULL;
		}
				
		D3DLOCKED_RECT bleh;
		if(FAILED(hr = surface->LockRect(&bleh, NULL, 0))) {
			LOG("[texture::genLightmap] failed to lock surface");
			return NULL;
		}

		if(bleh.Pitch != 512) {
			LOG("[texture::genLightmap] generation failed (pitch = %i)", bleh.Pitch);
			return NULL;
		}
		
		// 2 = red 1 = green 0 = blue 3 = null
		byte dstbits[128][128][4];

		for(int row = 0; row < 128; row++) {
			for(int col = 0; col < 128; col++) {
				float r, g, b;

				r = (float)data->imageBits[row][col][0] + boost;
				g = (float)data->imageBits[row][col][1] + boost;
				b = (float)data->imageBits[row][col][2] + boost;

				r *= gamma / 255.0f;
				g *= gamma / 255.0f;
				b *= gamma / 255.0f;

				//find the value to scale back up
				float scale = 1.0f;
				float temp;
				if(r > 1.0f && (temp = (1.0f/r)) < scale) scale = temp;
				if(g > 1.0f && (temp = (1.0f/g)) < scale) scale = temp;
				if(b > 1.0f && (temp = (1.0f/b)) < scale) scale = temp;

				// scale up color values
				scale *= 255.0f;		
				r *= scale;
				g *= scale;
				b *= scale;

				dstbits[row][col][3] = 0;				
				dstbits[row][col][2] = (byte)r;
				dstbits[row][col][1] = (byte)g;
				dstbits[row][col][0] = (byte)b;

				//console::log(console::FLAG_DEBUG, "[dst] %i, %i, %i, %i", dstbits[row][col][0], dstbits[row][col][1], dstbits[row][col][2], dstbits[row][col][3]);
			}
		}
		memcpy(bleh.pBits, dstbits, 128 * 128 * 4);

		if(FAILED(surface->UnlockRect())) {
			LOG("[texture::genLightmap] failed to unlock surface");
			return NULL;
		}

		//texture->GenerateMipSubLevels();

		DXTexture* dxtex = new DXTexture();
		dxtex->texture = texture;		
		dxtex->is_lightmap = true;
		return dxtex;
}

void texture::con_list_textures(int argc, char* argv[], void* user)
{
	for(texture_hash_map::iterator it = texture_cache.begin(); it != texture_cache.end(); ++it)
	{
		if((argc == 1) || ((argc == 2) && wildcmp(argv[1], it->first)) || 
			((argc == 3) && wildcmp(argv[1], it->first) && it->second->shader && wildcmp(argv[2], it->second->shader->name)))
			INFO("%s %s%s%s", 
			it->second->name, 
			it->second->shader ? "(" : "", 
			it->second->shader ? it->second->shader->name : "", 
			it->second->shader ? ")" : "");
	}
}