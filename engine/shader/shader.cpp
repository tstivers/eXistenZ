/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: shader.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#include "precompiled.h"
#include "shader/shader.h"

using namespace shader;

Shader::Shader(const char* name)
{
	this->name = strdup(name);
	flags = 0;
}

Shader::~Shader()
{	
	free(this->name);
}