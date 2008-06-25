#pragma once

namespace q3shader
{
	class Q3ShaderPass
	{
	public:
		typedef vector<string> shader_lines;
		Q3ShaderPass(Q3Shader* shader, const shader_lines& shadertext);
		~Q3ShaderPass();
		void activate();
		void deactivate();

		static void initParseMap();

	protected:
		typedef vector<string> params;
		typedef unordered_map<string, void(Q3ShaderPass::*)(const params&)> parse_map;
		static parse_map s_parseMap;
		bool m_activated;
		string m_mapname;
		texture::DXTexture* m_map;
		bool m_overridedepth;

		typedef function<HRESULT(void)> shader_function;
		typedef vector<shader_function> function_list;
		function_list m_activate;
		function_list m_deactivate;

		Q3Shader* m_shader;

		void parseMap(const params& p);
		void parseClampMap(const params& p);
		void parseRGBGen(const params& p);
		void parseBlendFunc(const params& p);
		void parseTCGen(const params& p);
		void parseTCMod(const params& p);
		void parseAlphaFunc(const params& p);
		void parseDepthFunc(const params& p);
		void parseDepthWrite(const params& p);
		void parseAlphaGen(const params& p);
		void parseDetail(const params& p);
		void parseAnimMap(const params& p);
		void parseAlphaMap(const params& p);

		HRESULT setTexture();
		HRESULT setAnimatedTexture();
		HRESULT setRenderState(D3DRENDERSTATETYPE state, DWORD value);
		HRESULT setSamplerState(DWORD sampler,  D3DSAMPLERSTATETYPE type, DWORD value);
		HRESULT setTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value);
		HRESULT setLightmapTexture();

		float m_animfrequency;
		vector<string> m_animapnames;
		vector<texture::DXTexture*> m_animaps;
	};
}