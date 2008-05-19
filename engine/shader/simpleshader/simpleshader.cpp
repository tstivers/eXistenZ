#include "precompiled.h"
#include "shader/shader.h"
#include "shader/simpleshader/simpleshader.h"
#include "texture/texturecache.h"
#include "render/render.h"

using namespace shader;

SimpleShader::SimpleShader(const char* name) : Shader(name)
{
	texture = NULL;	
}

SimpleShader::~SimpleShader()
{
}

bool SimpleShader::load(const char* filename) {
	texture = texture::getTexture(filename);
	if(!texture)
		return false;

	if(debug) LOG("loaded \"%s\"", texture->name);

	return true;
}

void SimpleShader::setTexture(int type, texture::DXTexture* texture)
{
	switch(type) {
		case TEXTURE_DIFFUSE: 
			this->texture = texture;
			break;
		default:
			break;
	}
}

int SimpleShader::activate()
{
	render::device->SetTexture(0, texture->texture);
	return 1;
}

bool SimpleShader::nextPass()
{
	return false;
}

void SimpleShader::deactivate()
{
}