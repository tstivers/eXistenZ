#include "precompiled.h"
#include "render/render.h"
#include "render/frustrum.h"

namespace render {
	enum {
		PLANE_LEFT,
		PLANE_RIGHT,
		PLANE_NEAR,
		PLANE_FAR,
		PLANE_TOP,
		PLANE_BOTTOM
	};

	D3DXPLANE clip_plane[6];
};

void render::calc_frustrum(void)
{
	D3DXMATRIX mat = render::view * render::projection;

	clip_plane[PLANE_LEFT].a = mat._14 + mat._11;
	clip_plane[PLANE_LEFT].b = mat._24 + mat._21;
	clip_plane[PLANE_LEFT].c = mat._34 + mat._31;
	clip_plane[PLANE_LEFT].d = mat._44 + mat._41;

	clip_plane[PLANE_RIGHT].a = mat._14 - mat._11;
	clip_plane[PLANE_RIGHT].b = mat._24 - mat._21;
	clip_plane[PLANE_RIGHT].c = mat._34 - mat._31;
	clip_plane[PLANE_RIGHT].d = mat._44 - mat._41;

	clip_plane[PLANE_TOP].a = mat._14 - mat._12;
	clip_plane[PLANE_TOP].b = mat._24 - mat._22;
	clip_plane[PLANE_TOP].c = mat._34 - mat._32;
	clip_plane[PLANE_TOP].d = mat._44 - mat._42;

	clip_plane[PLANE_BOTTOM].a = mat._14 + mat._12;
	clip_plane[PLANE_BOTTOM].b = mat._24 + mat._22;
	clip_plane[PLANE_BOTTOM].c = mat._34 + mat._32;
	clip_plane[PLANE_BOTTOM].d = mat._44 + mat._42;

	clip_plane[PLANE_NEAR].a = mat._13;
	clip_plane[PLANE_NEAR].b = mat._23;
	clip_plane[PLANE_NEAR].c = mat._33;
	clip_plane[PLANE_NEAR].d = mat._43;

	clip_plane[PLANE_FAR].a = mat._14 - mat._13;
	clip_plane[PLANE_FAR].b = mat._24 - mat._23;
	clip_plane[PLANE_FAR].c = mat._34 - mat._33;
	clip_plane[PLANE_FAR].d = mat._44 - mat._43;

	for(int plane_index = 0; plane_index < 6; plane_index++)
		D3DXPlaneNormalize(&clip_plane[plane_index], &clip_plane[plane_index]);
}

inline_ bool render::box_in_frustrum(const D3DXVECTOR3 &mins, const D3DXVECTOR3 &maxs)
{
	for(int i = 0; i < 6; i++) {
		if(D3DXPlaneDotCoord(&clip_plane[i], &mins) > 0) continue;
		if(D3DXPlaneDotCoord(&clip_plane[i], &maxs) > 0) continue;
		if(D3DXPlaneDotCoord(&clip_plane[i], &D3DXVECTOR3(maxs.x, mins.y, mins.z)) > 0) continue;
		if(D3DXPlaneDotCoord(&clip_plane[i], &D3DXVECTOR3(maxs.x, maxs.y, mins.z)) > 0) continue;
		if(D3DXPlaneDotCoord(&clip_plane[i], &D3DXVECTOR3(maxs.x, mins.y, maxs.z)) > 0) continue;
		if(D3DXPlaneDotCoord(&clip_plane[i], &D3DXVECTOR3(mins.x, maxs.y, maxs.z)) > 0) continue;
		if(D3DXPlaneDotCoord(&clip_plane[i], &D3DXVECTOR3(mins.x, mins.y, maxs.z)) > 0) continue;
		if(D3DXPlaneDotCoord(&clip_plane[i], &D3DXVECTOR3(mins.x, maxs.y, mins.z)) > 0) continue;

		return false;
	}

	return true;
}

inline_ bool render::box_in_frustrum(const int min[], const int max[]) {
	return box_in_frustrum(D3DXVECTOR3((float)min[0], (float)min[1], (float)min[2]),
		D3DXVECTOR3((float)max[0], (float)max[1], (float)max[2]));
}
