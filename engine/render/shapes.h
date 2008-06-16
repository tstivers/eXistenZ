#pragma once

namespace texture
{
	class DXTexture;
	struct Material;
}

namespace render
{
	struct LineVertex
	{
		D3DXVECTOR3 pos;
		D3DCOLOR color;
		LineVertex(float x, float y, float z, DWORD color)
			: pos(x, y, z), color(color) {}
		LineVertex() {}
	};
	void drawLine(const D3DXVECTOR3* vertices, int count, D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255), bool clip = true);
	void drawLine(const D3DXVECTOR3& start, const D3DXVECTOR3& end, D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255));
	void drawLines(const D3DXVECTOR3* segements, int count, D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255));
	void drawAABBox(const D3DXVECTOR3* min, const D3DXVECTOR3* max, D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255));
	void drawBox(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale, texture::DXTexture* texture = NULL, texture::Material* lighting = NULL);
	void drawSphere(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale, texture::DXTexture* texture = NULL, texture::Material* lighting = NULL);
	void draw3DText(const string text, const D3DXVECTOR3& pos, DWORD flags);
	void drawPoints(const D3DXVECTOR3* points, int count, D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255));
	void drawPoint(const D3DXVECTOR3* POINT, D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255));
	void drawTriangles(const D3DXVECTOR3* vertices, int count, D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255));
	void drawTriangle(const D3DXVECTOR3* vertices, D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255));
	void drawLineSegments(const LineVertex* verts, int count);
	void drawAxis(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot);
	void drawAxis(const D3DXMATRIX& transform);
	void releaseLine();
	void releaseFont();
};