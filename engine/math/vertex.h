#pragma once

struct BSPVertex {
	D3DXVECTOR3 pos;
	D3DXVECTOR3	nrm;
	D3DCOLOR	diffuse;
	D3DXVECTOR2 tex1;
	D3DXVECTOR2 tex2;	
	static const DWORD FVF = ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) );		

	BSPVertex operator+(const BSPVertex& rhs) const
	{
		BSPVertex result;

		result.pos = pos + rhs.pos;
		result.nrm = nrm + rhs.nrm;
		result.tex1 = tex1 + rhs.tex1;
		result.tex2 = tex2 + rhs.tex2;
		result.diffuse = D3DXCOLOR(diffuse) + D3DXCOLOR(rhs.diffuse);
		return result;
	}

	BSPVertex operator*(const float rhs) const
	{
		BSPVertex result;
		result.pos = pos * rhs;
		result.nrm = nrm * rhs;
		result.tex1 = tex1 * rhs;
		result.tex2 = tex2 * rhs;
		result.diffuse = D3DXCOLOR(diffuse) * rhs;
		return result;
	}

	bool operator==(const BSPVertex& rhs)
	{
		return (pos == rhs.pos) &&
			(nrm == rhs.nrm) &&
			(tex1 == rhs.tex1) &&
			(tex2 == rhs.tex2) &&
			(diffuse == rhs.diffuse);
	}
};

struct TVertex {
	D3DXVECTOR3 pos;
	D3DXVECTOR3 nrm;
	D3DXVECTOR2 tex1;
	static const DWORD FVF = ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
};

struct SkyVertex {
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex1;
	static const DWORD FVF = ( D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));
};