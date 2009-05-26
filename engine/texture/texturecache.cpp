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
#include "q3bsp/q3bsp.h"

namespace texture
{
	int debug;
	int draw_unknown;
	int use_default;
	int use_atlas = 1;

	typedef stdext::hash_map<string, DXTexture*> texture_hash_map;

	texture_hash_map texture_cache;
	misc::AliasList texture_alias;

	void load_maps();
	void init();
	DXTexture* active_texture;

	DXTexture* genLightmapAtlas(tBSPLightmap* data, float gamma, int boost, DXTexture* overbright = NULL);
};

REGISTER_STARTUP_FUNCTION(texture, texture::init, 10);

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
	settings::addsetting("system.render.texture.use_atlas", settings::TYPE_INT, 0, NULL, NULL, &use_atlas);
	settings::setstring("system.render.texture.default_texture", "textures/default/uvmap.png");
	settings::setstring("system.render.texture.texture_alias_file", "textures/texture_alias.txt");
	settings::setstring("system.render.texture.shader_alias_file", "textures/shader_alias.txt");
	settings::setstring("system.render.texture.shader_map_file", "textures/shader_map.txt");
	active_texture = NULL;
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
	texture_alias.load(settings::getstring("system.render.texture.texture_alias_file"));
}

texture::DXTexture* texture::createTexture(const char* name, int width, int height)
{
	IDirect3DTexture9* texture = NULL;
	HRESULT hr;

	if (FAILED(render::device->CreateTexture(
		width,	// width
		height,	// height
		1,		// levels
		0,	// flags
		D3DFMT_X8R8G8B8, // format
		D3DPOOL_MANAGED, // pool
		&texture,
		NULL)))
	{
		LOG("failed to create texture");
		return NULL;
	}

	DXTexture* t = new DXTexture(name);
	t->texture = texture;
	ASSERT(!textureExists(name));
	texture_cache.insert(make_pair(name, t));
	return t;
}

texture::DXTexture* texture::getTexture(const char* texname, bool use_alias)
{
	char name[MAX_PATH];

	// check to see if texture is aliased
	if (use_alias && texture_alias.findAlias(texname))
		strcpy(name, texture_alias.findAlias(texname));
	else
		strcpy(name, texname);

	// check to see if texture is in texture_cache
	texture_hash_map::iterator iter = texture_cache.find(name);
	if (iter != texture_cache.end())
		return (*iter).second;

	// texture wasn't in texture_cache, try to load it
	DXTexture* texture = loadTexture(name);
	if (texture)
	{
		texture_cache.insert(texture_hash_map::value_type(texture->name, texture));
		return texture;
	}

	// couldn't load it, check for default texture
	if (use_default)
	{
		DXTexture* texture = loadTexture(settings::getstring("system.render.texture.default_texture"));
		if (texture)
		{
			// set the fake name
			texture->name = name;
			texture_cache.insert(texture_hash_map::value_type(texture->name, texture));
			return texture;
		}
	}

	// hand back an empty texture I guess
	if (draw_unknown)
		return new DXTexture(name);
	else
		return NULL;
}

void texture::flush()
{
}

void texture::unloadTexture(const char* name)
{
	if(textureExists(name))
	{
		DXTexture* texture = getTexture(name);
		texture_cache.erase(name);
		delete texture;
	}
}

texture::DXTexture* texture::loadTexture(const char* name)
{
	IDirect3DTexture9* texture = NULL;
	vfs::File file;
	char buf[MAX_PATH];

	strcpy(buf, name);
	char* endptr = &buf[strlen(name)];

	// first try the passed in value
	file = vfs::getFile(buf);
	if (file) goto found;

	// try .jpg
	strcpy(endptr, ".jpg");
	file = vfs::getFile(buf);
	if (file) goto found;

	// try .bmp
	strcpy(endptr, ".bmp");
	file = vfs::getFile(buf);
	if (file) goto found;

	// try .tga
	strcpy(endptr, ".tga");
	file = vfs::getFile(buf);
	if (file) goto found;

	// try .tif
	strcpy(endptr, ".tif");
	file = vfs::getFile(buf);
	if (file) goto found;

	goto done;

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

	HRESULT hr = D3DXCreateTextureFromFileInMemory(render::device, file->cache(), file->getSize(), &texture);

	if (FAILED(hr))
		goto err;

	if (debug)
	{
		LOG("loaded %s", name);
	}

	if (!file)
		goto done;

done:
	if (!texture)
		goto err;

	DXTexture* dxtex = new DXTexture(name);
	dxtex->texture = texture;
	return dxtex;

err:
	if(debug)
	LOG("failed to load %s", name);

	return NULL;
}

bool texture::textureExists(const std::string& name)
{
	return texture_cache.find(name) != texture_cache.end();
}

texture::DXTexture* texture::genLightmap(tBSPLightmap* data, float gamma, int boost, texture::DXTexture* overbright)
{
	IDirect3DTexture9* texture = NULL;
	IDirect3DSurface9* surface = NULL;
	HRESULT hr;

	if(use_atlas)
		return genLightmapAtlas(data, gamma, boost);

	if (FAILED(render::device->CreateTexture(
				   128,	// width
				   128,	// height
				   1,		// levels
				   0,	// flags
				   D3DFMT_X8R8G8B8, // format
				   D3DPOOL_MANAGED, // pool
				   &texture,
				   NULL)))
	{
		LOG("failed to generate lightmap");
		return NULL;
	}

	if (FAILED(texture->GetSurfaceLevel(0, &surface)))
	{
		LOG("failed to get surface");
		return NULL;
	}

	D3DLOCKED_RECT bleh;
	if (FAILED(hr = surface->LockRect(&bleh, NULL, 0)))
	{
		LOG("failed to lock surface");
		return NULL;
	}

	if (bleh.Pitch != 512)
	{
		LOG("generation failed (pitch = %i)", bleh.Pitch);
		return NULL;
	}

	// 2 = red 1 = green 0 = blue 3 = null
	byte dstbits[128][128][4];

	for (int row = 0; row < 128; row++)
	{
		for (int col = 0; col < 128; col++)
		{
			byte color[3];

			color[0] = data->imageBits[row][col][0];
			color[1] = data->imageBits[row][col][1];
			color[2] = data->imageBits[row][col][2];

			q3bsp::R_ColorShiftLightingBytes(color, overbright ? 2 : 1);

			dstbits[row][col][3] = 0;
			dstbits[row][col][2] = color[0];
			dstbits[row][col][1] = color[1];
			dstbits[row][col][0] = color[2];

			//console::log(console::FLAG_DEBUG, "[dst] %i, %i, %i, %i", dstbits[row][col][0], dstbits[row][col][1], dstbits[row][col][2], dstbits[row][col][3]);
		}
	}
	memcpy(bleh.pBits, dstbits, 128 * 128 * 4);

	if (FAILED(surface->UnlockRect()))
	{
		LOG("failed to unlock surface");
		return NULL;
	}

	surface->Release();

	//texture->GenerateMipSubLevels();

	static int count = 0;
	DXTexture* dxtex = new DXTexture(string("lightmap_") + lexical_cast<string>(count++));
	dxtex->texture = texture;
	dxtex->is_lightmap = true;
	if(!overbright)
		dxtex->overbright = genLightmap(data, gamma, boost, dxtex);	
	return dxtex;
}

void texture::con_list_textures(int argc, char* argv[], void* user)
{
	for (texture_hash_map::iterator it = texture_cache.begin(); it != texture_cache.end(); ++it)
	{
		if ((argc == 1) || ((argc == 2) && wildcmp(argv[1], it->first.c_str())) ||
				((argc == 3) && wildcmp(argv[1], it->first.c_str())))
			INFO("%s", it->second->name.c_str());
	}
}

texture::DXTexture* texture::genLightmapAtlas(tBSPLightmap* data, float gamma, int boost, DXTexture* overbright)
{
	static int count;
	DXTexture* lightmap = NULL;
	if(!textureExists("lightmap_atlas"))
	{
		lightmap = createTexture("lightmap_atlas", 2048, 2048);
		lightmap->is_lightmap = true;
		lightmap->overbright = createTexture("lightmap_atlas_overbright", 2048, 2048);
		lightmap->overbright->is_lightmap = true;
		count = 0;
	}
	else if(overbright)
		lightmap = overbright;
	else
		lightmap = getTexture("lightmap_atlas", false);

	ASSERT(lightmap);

	IDirect3DSurface9* surface = NULL;
	if (FAILED(lightmap->texture->GetSurfaceLevel(0, &surface)))
	{
		LOG("failed to get surface");
		return NULL;
	}

	D3DLOCKED_RECT bleh;
	if (FAILED(surface->LockRect(&bleh, NULL, 0)))
	{
		LOG("failed to lock surface");
		return NULL;
	}

	// 2 = red 1 = green 0 = blue 3 = null
	byte dstbits[128][128][4];

	for (int row = 0; row < 128; row++)
	{
		for (int col = 0; col < 128; col++)
		{
			byte color[3];

			color[0] = data->imageBits[row][col][0];
			color[1] = data->imageBits[row][col][1];
			color[2] = data->imageBits[row][col][2];

			q3bsp::R_ColorShiftLightingBytes(color, overbright ? 2 : 1);

			dstbits[row][col][3] = 0;
			dstbits[row][col][2] = color[0];
			dstbits[row][col][1] = color[1];
			dstbits[row][col][0] = color[2];
		}
	}

	for(int i = 0; i < 128; i++)
		memcpy((char*)bleh.pBits + ((count % 16) * 512) + ((count / 16) * bleh.Pitch * 128) + (i * bleh.Pitch), dstbits[i], 128 * 4);

	if (FAILED(surface->UnlockRect()))
	{
		LOG("failed to unlock surface");
		return NULL;
	}

	surface->Release();
	if(!overbright)
		genLightmapAtlas(data, gamma, boost, lightmap->overbright);
	else
		count++;
	ASSERT(count < 256);
	return lightmap;
}