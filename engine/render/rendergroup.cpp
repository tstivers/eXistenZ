#include "precompiled.h"
#include "render/rendergroup.h"
#include "render/hwbuffer.h"

namespace render {
};

using namespace render;

RenderGroup::RenderGroup()
{
	ZeroMemory(this, sizeof(RenderGroup));
};

RenderGroup::~RenderGroup()
{
	delete vertexbuffer;
	delete indexbuffer;
}

void RenderGroup::acquire()
{
	if(acquired)
		return;

	vertexbuffer = getVB(numvertices * stride, fvf, stride);
	indexbuffer = getIB(numindices * sizeof(unsigned short));
	acquired = true;
}

void RenderGroup::release()
{
	if(!acquired)
		return;
	delete vertexbuffer;
	delete indexbuffer;
	acquired = false;
}

void RenderGroup::update(void* vertexdata, void* indexdata)
{
	if(vertexdata)
		vertexbuffer->update(vertexdata);

	if(indexdata)
		indexbuffer->update(indexdata);
}

RenderGroup* render::getRenderGroup(DWORD fvf, unsigned int stride, unsigned int numvertices, unsigned int numindices, bool dynamic)
{
	RenderGroup* rg = new RenderGroup;
	rg->fvf = fvf;
	rg->stride = stride;
	rg->numvertices = numvertices;
	rg->numindices = numindices;

	return rg;
}