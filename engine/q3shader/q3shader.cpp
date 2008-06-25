#include "precompiled.h"
#include "q3shader/q3shadercache.h"
#include "q3shader/q3shader.h"
#include "q3shader/q3shaderpass.h"
#include "render/render.h"

namespace q3shader
{
	Q3Shader::parse_map Q3Shader::s_parseMap;
	static void init();

}

using namespace q3shader;

REGISTER_STARTUP_FUNCTION(Q3Shader, Q3Shader::initParseMap, 10);

void Q3Shader::initParseMap()
{
	s_parseMap["surfaceparm"] = &Q3Shader::parseSurfaceParm;
	s_parseMap["light"] = &Q3Shader::parseIgnore;
	s_parseMap["cull"] = &Q3Shader::parseCull;
	s_parseMap["nopicmip"] = &Q3Shader::parseIgnore;
	s_parseMap["light1"] = &Q3Shader::parseIgnore; // no idea what this is; for the editor maybe?
	s_parseMap["tesssize"] = &Q3Shader::parseIgnore; // used by the editor to tesselate faces
	s_parseMap["deformvertexes"] = &Q3Shader::parseDeformVertexes;
	s_parseMap["polygonoffset"] = &Q3Shader::parsePolygonOffset;
	s_parseMap["nomipmaps"] = &Q3Shader::parseNoMipmaps;
	s_parseMap["portal"] = &Q3Shader::parsePortal;
	s_parseMap["skyparms"] = &Q3Shader::parseSkyParms;
	s_parseMap["sort"] = &Q3Shader::parseSort;
	s_parseMap["fogonly"] = &Q3Shader::parseIgnore; // not documented, ignore I guess
	s_parseMap["fogparms"] = &Q3Shader::parseFogParms;
	s_parseMap["entitymergable"] = &Q3Shader::parseIgnore; // not documented, for the editor?
	s_parseMap["cloudparms"] = &Q3Shader::parseCloudParms;
	s_parseMap["lightning"] = &Q3Shader::parseIgnore; // undocumented but I can guess
	s_parseMap["implicitmask"] = &Q3Shader::parseImplicitMask;
	s_parseMap["sky"] = &Q3Shader::parseIgnore; // I do my own skybox drawing
	s_parseMap["foggen"] = &Q3Shader::parseIgnore; // no idea
	s_parseMap["backsided"] = &Q3Shader::parseIgnore; // no idea
}

void Q3Shader::parseImplicitMask(const params& p)
{
	if(p[1] == "-")
	{
		// dunno
	}
	else // anything using this?
	{
		// dunno if this needs to be drawn or not
	}
}

void Q3Shader::parseCloudParms(const params& p)
{
	// undocumented ?
	//INFO("TODO: parse cloud parms");
}

void Q3Shader::parseFogParms(const params& p)
{
	// format is <r> <g> <b> <distance to opaque>
	//INFO("TODO: parse fog parms");
}

void Q3Shader::parseSort(const params& p)
{
	ASSERT(p.size() == 2);

	if(p[1] == "additive")
		m_sortorder = additive;
	else if(p[1] == "nearest")
		m_sortorder = nearest;
	else if(p[1] == "underwater")
		m_sortorder = underwater;
	else if(p[1] == "banner")
		m_sortorder = banner;
	else if(p[1] == "6")
		m_sortorder = banner;
	else if(p[1] == "opaque")
		m_sortorder = opaque;
	else if(p[1] == "5")
		m_sortorder = unset;
	else
		ASSERT(false);
}

void Q3Shader::parseSkyParms(const params& p)
{
	// format is skyParms <farbox> <cloudheight> <nearbox>
	//INFO("TODO: parse skyParms");
}

void Q3Shader::parsePortal(const params& p)
{
	m_sortorder = portal;
}


void Q3Shader::parseIgnore(const params& p)
{
}

void Q3Shader::parseNoMipmaps(const params& p)
{
	m_activate.push_back(bind(&Q3Shader::setSamplerState, this, 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE));
	m_deactivate.push_back(bind(&Q3Shader::setSamplerState, this, 0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC));
}

void Q3Shader::parsePolygonOffset(const params& p)
{
	is_offset = true;
}

void Q3Shader::parseDeformVertexes(const params& p)
{
	//INFO("TODO: handle deformVertexes");
}

void Q3Shader::parseCull(const params& p)
{
	if(p.size() == 1 || p[1] == "front")
	{
		m_activate.push_back(bind(&Q3Shader::setRenderState, this, D3DRS_CULLMODE, D3DCULL_CW));
		m_deactivate.push_back(bind(&Q3Shader::setRenderState, this, D3DRS_CULLMODE, D3DCULL_CCW));
	}
	else if(p[1] == "back" || p[1] == "backsided")
	{
		// this is the default, don't do anything
	}
	else if(p[1] == "disable" || p[1] == "none" || p[1] == "twosided")
	{
		m_activate.push_back(bind(&Q3Shader::setRenderState, this, D3DRS_CULLMODE, D3DCULL_NONE));
		m_deactivate.push_back(bind(&Q3Shader::setRenderState, this, D3DRS_CULLMODE, D3DCULL_CCW));
	}
	else
		ASSERT(false);

}

void Q3Shader::parseSurfaceParm(const params& p)
{
	if(p.size() == 1)
		return;

	if(p[1] == "trans")
		is_transparent = true;
	else if(p[1] == "metalsteps") // ignore
		{}
	else if(p[1] == "nomarks")
	{
	}
	else if(p[1] == "alphashadow")
	{
	}
	else if(p[1] == "nolightmap")
	{
		is_nolightmap = true;
	}
	else if(p[1] == "nonsolid")
	{
		is_noclip = true;
	}
	else if(p[1] == "noimpact")
	{
	}
	else if(p[1] == "nodamage")
	{
	}
	else if(p[1] == "playerclip")
	{
		is_playerclip = true;
	}
	else if(p[1] == "lightfilter") // only used by radiant I believe
	{
	}
	else if(p[1] == "water")
	{
		is_water = true;
	}
	else if(p[1] == "sky")
	{
		is_sky = true;
		is_nodraw = true; // don't draw the sky
	}
	else if(p[1] == "nodraw")
	{
		is_nodraw = true;
	}
	else if (p[1] == "nodrop")
	{
	}
	else if (p[1] == "structural")
	{
	}
	else if(p[1] == "slick")
	{
		is_slick = true;
	}
	else if(p[1] == "origin")
	{
		// no clue
	}
	else if(p[1] == "areaportal")
	{
	}
	else if(p[1] == "detail")
	{
		// dunno, not documented
	}
	else if(p[1] == "clusterportal")
	{
		// only used by map processor
	}
	else if(p[1] == "donotenter")
	{
		// bot only
	}
	else if(p[1] == "fog")
	{
		is_fog = true;
	}
	else if(p[1] == "lava")
	{
		is_lava = true;
	}
	else if(p[1] == "solid")
	{
		// this is the default
	}
	else if(p[1] == "pointlight")
	{
		// used by qeradiant ?
	}
	else if(p[1] == "slime")
	{
		is_slime = true;
	}
	else if(p[1] == "nodlight")
	{
		is_nodynamiclighting = true;
	}
	else if(p[1] == "nomipmaps")
	{
		parseNoMipmaps(p);
	}
	else
		ASSERT(false);
}

HRESULT Q3Shader::setRenderState(D3DRENDERSTATETYPE state, DWORD value)
{
	return render::device->SetRenderState(state, value);
}

HRESULT Q3Shader::setSamplerState( DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value )
{
	return render::device->SetSamplerState(sampler, type, value);
}


Q3Shader::Q3Shader(Q3ShaderCache* cache, const shader_lines& shadertext)
	: m_cache(cache), is_transparent(false), is_nolightmap(false), is_noclip(false), is_playerclip(false), is_offset(false),
	m_sortorder(unset), is_water(false), is_sky(false), is_nodraw(false), is_slick(false), is_fog(false), is_lava(false),
	is_slime(false), is_nodynamiclighting(false)
{
	for(shader_lines::const_iterator it = shadertext.begin(); it != shadertext.end(); ++it)
	{
		if(*it == "{")
		{
			vector<string> passlines;
			while(*(++it) != "}")
				passlines.push_back(*it);
			m_passes.push_back(shared_ptr<Q3ShaderPass>(new Q3ShaderPass(this, passlines)));
		}
		else
		{
			vector<string> v;
			split(v, *it, is_space(), token_compress_on);
			to_lower(v[0]);
			if(starts_with(v[0], string("qer_")) || starts_with(v[0], string("q3map_"))) // ignore all editor and map parms
				continue;
			//ASSERT(s_parseMap.find(v[0]) != s_parseMap.end());
			(this->*s_parseMap[v[0]])(v);
		}
	}

	if(getNbPasses() == 0)
		is_nodraw = true;
}

Q3Shader::~Q3Shader()
{

}

void Q3Shader::activate(texture::DXTexture* lightmap)
{
	m_lightmap = lightmap;
	for(function_list::iterator it = m_activate.begin(); it != m_activate.end(); it++)
		(*it)();
}

void Q3Shader::deactivate()
{
	for(function_list::iterator it = m_deactivate.begin(); it != m_deactivate.end(); it++)
		(*it)();
}

void Q3Shader::activatePass(int index)
{
	m_passes[index]->activate();
}

void Q3Shader::deactivatePass(int index)
{
	m_passes[index]->deactivate();
}
