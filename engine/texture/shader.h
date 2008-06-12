#pragma once

#include "vfs/vfs.h"

namespace texture
{
	class DXTexture;

	class Shader
	{
	public:
		char* name;
		int refcount;

		Shader();
		Shader(vfs::File file);
		Shader(char* filename);
		~Shader();

		void init(DXTexture* texture);
		bool activate(DXTexture* texture);
		void deactivate(DXTexture* texture);
		void acquire();
		void release();
		bool load(vfs::File file);
		bool load(char* filename);

		int line; // hack
		unsigned int flags;
		unsigned int last_time;

		DXTexture** textures;

		// animated chain
		int chain_frames;
		int chain_current;
		unsigned int chain_time;
		unsigned int chain_last_time;

		// alpha test
		DWORD alphamask;

		// blend_add
		int blend_add_src;
		int blend_add_dst;

		// tex_translate
		unsigned int ttrans_time;
		unsigned int ttrans_last_time;
		D3DXVECTOR3 ttrans_pos;
		D3DXVECTOR3 ttrans_rot;
	};
};