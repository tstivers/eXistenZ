#pragma once

namespace texture
{
	class DXTexture;
}

namespace q3shader
{
	class Q3ShaderCache;
	class Q3Shader;
	class Q3ShaderPass;

	class Q3Shader
	{
		friend class Q3ShaderPass;

	public:
		
		enum SORT_ORDER
		{
			unset,
			portal,
			sky,
			opaque,
			banner,
			underwater,
			additive,
			nearest
		};

		typedef vector<string> shader_lines;

		Q3Shader(Q3ShaderCache* cache, const shader_lines& shadertext);
		~Q3Shader();

		void activate();
		void activatePass(int index);
		bool isPassLightmap(int index);
		void deactivatePass(int index);
		void deactivate();

		int getNbPasses(){return m_passes.size(); }

		static void initParseMap();

		bool is_transparent;
		bool is_nolightmap;
		bool is_noclip;
		bool is_playerclip;
		bool is_offset;
		bool is_water;
		bool is_sky;
		bool is_nodraw;
		bool is_slick;
		bool is_fog;
		bool is_lava;
		bool is_slime;
		bool is_nodynamiclighting;
		bool is_useslightmap;

		SORT_ORDER m_sortorder;

	protected:
		typedef vector<string> params;

		void parseIgnore(const params& p);
		void parseSurfaceParm(const params& p);
		void parseCull(const params& p);
		void parseDeformVertexes(const params& p);
		void parsePolygonOffset(const params& p);
		void parseNoMipmaps(const params& p);
		void parsePortal(const params& p);
		void parseSkyParms(const params& p);
		void parseSort(const params& p);
		void parseFogParms(const params& p);
		void parseCloudParms(const params& p);
		void parseImplicitMask(const params& p);

		HRESULT setRenderState(D3DRENDERSTATETYPE state, DWORD value);
		HRESULT setSamplerState(DWORD sampler,  D3DSAMPLERSTATETYPE type, DWORD value);

		typedef function<HRESULT(void)> shader_function;
		typedef vector<shader_function> function_list;
		function_list m_activate;
		function_list m_deactivate;

		Q3ShaderCache* m_cache;
		
		typedef unordered_map<string, void(Q3Shader::*)(const params&)> parse_map;
		static parse_map s_parseMap;
		
		typedef vector<shared_ptr<Q3ShaderPass>> pass_list;
		pass_list m_passes;
		texture::DXTexture* m_lightmap; // hacky
	};
}