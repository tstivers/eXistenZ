/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: shader.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
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

	return true;
}

