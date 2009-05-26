#pragma once

namespace texture
{
	extern int debug;
	extern int use_atlas;

	class DXTexture
	{
	public:
		std::string name;
		bool is_transparent;
		bool is_lightmap;
		bool use_texture;
		bool draw;
		bool sky;

		IDirect3DTexture9* texture;

		DXTexture(const std::string& name);
		~DXTexture();
		bool activate(bool deactivate_current = true);
		void deactivate();
		void acquire();
		void release();
		DXTexture* overbright;
	};
}