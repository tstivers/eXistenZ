/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: bspcollide.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#include "precompiled.h"
#include "q3bsp/bspcollide.h"
#include "q3bsp/bsp.h"
#include "console/console.h"

//#define SMALL_FLOAT (0.001f)
#define SMALL_FLOAT (0.01f)

using namespace q3bsp;

void BSP::collide(D3DXVECTOR3 start, D3DXVECTOR3 end, D3DXVECTOR3 size, collider_t& collider, float fraction)
{
	ZeroMemory(&collider, sizeof(collider_t));
	collider.start = start;
	collider.end = end;
	collider.size = size;
	collider.fraction = fraction;
	collider.in_solid = false;
	collider.brush_id = 0;

	collideNode(collider, 0.0f, 1.0f, 0);
	
	collider.collision = collider.start + collider.fraction * (collider.end - collider.start);

	if(collider.fraction < SMALL_FLOAT)
		collider.fraction = 0;
}

void BSP::collideNode(collider_t& collider, float start_fraction, float end_fraction, int node_index)
{
	// check if we've already collided with something closer
	if(collider.fraction < start_fraction)
		return;

	// if this is a leaf check against the leaf brush
	if(node_index < 0) {
		collideLeaf(collider, ~node_index);
		return;
	}


	const tBSPNode& node = nodes[node_index];
	const tBSPPlane& plane = planes[node.plane];

	float start_dist = plane.vNormal.x * collider.start.x +
		plane.vNormal.y * collider.start.y +
		plane.vNormal.z * collider.start.z - plane.d;

	float end_dist = plane.vNormal.x * collider.end.x +
		plane.vNormal.y * collider.end.y +
		plane.vNormal.z * collider.end.z - plane.d;

	float offset = fabs(plane.vNormal.x * collider.size.x) +
		fabs(plane.vNormal.y * collider.size.y) +
		fabs(plane.vNormal.z * collider.size.z);

	if((start_dist >= offset) && (end_dist >= offset)) {
		// box was in front of plane, check front node
		collideNode(collider, start_fraction, end_fraction, node.front);
		return;
	}

	if((start_dist < -offset) && (end_dist < -offset)) {
		// box was behind plane, check back node
		collideNode(collider, start_fraction, end_fraction, node.back);
		return;
	}

	// plane intersected box, check both
	collideNode(collider, start_fraction, end_fraction, node.front);
	collideNode(collider, start_fraction, end_fraction, node.back);
	return;

	/* todo: fix this mess
	bool front_to_back = true;
	float inverse_distance;
	float fraction1, fraction2, mid_fraction;
	D3DXVECTOR3 mid_point, tmp_point;

	// box was split, check front and back
	if(start_dist < end_dist) {
		// moving from back to front
		front_to_back = false;
		inverse_distance = 1.0f / (start_dist - end_dist);
		fraction1 = (start_dist - offset - SMALL_FLOAT) * inverse_distance;
		fraction2 = (start_dist + offset + SMALL_FLOAT) * inverse_distance;
	} else if(start_dist > end_dist) {
		// moving from front to back
		front_to_back = true;
		inverse_distance = 1.0f / (start_dist - end_dist);
		fraction1 = (start_dist + offset + SMALL_FLOAT) * inverse_distance;
		fraction2 = (start_dist - offset - SMALL_FLOAT) * inverse_distance;
	}
	else { // should never happen, corner case
		fraction1 = 1.0f;
		fraction2 = 0.0f;
	}

	// check from start point to plane collision
	if(fraction1 < 0.0f)
		fraction1 = 0.0f;
	if(fraction1 > 1.0f)
		fraction1 = 1.0f;

	mid_fraction = start_fraction + (end_fraction - start_fraction) * fraction1;
	mid_point = collider.start + (fraction1 * (collider.end - collider.start));
	tmp_point = collider.end;
	collider.end = mid_point;
	collideNode(collider, start_fraction, mid_fraction, front_to_back ? node.front : node.back);
	collider.end = tmp_point;

	// check from plane collision to end point
	if(fraction2 < 0.0f)
		fraction2 = 0.0f;
	if(fraction2 > 1.0f)
		fraction2 = 1.0f;

	mid_fraction = start_fraction + (end_fraction - start_fraction) * fraction2;
	mid_point = collider.start + (fraction2 * (collider.end - collider.start));
	tmp_point = collider.start;
	collider.start = mid_point;
	collideNode(collider, mid_fraction, end_fraction, front_to_back ? node.back : node.front);
	collider.start = tmp_point; */
}

void BSP::collideLeaf(collider_t& collider, int leaf_index)
{
	tBSPLeaf &leaf = leafs[leaf_index];
	int brush_start = leaf.leafBrush;
	int brush_count = leaf.numOfLeafBrushes;

	while(brush_count--) { // loop through brushes in the leaf
		int brush_index = leafbrushes[brush_start + brush_count];
		tBSPBrush& brush = brushes[brush_index];
		
		// this needs to be figured out
		if(bsp_textures[brush.textureID].flags == 16528) // ignore the 'hint' texture
			continue;

		if(bsp_textures[brush.textureID].flags == 16544) // ignore the 'cluster portal' texture
			continue;

		if(bsp_textures[brush.textureID].flags == 16512) // ignore the 'nodrawsolid' texture
			continue;


		int side_start = brush.brushSide;
		int side_index = brush.numOfBrushSides;

		float enter_frac = -1.0f;
		float exit_frac = 1.0f;
		bool start_out = false;
		bool end_out = false;
		bool full_out = false;

		D3DXVECTOR3 hit_normal(0, 0, 0);
		//LOG2("[BSP::collideLeaf] checking brush %i", brush_index);

		while(side_index--) { // loop through sides of the brush
			tBSPPlane& plane = planes[brushsides[side_start + side_index].plane];
			
			D3DXVECTOR3 size = -collider.size;
			if(plane.vNormal.x < 0) size.x = -size.x;
			if(plane.vNormal.y < 0) size.y = -size.y;
			if(plane.vNormal.z < 0) size.z = -size.z;

			float dist = plane.d - D3DXVec3Dot(&size, &plane.vNormal);
			float d1 = D3DXVec3Dot(&collider.start, &plane.vNormal) - dist;
			float d2 = D3DXVec3Dot(&collider.end, &plane.vNormal) - dist;
			float f;

			//if((d1 >= -SMALL_FLOAT) && (d1 < 0))
			//	d1 += SMALL_FLOAT;

			//if((d2 < SMALL_FLOAT) && (d2 > 0))
			//	d2 -= SMALL_FLOAT;

			if(d1 >= -SMALL_FLOAT) start_out = true;
			if(d2 >= SMALL_FLOAT) end_out = true;

			//if(1) con::log(con::FLAG_DEBUG, "[bspcollide] d1,d2 = (%f, %f)", d1, d2);

			if((d1 >= -SMALL_FLOAT) && (d2 >= -SMALL_FLOAT)) { // both in front, not in this brush
				full_out = true;
				break;
			}

			if((d1 < -SMALL_FLOAT) && (d2 < -SMALL_FLOAT)) // both behind, check other brush sides
				continue;

			if(d1 >= d2) { // moving into the brush
				f = (d1) / (d1 - d2);
				if(f > enter_frac) { // collided
					enter_frac = f;
					hit_normal = plane.vNormal;
				}
			} else { // moving out of the brush
				f = (d1) / (d1 - d2);
				if(f < exit_frac)
					exit_frac = f;
			}
		}

		// done looping through this brush, check for collision and update collider
		if(!full_out) { // there was a collision
			if(!start_out && !end_out) { // totally in the brush
				collider.fraction = 0.0f;
				collider.normal = hit_normal;
				collider.brush_contents = brush.textureID;
				collider.brush_id = brush_index;
				collider.in_solid = true;
				FRAMEDO(LOG3("[q3bsp::collideLeaf] ERROR: totally in \"%s\" (%i)", 
					bsp_textures[brush.textureID].strName, 
					bsp_textures[brush.textureID].flags));
				return;
			}

			if(enter_frac < exit_frac) { // moved into a brush
				if((enter_frac > -1.0f) && (enter_frac < collider.fraction)) { // haven't already hit a brush, update collision
						if(enter_frac < 0.0f)
							enter_frac = 0.0f;
						collider.fraction = enter_frac;
						collider.normal = hit_normal;
						collider.brush_contents = brush.textureID;
						collider.brush_id = brush_index;
						//LOG3("[bspcollide] collided with \"%s\" (%i)", bsp_textures[brush.textureID].strName, bsp_textures[brush.textureID].flags);
				}
			}
		}

		if(collider.fraction == 0.0f) // can't move any more, return
			return;
	}
}