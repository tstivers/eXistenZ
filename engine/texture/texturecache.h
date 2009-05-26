#pragma once

namespace texture
{
	class DXTexture;

	void acquire(void);
	void release(void);

	struct tBSPLightmap
	{
		byte imageBits[128][128][3];   // The RGB data in a 128x128 image
	};

	DXTexture* getTexture(const char* name, bool use_alias = true);
	DXTexture* loadTexture(const char* name);
	void unloadTexture(const char* name);

	DXTexture* createTexture(const char* name, int width, int height);
	DXTexture* genLightmap(tBSPLightmap* data, float gamma, int boost, texture::DXTexture* overbright = NULL);
	bool textureExists(const std::string& name);
	void flush();

	void con_list_textures(int argc, char* argv[], void* user);
};