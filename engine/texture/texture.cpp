#include "precompiled.h"
#include "texture/texture.h"
#include "texture/texturecache.h"
#include "render/render.h"
#include "render/rendergroup.h"
#include "render/dx.h"

namespace texture
{
};

using namespace texture;

DXTexture::DXTexture(const std::string& name)
: texture(NULL), name(name), is_transparent(false), use_texture(true), is_lightmap(false), draw(true), sky(false), overbright(NULL)
{
}

DXTexture::~DXTexture()
{
	if (texture)
		texture->Release();
}

bool DXTexture::activate(bool deactivate_current)
{
	render::frame_texswaps++;

	if (is_lightmap)
	{
		if (deactivate_current)
		{
			if (render::current_lightmap)
				render::current_lightmap->deactivate();
		}

		render::current_lightmap = this;

		render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
		render::device->SetTexture(1, texture);

		return true;
	}

	if (deactivate_current)
	{
		if (render::current_texture)
			render::current_texture->deactivate();
	}

	render::current_texture = this;

	//if (shader)
	//	shader->activate(this);

	//if (!use_texture)
	//	return true;

	render::device->SetTexture(0, texture);
	
	return true;
}

void DXTexture::deactivate()
{
	if (is_lightmap)
	{
		render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
		render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		render::current_lightmap = NULL;
		return;
	}

	render::current_texture = NULL;

	//if (shader)
	//	shader->deactivate(this);
}

void DXTexture::acquire()
{
}

void DXTexture::release()
{
}
