#include "precompiled.h"
#include "texture/shader.h"
#include "texture/texture.h"
#include "texture/texturecache.h"
#include "render/render.h"
#include "render/dx.h"
#include "vfs/vfs.h"
#include "vfs/file.h"
#include "timer/timer.h"

namespace texture {
	typedef void (* parse_command)(Shader* shader, int argc, char* argv[]);
	typedef void (* init_command)(Shader* shader, DXTexture* texture);
	typedef void (* activate_command)(Shader* shader, DXTexture* texture);
	typedef void (* deactivate_command)(Shader* shader, DXTexture* texture);

	void parse_alphatest(Shader* shader, int argc, char* argv[]);
	void parse_chain(Shader* shader, int argc, char* argv[]);
	void parse_texture(Shader* shader, int argc, char* argv[]);
	void parse_ttransform(Shader* shader, int argc, char* argv[]);
	void parse_blendadd(Shader* shader, int argc, char* argv[]);

	void init_nodraw(Shader* shader, DXTexture* texture);
	void init_sky(Shader* shader, DXTexture* texture);
	void init_chain(Shader* shader, DXTexture* texture);
	void init_alphablend(Shader* shader, DXTexture* texture);
	void init_blendadd(Shader* shader, DXTexture* texture);
	void init_invert(Shader* shader, DXTexture* texture);

	void activate_alphatest(Shader* shader, DXTexture* texture);
	void deactivate_alphatest(Shader* shader, DXTexture* texture);
	void activate_cullmode(Shader* shader, DXTexture* texture);
	void deactivate_cullmode(Shader* shader, DXTexture* texture);
	void activate_alphablend(Shader* shader, DXTexture* texture);
	void deactivate_alphablend(Shader* shader, DXTexture* texture);
	void activate_chain(Shader* shader, DXTexture* texture);
	void activate_blendadd(Shader* shader, DXTexture* texture);
	void deactivate_blendadd(Shader* shader, DXTexture* texture);
	void activate_invert(Shader* shader, DXTexture* texture);
	void deactivate_invert(Shader* shader, DXTexture* texture);
	void activate_ttransform(Shader* shader, DXTexture* texture);
	void deactivate_ttransform(Shader* shader, DXTexture* texture);


	struct {
		char* command;
		unsigned int flag;
		parse_command parse;
		init_command init;
		activate_command activate;
		deactivate_command deactivate;
	} commands[] = {
		//	command				flag		parse				init				activate				deactivate
		{	"TEXTURE",			0x0000,		parse_texture,		NULL,				NULL,					NULL					},
		{	"NODRAW",			0x0001,		NULL,				init_nodraw,		NULL,					NULL					},		
		{	"ALPHATEST",		0x0002,		parse_alphatest,	NULL,				activate_alphatest,		deactivate_alphatest	},
		{	"2SIDED",			0x0004,		NULL,				NULL,				activate_cullmode,		deactivate_cullmode		},
		{	"ALPHABLEND",		0x0008,		NULL,				init_alphablend,	activate_alphablend,	deactivate_alphablend	},
		{	"ANIMATE_CHAIN",	0x0010,		parse_chain,		init_chain,			activate_chain,			NULL					},
		{	"BLEND_ADD",		0x0020,		parse_blendadd,		init_blendadd,		activate_blendadd,		deactivate_blendadd		},
		{	"SKY",				0x0040,		NULL,				init_sky,			NULL,					NULL					},
		{	"TEX_TRANSFORM",	0x0080,		parse_ttransform,	NULL,				activate_ttransform,	deactivate_ttransform	},
		{	"INVERT_TEXTURE",	0x0100,		NULL,				NULL,				activate_invert,		deactivate_invert		},		
		{	NULL,				0x0000,		NULL,				NULL,				NULL,					NULL					}
	};
};

using namespace texture;

Shader::Shader()
{
	refcount = 1;
}

Shader::Shader(char* filename)
{
	vfs::IFilePtr file = vfs::getFile(filename);
	if(!file)
		return;

	load(file);
}

Shader::Shader(vfs::IFilePtr file)
{
	load(file);
}

void Shader::acquire()
{
	refcount++;
}

void Shader::release()
{
	refcount--;
	if(refcount <= 0)
		delete this;
}

bool Shader::load(char* filename)
{
	vfs::IFilePtr file = vfs::getFile(filename);
	if(!file)
		return false;

	return load(file);
}

Shader::~Shader()
{
}

bool Shader::load(vfs::IFilePtr file)
{
	char buf[256];

	name = _strdup(file->filename);
	line = 0;
	flags = 0;

	while(file->readLine(buf, 256)) {
		//LOG("[Shader::load] processing \"%s\"", buf);
		line++;

		char* comment = strstr(buf, "//");
		if(comment) *comment = 0;

		strip(buf);

		if(!buf[0]) continue;

		char * name = buf;
		char* args = strchr(name, ' ');
		int argc;
		char* argv[12];

		if(args)
			*(args++) = 0;
		argv[0] = name;
		argc = countArgs(args) + 1;
		for(int arg_idx = 1; arg_idx < argc; arg_idx++)
			argv[arg_idx] = getToken(&args, " \t");

		int command_idx;
		for(command_idx = 0; commands[command_idx].command; command_idx++)
			if(!_stricmp(name, commands[command_idx].command)) {
				flags |= commands[command_idx].flag;
				if(commands[command_idx].parse)
					commands[command_idx].parse(this, argc, argv);
				break;
			}

		if(!commands[command_idx].command)
			LOG("[Shader::load] %s[%i]: unknown command \"%s\"", file->filename, line, name);
	}

	return true;
}


void Shader::init(DXTexture* texture)
{
	for(int command_idx = 0; commands[command_idx].command; command_idx++)
		if(flags & commands[command_idx].flag)
			if(commands[command_idx].init)
				commands[command_idx].init(this, texture);
}

bool Shader::activate(DXTexture* texture)
{
	//FRAMEDO(LOG("[Shader] activating %s", name));
	active_shader = this;	
	for(int command_idx = 0; commands[command_idx].command; command_idx++)
		if(flags & commands[command_idx].flag)
			if(commands[command_idx].activate)
				commands[command_idx].activate(this, texture);

	return true;
}

void Shader::deactivate(DXTexture* texture)
{
	//FRAMEDO(LOG("[Shader] deactivating %s", name));
	active_shader = NULL;
	for(int command_idx = 0; commands[command_idx].command; command_idx++)
		if(flags & commands[command_idx].flag)
			if(commands[command_idx].deactivate)
				commands[command_idx].deactivate(this, texture);
}

void texture::parse_alphatest(Shader* shader, int argc, char* argv[])
{
	if(argc != 2) {
		LOG("[parse_alphatest] %s[%i]: ALPHATEST takes 1 argument", shader->name, shader->line);
		return;
	}

	sscanf(argv[1], "%i", &shader->alphamask);
}

void texture::parse_chain(Shader* shader, int argc, char* argv[])
{
	if(argc != 3) {
		LOG("[parse_alphatest] %s[%i]: ANIMATE_CHAIN takes 2 argument", shader->name, shader->line);
		return;
	}

	sscanf(argv[1], "%i", &shader->chain_frames);
	sscanf(argv[2], "%i", &shader->chain_time);
	shader->textures = new DXTexture*[shader->chain_frames];
	for(int idx = 0; idx < shader->chain_frames; idx++)
		shader->textures[idx] = NULL;
	shader->chain_last_time = 0;
}

void texture::parse_texture(Shader* shader, int argc, char* argv[])
{
	int tex_idx = 0;
	while(shader->textures[tex_idx])
		tex_idx++;

	shader->textures[tex_idx] = getTexture(argv[1], false);
}

void texture::parse_blendadd(Shader* shader, int argc, char* argv[])
{
	shader->blend_add_src = D3DBLEND_ONE;
	shader->blend_add_dst = D3DBLEND_ONE;

	if(argc == 1)
		return;

	if(argc > 1)
		sscanf(argv[1], "%i", &shader->blend_add_src);

	if(argc > 2)
		sscanf(argv[2], "%i", &shader->blend_add_dst);
}

void texture::parse_ttransform(Shader* shader, int argc, char* argv[])
{
	if(argc != 8) {
		LOG("[parse_ttransform] %s[%i]: TEX_TRANSFORM time tx ty tz rotx roty rotz", shader->name, shader->line);
		return;
	}

	sscanf(argv[1], "%i", &shader->ttrans_time);
	sscanf(argv[2], "%f", &shader->ttrans_pos.x);
	sscanf(argv[3], "%f", &shader->ttrans_pos.y);
	sscanf(argv[4], "%f", &shader->ttrans_pos.z);
	sscanf(argv[5], "%f", &shader->ttrans_rot.x);
	sscanf(argv[6], "%f", &shader->ttrans_rot.y);
	sscanf(argv[7], "%f", &shader->ttrans_rot.z);
}


void texture::init_nodraw(Shader* shader, DXTexture* texture)
{
	texture->draw = false;
}

void texture::init_sky(Shader* shader, DXTexture* texture)
{
	texture->sky = true;
}


void texture::init_alphablend(Shader* shader, DXTexture* texture)
{	
	texture->is_transparent = true;
}

void texture::init_blendadd(Shader* shader, DXTexture* texture)
{	
	texture->is_transparent = true;
}

void texture::init_chain(Shader* shader, DXTexture* texture)
{	
	texture->use_texture = false;
	shader->chain_current = 0;
}


void texture::activate_alphatest(Shader* shader, DXTexture* texture)
{
	render::device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	render::device->SetRenderState( D3DRS_ALPHAREF, shader->alphamask );
	render::device->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
}

void texture::deactivate_alphatest(Shader* shader, DXTexture* texture)
{
	render::device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );	
	render::device->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
}

void texture::activate_alphablend(Shader* shader, DXTexture* texture)
{
	render::device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	render::device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	render::device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	render::device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
}

void texture::deactivate_alphablend(Shader* shader, DXTexture* texture)
{
	render::device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	render::device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
}

void texture::activate_blendadd(Shader* shader, DXTexture* texture)
{
	render::device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	render::device->SetRenderState( D3DRS_SRCBLEND, shader->blend_add_src );
	render::device->SetRenderState( D3DRS_DESTBLEND, shader->blend_add_dst );
	render::device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
}

void texture::deactivate_blendadd(Shader* shader, DXTexture* texture)
{
	render::device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );	
	render::device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
}

void texture::activate_invert(Shader* shader, DXTexture* texture)
{
	render::device->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
	//device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT );
}

void texture::deactivate_invert(Shader* shader, DXTexture* texture)
{
	render::device->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
	//device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
}

void texture::activate_cullmode(Shader* shader, DXTexture* texture)
{
	render::device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
}

void texture::deactivate_cullmode(Shader* shader, DXTexture* texture)
{
	render::device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
}

void texture::activate_chain(Shader* shader, DXTexture* texture)
{
	if((shader->chain_last_time + shader->chain_time) < timer::game_ms) {
		shader->chain_last_time = timer::game_ms;
		shader->chain_current++;
	}

	if(shader->chain_current >= shader->chain_frames)
		shader->chain_current = 0;

	shader->textures[shader->chain_current]->activate(false);
}

void texture::activate_ttransform(Shader* shader, DXTexture* texture)
{
	// todo
}

void texture::deactivate_ttransform(Shader* shader, DXTexture* texture)
{
	// todo
}
