/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: texturecache.h,v 1.3 2003/10/08 16:38:58 tstivers Exp $
//

#pragma once

namespace texture {
	class DXTexture;
	class Shader;

	void init(void);
	void acquire(void);
	void release(void);

	struct tBSPLightmap
	{
		byte imageBits[128][128][3];   // The RGB data in a 128x128 image
	};

	struct eqstr {
		bool operator() (char* s1, char* s2) const {
			return strcmp(s1, s2) == 0;
		}
	};

	typedef struct _pair {
		char* key;
		char* value;
		_pair(char* k, char* v) {key = k; value = v;}
	} pair;

	typedef stdext::hash_map<char*, DXTexture*, hash_char_ptr> texture_hash_map;
	typedef stdext::hash_map<char*, Shader*, hash_char_ptr> shader_hash_map;
	typedef std::list<pair*> alias_list;

	DXTexture* getTexture(const char* name, bool use_alias = true);
	DXTexture* loadTexture(const char* name);	
	DXTexture* genLightmap(tBSPLightmap* data, float gamma = 1.0, int boost = 0);
	void flush();

	extern Shader* active_shader;

	void con_list_textures(int argc, char* argv[], void* user);
};