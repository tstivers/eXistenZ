/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: texture.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#include "precompiled.h"
#include "texture/texture.h"
#include "texture/shader.h"
#include "texture/texturecache.h"
#include "console/console.h"
#include "render/render.h"
#include "render/dx.h"

namespace texture {
};

using namespace texture;

DXTexture::DXTexture() 
{ 
	texture = NULL; 
	shader = NULL;
	name = NULL; 
	is_transparent = false;
	use_texture = true;
	is_lightmap = false;
	draw = true;
	sky = false;
	refcount = 1; 
}

DXTexture::~DXTexture() 
{ 
	if(texture) 
		texture->Release(); 

	if(shader)
		shader->release();

	delete name; 
}

bool DXTexture::activate()
{	
	if(is_lightmap) {
		render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
		render::device->SetTexture(1, texture);
		return true;
	}

	if(shader)
		shader->activate(this);

	if(!use_texture)
		return true;

	if(texture)
		render::device->SetTexture(0, texture);
	else
		render::device->SetTexture(0, NULL);

	return true;
}

void DXTexture::deactivate()
{
	if(shader)
		shader->deactivate(this);	
}

void DXTexture::acquire()
{
	refcount++;
}

void DXTexture::release()
{
	refcount--;
	if(!refcount)
		delete this;
}
