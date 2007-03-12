/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#include "precompiled.h"
#include "q3shader/q3shadercache.h"
#include "q3shader/q3shader.h"
#include "console/console.h"
#include "settings/settings.h"
#include "vfs/vfs.h"
#include "vfs/file.h"
#include "texture/texturecache.h"
#include "render/render.h"

namespace q3shader {
	
	typedef void (* parse_command)(Q3Shader* shader, int argc, char* argv[]);

	void parse_surfaceparm(Q3Shader* shader, int argc, char* argv[]);
	void parse_cull(Q3Shader* shader, int argc, char* argv[]);
	void parse_map(Q3Shader* shader, int argc, char* argv[]);
	void parse_blend(Q3Shader* shader, int argc, char* argv[]);
	void parse_rgbgen(Q3Shader* shader, int argc, char* argv[]);
	void parse_alphafunc(Q3Shader* shader, int argc, char* argv[]);
	void parse_depthfunc(Q3Shader* shader, int argc, char* argv[]);

	struct {
		char* command;
		int flag;
		parse_command parse;
	} commands[] = {
		// these get ignored
		{ "qer_*",			0x00, NULL }, // ignore qer crap
		{ "q3map_*",		0x00, NULL }, // and q3map crap
		{ "tessSize",		0x00, NULL }, // only for bsp compiler

		// these need processed
		{ "tcmod",			FLAG_TCMOD, NULL }, // ignore for now
		{ "depthwrite",		FLAG_DEPTHWRITE, NULL }, // only need the flag
		{ "depthfunc",		FLAG_DEPTHFUNC,	parse_depthfunc },
		{ "surfaceparm",	0x00, parse_surfaceparm },
		{ "cull",			FLAG_CULL, parse_cull },
		{ "map",			FLAG_MAP, parse_map },
		{ "blendfunc",		FLAG_BLEND, parse_blend },
		{ "rgbgen",			0x00, parse_rgbgen },
		{ "alphafunc",		FLAG_ALPHATEST, parse_alphafunc },
		{ NULL, NULL }
	};

	struct {
		char* key;
		int value;
	} constants[] = {
		// surfaceparm
		{ "trans",			Q3SURF_TRANS },
		{ "nomarks",		Q3SURF_NOMARKS },
		{ "nonsolid",		Q3SURF_NONSOLID },
		{ "nolightmap",		Q3SURF_NOLIGHTMAP },
		{ "nodraw",			Q3SURF_NODRAW },
		{ "lava",			0x0020 },
		{ "noimpact",		0x0040 },
		{ "playerclip",		0x0080 },
		{ "structural",		0x0100 },
		{ "hint",			0x0200 },
		{ "donotenter",		0x0400 },
		{ "nodamage",		0x0800 },
		{ "alphashadow",	0x1000 },
		{ "slime",			0x2000 },
		{ "nodrop",			0x4000 },
		{ "clusterportal",	0x8000 },
		{ "water",			0x010000 },
		{ "metalsteps",		0x020000 },
		{ "areaportal",		0x040000 },

		// cull
		{ "none",				D3DCULL_NONE },
		{ "disable",			D3DCULL_NONE },

		// blend modes
		{ "GL_ONE",					D3DBLEND_ONE },
		{ "GL_ZERO",				D3DBLEND_ZERO },
		{ "GL_SRC_ALPHA",			D3DBLEND_SRCALPHA },
		{ "GL_DST_ALPHA",			D3DBLEND_DESTALPHA },
		{ "GL_ONE_MINUS_SRC_ALPHA", D3DBLEND_INVSRCALPHA },
		{ "GL_ONE_MINUS_DST_ALPHA", D3DBLEND_INVDESTALPHA },
		{ "GL_SRC_COLOR",			D3DBLEND_SRCCOLOR },
		{ "GL_DST_COLOR",			D3DBLEND_DESTCOLOR },
		{ "GL_ONE_MINUS_SRC_COLOR",	D3DBLEND_INVSRCCOLOR },
		{ "GL_ONE_MINUS_DST_COLOR",	D3DBLEND_INVDESTCOLOR },

		// end
		{ NULL, 0 }
	};

	int getConstant(const char* key);
};

using namespace q3shader;

Q3Shader::Q3Shader(const char* name)
{
	this->name = _strdup(name);
	this->flags = 0;
	this->surfaceparms = 0;
	this->passes = 0;
	this->line = 0;
}

Q3Shader::Q3Shader(const char* name, const char* filename)
{
	this->name = _strdup(name);
	this->filename = _strdup(filename);
	this->flags = 0;
	this->surfaceparms = 0;
	this->passes = 0;
	this->line = 0;
}


Q3Shader::~Q3Shader()
{
}


bool Q3Shader::load(const char* filename)
{
	// open file and skip down to our section
	vfs::IFilePtr file = vfs::getFile(filename);
	if(!file)
		return false;

	this->filename = _strdup(filename);

	char buf[1024];
	line = 0;
	int level = 0;

	while(file->readLine(buf, 1024)) {
		
		line++;

		char* comment = strstr(buf, "//");
		if(comment) *comment = 0;

		strip(buf);

		if(!buf[0]) continue;
		//LOG2("[q3shader::parseShader] processing \"%s\"", buf);
		char* token = buf;
		char* this_token;
		while(this_token = getToken(&token, " \t")) {
			if(this_token[0] == '{')
				level++;
			else if(this_token[0] == '}')
				level--;
			else if(this_token[0] && level == 0) {
				if(!_stricmp(this_token, this->name)) {
					file->readLine(buf, 1024);
					line++;
					parse(file);

					return true;
				}
			}
		}
	}

	return false;
}

bool Q3Shader::parse(vfs::IFilePtr file)
{
	char buf[1024];

	while(file->readLine(buf, 1024)) {
		
		line++;

		char* comment = strstr(buf, "//");
		if(comment) *comment = 0;
		strip(buf);
		if(!buf[0]) continue;
			
		if(buf[0] == '{') {
			passes++;
			Q3Shader* shader = new Q3Shader(this->name, this->filename);
			shader->line = line;
			shader->parse(file);
			pass.push_back(shader);
			this->line = shader->line;
		}
		else if(buf[0] == '}') {
			return true;
		}
		else {
			parseLine(buf);			
		}
	}

	return true;
}

void Q3Shader::parseLine(char* line)
{
	LOG2("[Q3Shader::parseLine] parsing [%s]", line);
	char * name = line;
	char* args = strchr(name, ' ');
	int argc;
	char* argv[32];

	if(args)
		*(args++) = 0;
	argv[0] = name;
	argc = countArgs(args) + 1;
	for(int arg_idx = 1; arg_idx < argc; arg_idx++)
		argv[arg_idx] = getToken(&args, " \t");
	
	int command_idx;
	for(command_idx = 0; commands[command_idx].command; command_idx++)
		if(wildcmp(commands[command_idx].command, name)) {
			flags |= commands[command_idx].flag;
			if(commands[command_idx].parse)
				commands[command_idx].parse(this, argc, argv);
			break;
		}

	if(!commands[command_idx].command)
		LOG2("[Q3Shader::parseLine] unknown command \"%s\"", name);
}

void q3shader::parse_surfaceparm(Q3Shader* shader, int argc, char* argv[])
{
	strip(argv[1]);
	int flag = getConstant(argv[1]);
	if(flag != 0) {
		shader->surfaceparms |= flag;
		return;
	}

	LOG4("[parse_surfaceparm] %s[%i] : unknown parm \"%s\"", shader->filename, shader->line, argv[1]);
}

void q3shader::parse_cull(Q3Shader* shader, int argc, char* argv[])
{
	strip(argv[1]);
	int cullmode = getConstant(argv[1]);
	if(cullmode != 0) {
		shader->cullmode = cullmode;
		return;
	}

	LOG4("[parse_cull] %s[%i] : unknown parm \"%s\"", shader->filename, shader->line, argv[1]);
}

void q3shader::parse_map(Q3Shader* shader, int argc, char* argv[])
{
	strip(argv[1]);
	
	if(!_stricmp(argv[1], "$lightmap")) {
		shader->texture.push_back((texture::DXTexture*) 0x01);
		return;
	}

	char* ext = strrchr(argv[1], '.');
	if(ext)
		*ext = 0;

	//LOG4("[parse_map] %s[%i] : loading \"%s\"", shader->filename, shader->line, argv[1]);

	texture::DXTexture* texture = texture::getTexture(argv[1]);
	if(texture) {
		shader->texture.push_back(texture);
		return;
	}

	LOG4("[parse_map] %s[%i] : couldn't load \"%s\"", shader->filename, shader->line, argv[1]);
}

void q3shader::parse_blend(Q3Shader* shader, int argc, char* argv[])
{
	strip(argv[1]);
	
	if(!_stricmp(argv[1], "add")) {
		shader->src_blend = D3DBLEND_ONE;
		shader->dest_blend = D3DBLEND_ONE;
		return;
	}

	if(!_stricmp(argv[1], "filter")) {
		shader->src_blend = D3DBLEND_ZERO;
		shader->dest_blend = D3DBLEND_SRCCOLOR;
		return;
	}

	if(!_stricmp(argv[1], "blend")) {
		shader->src_blend = D3DBLEND_SRCALPHA;
		shader->dest_blend = D3DBLEND_INVSRCALPHA;
		return;
	}

	int blend_mode = getConstant(argv[1]);
	if(blend_mode != 0) {
		shader->src_blend = blend_mode;		
	} else {
		LOG4("[parse_blend] %s[%i] : unknown blend mode \"%s\"", shader->filename, shader->line, argv[1]);
	}

	blend_mode = getConstant(argv[2]);
	if(blend_mode != 0) {
		shader->src_blend = blend_mode;		
	} else {
		LOG4("[parse_blend] %s[%i] : unknown blend mode \"%s\"", shader->filename, shader->line, argv[2]);
	}
}

void q3shader::parse_alphafunc(Q3Shader* shader, int argc, char* argv[])
{
	strip(argv[1]);

	if(!_stricmp(argv[1], "gt0")) {
		shader->alpharef = 0;
		shader->alphafunc = D3DCMP_GREATER;		
		return;
	}

	if(!_stricmp(argv[1], "lt128")) {
		shader->alpharef = 128;
		shader->alphafunc = D3DCMP_LESS;		
		return;
	}

	if(!_stricmp(argv[1], "ge128")) {
		shader->alpharef = 128;
		shader->alphafunc = D3DCMP_GREATEREQUAL;		
		return;
	}

	LOG4("[parse_alphafunc] %s[%i] : unknown test mode \"%s\"", shader->filename, shader->line, argv[1]);
}

void q3shader::parse_depthfunc(Q3Shader* shader, int argc, char* argv[])
{
	strip(argv[1]);

	if(!_stricmp(argv[1], "lequal")) {		
		shader->depthfunc = D3DCMP_LESSEQUAL;		
		return;
	}

	if(!_stricmp(argv[1], "equal")) {		
		shader->alphafunc = D3DCMP_EQUAL;		
		return;
	}

	LOG4("[parse_depthfunc] %s[%i] : unknown depth func \"%s\"", shader->filename, shader->line, argv[1]);
}


void q3shader::parse_rgbgen(Q3Shader* shader, int argc, char* argv[])
{
	strip(argv[1]);

	if(!_stricmp(argv[1], "identity") || !_stricmp(argv[1], "identityLighting"))
		return;

	LOG4("[parse_rgbgen] %s[%i] : unknown rgbgen mode \"%s\"", shader->filename, shader->line, argv[1]);
}


int q3shader::getConstant(const char* key)
{
	for(int const_idx = 0; constants[const_idx].key; const_idx++)
		if(!_stricmp(key, constants[const_idx].key))
			return constants[const_idx].value;

	return 0;
}

#define BLEH if(!_stricmp(this->name, "textures/base_wall/protobanner"))

bool Q3Shader::activate(texture::DXTexture* lightmap, int pass)
{
	this->lightmap = lightmap;
	
	BLEH {
		LOG("activating banner");
	}

	if(pass == 0)
	{
		if(surfaceparms & Q3SURF_NODRAW)
			return false;
		
		if(this->flags & FLAG_STD_TEXTURE) {
			if(texture[0])
				render::device->SetTexture(0, texture[0]->texture);

			if(lightmap && render::lightmap) {
				render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
				render::device->SetTexture(1, lightmap->texture);
			}
			else
				render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			return true;
		}

		if(flags & FLAG_MAP) {
			if(texture[0] == (texture::DXTexture*)0x01) {
				if(lightmap && render::lightmap) {
					//render::device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1);
					render::device->SetTexture(0, lightmap->texture);
				}
			}
			else
				render::device->SetTexture(0, texture[0]->texture);
		}

		if(flags & FLAG_BLEND) {
			render::device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			render::device->SetRenderState( D3DRS_SRCBLEND, src_blend );
			render::device->SetRenderState( D3DRS_DESTBLEND, dest_blend );
			if(!(flags & FLAG_DEPTHWRITE))
				render::device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		}

		if(flags & FLAG_CULL) {
			render::device->SetRenderState( D3DRS_CULLMODE, cullmode );
		}

		if(flags & FLAG_DEPTHFUNC) {
			render::device->SetRenderState( D3DRS_ZFUNC, depthfunc );
		}

		if(flags & FLAG_ALPHATEST) {
			render::device->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			render::device->SetRenderState( D3DRS_ALPHAREF, alpharef );
			render::device->SetRenderState( D3DRS_ALPHAFUNC, alphafunc );
		}

		return true;
	}
	else
		return this->pass[pass - 1]->activate(lightmap);
		
	return true;
}

void Q3Shader::deactivate(int pass)
{
	if(pass == 0)
	{
		if(surfaceparms & Q3SURF_NODRAW)
			return;

		if(flags & FLAG_STD_TEXTURE) {
			render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);			
			return;
		}

		if(flags & FLAG_MAP) {
			render::device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
			render::device->SetTexture(0, NULL);
		}

		if(flags & FLAG_BLEND) {
			render::device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			if(!(flags & FLAG_DEPTHWRITE))
				render::device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
		}

		if(flags & FLAG_CULL) {
			render::device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		}

		if(flags & FLAG_DEPTHFUNC) {
			render::device->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
		}

		if(flags & FLAG_ALPHATEST) {
			render::device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		}

		return;
	}
	else
		this->pass[pass - 1]->deactivate();
}