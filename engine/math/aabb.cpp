/////////////////////////////////////////////////////////////////////////////
// render.cpp
// rendering system implementation
// $Id$
//

#include "precompiled.h"
#include "math/aabb.h"
#include "console/console.h"

	
AABB::AABB()
{
	reset();
}

AABB::AABB(D3DXVECTOR3* min, D3DXVECTOR3* max)
{
	reset();
	extend(min, max);
}

AABB::~AABB()
{
}

void AABB::reset() {
	ZeroMemory(this, sizeof(AABB));
	min = D3DXVECTOR3(BIGFLOAT, BIGFLOAT, BIGFLOAT);
	max = D3DXVECTOR3(-BIGFLOAT, -BIGFLOAT, -BIGFLOAT);
}

void AABB::extend(const D3DXVECTOR3* min, const D3DXVECTOR3* max)
{
	if(min->x < this->min.x) this->min.x = min->x;
	if(min->y < this->min.y) this->min.y = min->y;
	if(min->z < this->min.z) this->min.z = min->z;

	if(max->x > this->max.x) this->max.x = max->x;
	if(max->y > this->max.y) this->max.y = max->y;
	if(max->z > this->max.z) this->max.z = max->z;

	recalcPos();
}

void AABB::extend(const D3DXVECTOR3* pt)
{
	extend(pt, pt);		
}

void AABB::extend(const AABB* bbox)
{
	extend(&(bbox->min), &(bbox->max));
}

void AABB::recalcPos()
{
	pos = (min + max) / 2.0f;
	extents.x = fabs(max.x - min.x) / 2.0f;
	extents.y = fabs(max.y - min.y) / 2.0f;
	extents.z = fabs(max.z - min.z) / 2.0f;

	recalc();
}

void AABB::recalcMinMax()
{
	min = pos - extents;
	max = pos + extents;

	recalc();
}

void AABB::recalc()
{
	cdist = D3DXVec3Length(&D3DXVECTOR3(pos - min));
}

void AABB::debugPrint()
{
	LOG7("[AABB] min: {%.2f, %.2f, %.2f}  max: {%.2f, %.2f, %.2f}",
		min.x, min.y, min.z,
		max.x, max.y, max.z);
	LOG7("[AABB] pos: {%.2f, %.2f, %.2f}  extents: {%.2f, %.2f, %.2f}",
		pos.x, pos.y, pos.z,
		extents.x, extents.y, extents.z);
	LOG2("[AABB] cdist: %.2f", cdist);
	LOG7("[AABB] pdist: {%.2f, %.2f, %.2f, %.2f, %.2f, %.2f}",
		pdist[0], pdist[1], pdist[2], pdist[3], pdist[4], pdist[5]);
}

bool AABB::intersects(const AABB* test)
{
	if(D3DXVec3Length(&D3DXVECTOR3(pos - test->pos)) > (cdist + test->cdist))
		return false;

	if((pos.x + extents.x) < (test->pos.x - test->extents.x)) return false;
	if((pos.x - extents.x) > (test->pos.x + test->extents.x)) return false;

	if((pos.y + extents.y) < (test->pos.y - test->extents.y)) return false;
	if((pos.y - extents.y) > (test->pos.y + test->extents.y)) return false;

	if((pos.z + extents.z) < (test->pos.z - test->extents.z)) return false;
	if((pos.z - extents.z) > (test->pos.z + test->extents.z)) return false;

	return true;
}

bool AABB::isContained(AABB* test)
{
	if((min.x < test->min.x) || (min.x > test->max.x)) return false;
	if((max.x < test->min.x) || (max.x > test->max.x)) return false;

	if((min.y < test->min.y) || (min.y > test->max.y)) return false;
	if((max.y < test->min.y) || (max.y > test->max.y)) return false;

	if((min.z < test->min.z) || (min.z > test->max.z)) return false;
	if((max.z < test->min.z) || (max.z > test->max.z)) return false;

	return true;
}

bool AABB::contains(AABB* test)
{
	return test->isContained(this);
}

bool AABB::contains(const D3DXVECTOR3* pt)
{
	if((pt->x < min.x) || (pt->x > max.x)) return false;
	if((pt->y < min.y) || (pt->y > max.y)) return false;
	if((pt->z < min.z) || (pt->z > max.z)) return false;
	return true;
}