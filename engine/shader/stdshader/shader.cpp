/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#include "precompiled.h"
#include "shader/shader.h"
#include "shader.h"

using namespace shader;

StdShader::StdShader(const char* name) : Shader(name)
{
}

StdShader::~StdShader()
{
}

bool StdShader::load(const char* filename) {
	return false;
}

void StdShader::setTexture(int type, texture::DXTexture* texture)
{
}

int StdShader::activate()
{
	return 0;
}

bool StdShader::nextPass()
{
	return false;
}

void StdShader::deactivate()
{
}