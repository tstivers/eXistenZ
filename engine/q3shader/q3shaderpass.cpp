#include "precompiled.h"
#include "q3shader/q3shadercache.h"
#include "q3shader/q3shader.h"
#include "q3shader/q3shaderpass.h"
#include "render/render.h"
#include "texture/texturecache.h"
#include "texture/texture.h"
#include "timer/timer.h"

namespace q3shader
{
	Q3ShaderPass::parse_map Q3ShaderPass::s_parseMap;
	static void init();
	static unordered_map<string, DWORD> blend_modes;
}

using namespace q3shader;

REGISTER_STARTUP_FUNCTION(Q3ShaderPass, Q3ShaderPass::initParseMap, 10);

void Q3ShaderPass::initParseMap()
{
	blend_modes["gl_one"] = D3DBLEND_ONE;
	blend_modes["gl_zero"] = D3DBLEND_ZERO;
	blend_modes["gl_dst_color"] = D3DBLEND_DESTCOLOR;
	blend_modes["gl_one_minus_dst_color"] = D3DBLEND_INVDESTCOLOR;
	blend_modes["gl_src_alpha"] = D3DBLEND_SRCALPHA;
	blend_modes["gl_one_minus_src_alpha"] = D3DBLEND_INVSRCALPHA;
	blend_modes["gl_src_color"] = D3DBLEND_SRCCOLOR;
	blend_modes["gl_one_minus_src_color"] = D3DBLEND_INVSRCCOLOR;
	blend_modes["gl_dst_alpha"] = D3DBLEND_DESTALPHA;
	blend_modes["gl_one_minus_dst_alpha"] = D3DBLEND_INVDESTALPHA;

	Q3ShaderPass::s_parseMap["map"] = &Q3ShaderPass::parseMap;
	Q3ShaderPass::s_parseMap["rgbgen"] = &Q3ShaderPass::parseRGBGen;
	Q3ShaderPass::s_parseMap["blendfunc"] = &Q3ShaderPass::parseBlendFunc;
	Q3ShaderPass::s_parseMap["tcgen"] = &Q3ShaderPass::parseTCGen;
	Q3ShaderPass::s_parseMap["tcmod"] = &Q3ShaderPass::parseTCMod;
	Q3ShaderPass::s_parseMap["clampmap"] = &Q3ShaderPass::parseClampMap;
	Q3ShaderPass::s_parseMap["alphafunc"] = &Q3ShaderPass::parseAlphaFunc;
	Q3ShaderPass::s_parseMap["depthfunc"] = &Q3ShaderPass::parseDepthFunc;
	Q3ShaderPass::s_parseMap["depthwrite"] = &Q3ShaderPass::parseDepthWrite;
	Q3ShaderPass::s_parseMap["alphagen"] = &Q3ShaderPass::parseAlphaGen;
	Q3ShaderPass::s_parseMap["detail"] = &Q3ShaderPass::parseDetail;
	Q3ShaderPass::s_parseMap["animmap"] = &Q3ShaderPass::parseAnimMap;
	Q3ShaderPass::s_parseMap["alphamap"] = &Q3ShaderPass::parseAlphaMap;
}

void Q3ShaderPass::parseAlphaMap(const params& p)
{
	// this is not used
}

void Q3ShaderPass::parseAnimMap(const params& p)
{
	m_animfrequency = 1000.0 / lexical_cast<float>(p[1]);
	for(int i = 2; i < p.size(); i++)
		m_animapnames.push_back(p[i]);
	m_activate.push_back(bind(&Q3ShaderPass::setAnimatedTexture, this));
}


void Q3ShaderPass::parseDetail(const params& p)
{
	// this is not used
}

void Q3ShaderPass::parseAlphaGen(const params& p)
{
	// TODO: parse alpha gen
}

void Q3ShaderPass::parseDepthWrite(const params& p)
{
	m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ZWRITEENABLE, TRUE));
	m_overridedepth = true;
}

void Q3ShaderPass::parseDepthFunc(const params& p)
{
	if(p[1] == "equal")
		m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ZFUNC, D3DCMP_EQUAL));
	else
		ASSERT(false);

	m_deactivate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ZFUNC, D3DCMP_LESSEQUAL));
}

void Q3ShaderPass::parseAlphaFunc(const params& p)
{
	if(p[1] == "gt0")
	{
		m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ALPHAFUNC, D3DCMP_GREATER));
		m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ALPHAREF, 0));
	}
	else if(p[1] == "ge128")
	{
		m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL));
		m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ALPHAREF, 128));
	}
	else if (p[1] == "lt128")
	{
		m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ALPHAFUNC, D3DCMP_LESS));
		m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ALPHAREF, 128));
	}
	else
		ASSERT(false);

	m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ALPHATESTENABLE, TRUE));
	m_deactivate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ALPHATESTENABLE, FALSE));
}

void Q3ShaderPass::parseClampMap(const params& p)
{
	//m_activate.push_back(bind(&Q3ShaderPass::setSamplerState, this, 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
	//m_activate.push_back(bind(&Q3ShaderPass::setSamplerState, this, 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));
	//m_deactivate.push_back(bind(&Q3ShaderPass::setSamplerState, this, 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
	//m_deactivate.push_back(bind(&Q3ShaderPass::setSamplerState, this, 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
	parseMap(p);
}

void Q3ShaderPass::parseTCMod(const params& p)
{
	// TODO: parse tcmod params
}

void Q3ShaderPass::parseTCGen(const params& p)
{
	// TODO: parse tcgen params
}

void Q3ShaderPass::parseBlendFunc(const params& p)
{
	if(p.size() == 2) // shortcut
	{
		params sp;
		sp.push_back("blendfunc");
		if(p[1] == "add" || p[1] == "gl_add")
		{
			sp.push_back("gl_one");
			sp.push_back("gl_one");
		}
		else if(p[1] == "filter")
		{
			sp.push_back("gl_dst_color");
			sp.push_back("gl_zero");
		}
		else if(p[1] == "blend")
		{
			sp.push_back("gl_src_alpha");
			sp.push_back("gl_one_minus_src_alpha");
		}
		else
			ASSERT(false);

		return parseBlendFunc(sp);
	}

	ASSERT(blend_modes.find(p[1]) != blend_modes.end());
	ASSERT(blend_modes.find(p[2]) != blend_modes.end());

	m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_SRCBLEND, blend_modes[p[1]]));
	m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_DESTBLEND, blend_modes[p[2]]));
	m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ALPHABLENDENABLE, TRUE));
	m_activate.push_back(bind(&Q3ShaderPass::setTextureStageState, this, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1));
	if(!m_overridedepth)
		m_activate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ZWRITEENABLE, FALSE));
	m_deactivate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ALPHABLENDENABLE, FALSE));
	m_deactivate.push_back(bind(&Q3ShaderPass::setRenderState, this, D3DRS_ZWRITEENABLE, TRUE));
}

void Q3ShaderPass::parseRGBGen(const params& p)
{
	if(p[1] == "identity")
	{
		// do nothing? material should be 1.0 1.0 1.0
	}
	else
	{
		// TODO: parse rgbgen params
	}
}

void Q3ShaderPass::parseMap(const params& p)
{
	if(p[1] == "$lightmap")
	{
		useLightmapHack = true;
		//m_activate.push_back(bind(&Q3ShaderPass::setLightmapTexture, this));
		m_activate.push_back(bind(&Q3ShaderPass::setTextureStageState, this, 0, D3DTSS_TEXCOORDINDEX, 1));
		m_deactivate.push_back(bind(&Q3ShaderPass::setTextureStageState, this, 0, D3DTSS_TEXCOORDINDEX, 0));
		// hacky: activate z-writing for anything that accepts a lightmap
		//parseDepthWrite(p);
		m_shader->is_useslightmap = true;
	}
	else
	{
		ASSERT(m_mapname.empty());
		m_mapname = p[1];
		m_activate.push_back(bind(&Q3ShaderPass::setTexture, this));
	}
}

Q3ShaderPass::Q3ShaderPass(Q3Shader* shader, const shader_lines& shadertext)
	: m_shader(shader), m_activated(false), m_animfrequency(0), m_overridedepth(false), m_current_map(0),
	m_lastswitched(0.0), useLightmapHack(false)
{
	for(shader_lines::const_iterator it = shadertext.begin(); it != shadertext.end(); ++it)
	{
		vector<string> v;
		split(v, *it, is_space(), token_compress_on);
		to_lower(v[0]);
		(this->*s_parseMap[v[0]])(v);
	}
}

Q3ShaderPass::~Q3ShaderPass()
{
}

void Q3ShaderPass::activate()
{
	for(function_list::iterator it = m_activate.begin(); it != m_activate.end(); it++)
		(*it)();
}

void Q3ShaderPass::deactivate()
{
	for(function_list::iterator it = m_deactivate.begin(); it != m_deactivate.end(); it++)
		(*it)();
}

HRESULT Q3ShaderPass::setTexture()
{
	if(!m_activated)
	{
		m_map = texture::getTexture(m_mapname.c_str());
		if(!m_map)
		{
			m_mapname = string(m_mapname.begin(), boost::find_last(m_mapname, ".").begin());
			m_map = texture::getTexture(m_mapname.c_str());
		}
		m_activated = true;
		if(!m_map)
			INFO("ERROR: unable to load texture: %s", m_mapname.c_str());
	}

	if(m_map)
		return render::device->SetTexture(0, m_map->texture);
	else
		return render::device->SetTexture(0, NULL);
}

HRESULT Q3ShaderPass::setAnimatedTexture()
{
	if(!m_activated)
	{
		for(vector<string>::iterator it = m_animapnames.begin(); it != m_animapnames.end(); ++it)
		{
			string mapname = *it;
			texture::DXTexture* map = texture::getTexture(mapname.c_str());
			if(!map)
			{
				mapname = string(mapname.begin(), boost::find_last(mapname, ".").begin());
				map = texture::getTexture(mapname.c_str());
			}
			m_animaps.push_back(map);
		}
		m_activated = true;
	}


	if(m_lastswitched + m_animfrequency < timer::game_ms)
	{
		m_current_map++;
		m_lastswitched = timer::game_ms;
		if(m_current_map == m_animaps.size())
			m_current_map = 0;
	}

	if(m_animaps[m_current_map])
		return render::device->SetTexture(0, m_animaps[m_current_map]->texture);
	else
		return render::device->SetTexture(0, NULL);
}

HRESULT Q3ShaderPass::setRenderState(D3DRENDERSTATETYPE state, DWORD value)
{
	return render::device->SetRenderState(state, value);
}

HRESULT Q3ShaderPass::setSamplerState( DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value )
{
	return render::device->SetSamplerState(sampler, type, value);
}

HRESULT Q3ShaderPass::setTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value)
{
	return render::device->SetTextureStageState(stage, type, value);
}

HRESULT Q3ShaderPass::setLightmapTexture()
{
	//if(m_shader->m_lightmap)
	//	return render::device->SetTexture(0, m_shader->m_lightmap->texture);
	//else
		return render::device->SetTexture(0, NULL);
}