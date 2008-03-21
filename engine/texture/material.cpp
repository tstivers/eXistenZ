#include "precompiled.h"
#include "texture/material.h"
#include "texture/shader.h"

namespace texture {
};

using namespace texture;

Material::Material()
{
	reset();
}

Material::Material(D3DCOLOR ambient, D3DXVECTOR3 direction, D3DXCOLOR color) :
	ambient(ambient)
{
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse = color;
	light.Direction = direction;
}

Material::~Material()
{
}

void Material::reset()
{
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	ZeroMemory(&ambient, sizeof(D3DCOLOR));
	light.Type = D3DLIGHT_DIRECTIONAL;
}