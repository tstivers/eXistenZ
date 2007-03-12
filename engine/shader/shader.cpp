/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#include "precompiled.h"
#include "shader/shader.h"

using namespace shader;

Shader::Shader(const char* name)
{
	this->name = _strdup(name);
	flags = 0;
}

Shader::~Shader()
{	
	free(this->name);
}