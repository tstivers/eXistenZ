#pragma once

namespace texture {
	class Shader;

	class DXTexture {
	public:
		char* name;
		int refcount;
		bool is_transparent;
		bool is_lightmap;
		bool use_texture;
		bool draw;
		bool sky;

		IDirect3DTexture9* texture;
		Shader* shader;

		DXTexture();
		~DXTexture();
		bool activate();
		void deactivate();
		void acquire();
		void release();
	};
};