/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: skybox.cpp,v 1.5 2003/12/13 02:58:04 tstivers Exp $
//

#include "precompiled.h"
#include "skybox/skybox.h"
#include "render/render.h"
#include "render/dx.h"
#include "console/console.h"
#include "texture/texturecache.h"
#include "texture/texture.h"
#include "settings/settings.h"

namespace skybox {
	texture::DXTexture** textures;

	D3DXVECTOR3 min, max;
	SkyVertex *verts;

	int draw;
	int width, height, depth;
	bool acquired;
	char texture[MAX_PATH];
	IDirect3DVertexBuffer9* dxvertbuf;
	D3DMATERIAL9 mtrl;

	void genBox();
	void con_sky(int argc, char* argv[], void* user);
};

void skybox::init()
{
	settings::addsetting("system.render.skybox.draw", settings::TYPE_INT, 0, NULL, NULL, &draw);
	settings::addsetting("system.render.skybox.width", settings::TYPE_INT, 0, NULL, NULL, &width);
	settings::addsetting("system.render.skybox.height", settings::TYPE_INT, 0, NULL, NULL, &height);
	settings::addsetting("system.render.skybox.depth", settings::TYPE_INT, 0, NULL, NULL, &depth);
	settings::addsetting("system.render.skybox.texture", settings::TYPE_STRING, 0, NULL, NULL, &texture);
	
	con::addCommand("toggle_sky", con::toggle_int, &draw);
	con::addCommand("sky_reset", skybox::reset, NULL);
	con::addCommand("sky", skybox::con_sky, NULL);

	draw = 1;
	acquired = false;
	width = 1;
	height = 1;
	depth = 1;
	texture[0] = 0;

	ZeroMemory( &mtrl, sizeof(mtrl) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;

}

void skybox::acquire()
{
	acquired = true;

	// get memory for verts
	verts = new SkyVertex[4 * 6];

	// generate our vertexes
	genBox();

	// create our vertex buffer
	if(FAILED(render::device->CreateVertexBuffer(4 * 6 * sizeof(SkyVertex),
		D3DUSAGE_WRITEONLY,
		SkyVertex.FVF,
		D3DPOOL_MANAGED,
		&dxvertbuf,
		NULL))) {
			LOG("[skybox::acquire] failed to create vertex buffer");
			return;
		}

	void* vertbuf;
	if(FAILED(dxvertbuf->Lock(0, 4 * 6 * sizeof(SkyVertex), &vertbuf, D3DLOCK_DISCARD))) {
		LOG("[skybox::acquire] failed to lock vertex buffer");
		return;
	}
	memcpy(vertbuf, verts, 4 * 6 * sizeof(SkyVertex));
	dxvertbuf->Unlock();

	// load our textures
	char texbuf[MAX_PATH];	
	strcpy(texbuf, texture);
	char* ext = texbuf + strlen(texbuf);
	textures = new texture::DXTexture*[6];

	strcpy(ext, "ft");
	textures[BOX_FRONT] = texture::getTexture(texbuf);
	strcpy(ext, "bk");
	textures[BOX_BACK] = texture::getTexture(texbuf);
	strcpy(ext, "lf");
	textures[BOX_LEFT] = texture::getTexture(texbuf);
	strcpy(ext, "rt");
	textures[BOX_RIGHT] = texture::getTexture(texbuf);
	strcpy(ext, "up");
	textures[BOX_TOP] = texture::getTexture(texbuf);
	strcpy(ext, "dn");
	textures[BOX_BOTTOM] = texture::getTexture(texbuf);
}

void skybox::reset()
{
	if(acquired)
		unacquire();

	acquire();
}

void skybox::genBox()
{
	float x = (float)width / 2.0f;
	float y = (float)height / 2.0f;
	float z = (float)depth / 2.0f;

	// generate front
	verts[0].pos.x = -x;
	verts[0].pos.y = -y;
	verts[0].pos.z = z;
	verts[0].tex1.x = 1.0f;
	verts[0].tex1.y = 1.0f;

	verts[1].pos.x = -x;
	verts[1].pos.y = y;
	verts[1].pos.z = z;
	verts[1].tex1.x = 1.0f;
	verts[1].tex1.y = 0.0f;

	verts[2].pos.x = x;
	verts[2].pos.y = y;
	verts[2].pos.z = z;
	verts[2].tex1.x = 0.0f;
	verts[2].tex1.y = 0.0f;

	verts[3].pos.x = x;
	verts[3].pos.y = -y;
	verts[3].pos.z = z;
	verts[3].tex1.x = 0.0f;
	verts[3].tex1.y = 1.0f;

	// generate back
	verts[5].pos.x = -x;
	verts[5].pos.y = -y;
	verts[5].pos.z = -z;
	verts[5].tex1.x = 0.0f;
	verts[5].tex1.y = 1.0f;

	verts[4].pos.x = -x;
	verts[4].pos.y = y;
	verts[4].pos.z = -z;
	verts[4].tex1.x = 0.0f;
	verts[4].tex1.y = 0.0f;

	verts[7].pos.x = x;
	verts[7].pos.y = y;
	verts[7].pos.z = -z;
	verts[7].tex1.x = 1.0f;
	verts[7].tex1.y = 0.0f;

	verts[6].pos.x = x;
	verts[6].pos.y = -y;
	verts[6].pos.z = -z;
	verts[6].tex1.x = 1.0f;
	verts[6].tex1.y = 1.0f;

	// generate left
	verts[8].pos.x = -x;
	verts[8].pos.y = y;
	verts[8].pos.z = -z;
	verts[8].tex1.x = 1.0f;
	verts[8].tex1.y = 0.0f;

	verts[9].pos.x = -x;
	verts[9].pos.y = y;
	verts[9].pos.z = z;
	verts[9].tex1.x = 0.0f;
	verts[9].tex1.y = 0.0f;

	verts[10].pos.x = -x;
	verts[10].pos.y = -y;
	verts[10].pos.z = z;
	verts[10].tex1.x = 0.0f;
	verts[10].tex1.y = 1.0f;

	verts[11].pos.x = -x;
	verts[11].pos.y = -y;
	verts[11].pos.z = -z;
	verts[11].tex1.x = 1.0f;
	verts[11].tex1.y = 1.0f;

	// generate right
	verts[13].pos.x = x;
	verts[13].pos.y = y;
	verts[13].pos.z = -z;
	verts[13].tex1.x = 0.0f;
	verts[13].tex1.y = 0.0f;

	verts[12].pos.x = x;
	verts[12].pos.y = y;
	verts[12].pos.z = z;
	verts[12].tex1.x = 1.0f;
	verts[12].tex1.y = 0.0f;

	verts[15].pos.x = x;
	verts[15].pos.y = -y;
	verts[15].pos.z = z;
	verts[15].tex1.x = 1.0f;
	verts[15].tex1.y = 1.0f;

	verts[14].pos.x = x;
	verts[14].pos.y = -y;
	verts[14].pos.z = -z;
	verts[14].tex1.x = 0.0f;
	verts[14].tex1.y = 1.0f;

	// generate top
	verts[17].pos.x = -x;
	verts[17].pos.y = y;
	verts[17].pos.z = z;
	verts[17].tex1.x = 1.0f;
	verts[17].tex1.y = 0.0f;

	verts[16].pos.x = x;
	verts[16].pos.y = y;
	verts[16].pos.z = z;
	verts[16].tex1.x = 1.0f;
	verts[16].tex1.y = 1.0f;

	verts[19].pos.x = x;
	verts[19].pos.y = y;
	verts[19].pos.z = -z;
	verts[19].tex1.x = 0.0f;
	verts[19].tex1.y = 1.0f;

	verts[18].pos.x = -x;
	verts[18].pos.y = y;
	verts[18].pos.z = -z;
	verts[18].tex1.x = 0.0f;
	verts[18].tex1.y = 0.0f;

	// generate bottom
	verts[20].pos.x = -x;
	verts[20].pos.y = -y;
	verts[20].pos.z = z;
	verts[20].tex1.x = 1.0f;
	verts[20].tex1.y = 1.0f;

	verts[21].pos.x = x;
	verts[21].pos.y = -y;
	verts[21].pos.z = z;
	verts[21].tex1.x = 1.0f;
	verts[21].tex1.y = 0.0f;

	verts[22].pos.x = x;
	verts[22].pos.y = -y;
	verts[22].pos.z = -z;
	verts[22].tex1.x = 0.0f;
	verts[22].tex1.y = 0.0f;

	verts[23].pos.x = -x;
	verts[23].pos.y = -y;
	verts[23].pos.z = -z;
	verts[23].tex1.x = 0.0f;
	verts[23].tex1.y = 1.0f;
}

void skybox::render()
{
	if(!draw)
		return;

	if(!acquired)
		acquire();

	render::device->SetStreamSource(0, dxvertbuf, 0, sizeof(SkyVertex));
	render::device->SetFVF(SkyVertex.FVF);
	render::device->SetMaterial( &mtrl );
	render::device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	render::device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	render::device->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	render::device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	render::device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	render::device->SetSamplerState( 0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP );
	render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);			
	render::device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	render::device->SetRenderState( D3DRS_LIGHTING, FALSE );
	render::device->SetRenderState( D3DRS_AMBIENT, 0);
	render::device->SetRenderState( D3DRS_ZENABLE, D3DZB_USEW );
	render::device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	render::device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	D3DXMATRIX skybox;
	D3DXMatrixIdentity(&skybox);
	D3DXMatrixTranslation(&skybox, render::cam_pos.x, render::cam_pos.y, render::cam_pos.z);
	render::device->SetTransform( D3DTS_WORLD, &skybox );

	if(render::wireframe) {
		render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);		
		render::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	} else {
		render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		render::device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		render::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	// render cube
	if(textures[BOX_FRONT])	textures[BOX_FRONT]->activate();
	render::device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	if(textures[BOX_BACK])	textures[BOX_BACK]->activate();
	render::device->DrawPrimitive(D3DPT_TRIANGLEFAN, 4, 2);
	if(textures[BOX_LEFT]) textures[BOX_LEFT]->activate();
	render::device->DrawPrimitive(D3DPT_TRIANGLEFAN, 8, 2);
	if(textures[BOX_RIGHT]) textures[BOX_RIGHT]->activate();
	render::device->DrawPrimitive(D3DPT_TRIANGLEFAN, 12, 2);
	if(textures[BOX_TOP]) textures[BOX_TOP]->activate();
	render::device->DrawPrimitive(D3DPT_TRIANGLEFAN, 16, 2);
	if(textures[BOX_BOTTOM]) textures[BOX_BOTTOM]->activate();
	render::device->DrawPrimitive(D3DPT_TRIANGLEFAN, 20, 2);

	render::device->SetTransform( D3DTS_WORLD, &render::world );
}

void skybox::unacquire()
{
	if(!acquired)
		return;

	delete [] verts;
	if(dxvertbuf)
		dxvertbuf->Release();
	
	acquired = false;
}

void skybox::release()
{
}

void skybox::con_sky(int argc, char* argv[], void* user)
{	
	if(argc != 2) {
		LOG("usage: /sky <texture>");
		return;
	}

	sprintf(texture, "textures/skybox/%s", argv[1]);
	reset();
}
