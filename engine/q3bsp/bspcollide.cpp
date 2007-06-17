#include "precompiled.h"
#include "q3bsp/bspcollide.h"
#include "q3bsp/bleh.h"

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


	const BSPNode& node = nodes[node_index];
	const BSPPlane& plane = planes[node.plane];

	float start_dist = plane.nrm.x * collider.start.x +
		plane.nrm.y * collider.start.y +
		plane.nrm.z * collider.start.z - plane.dst;

	float end_dist = plane.nrm.x * collider.end.x +
		plane.nrm.y * collider.end.y +
		plane.nrm.z * collider.end.z - plane.dst;

	float offset = fabs(plane.nrm.x * collider.size.x) +
		fabs(plane.nrm.y * collider.size.y) +
		fabs(plane.nrm.z * collider.size.z);

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
	BSPLeaf &leaf = leafs[leaf_index];
	int brush_start = leaf.leafbrush;
	int brush_count = leaf.numleafbrushes;

	while(brush_count--) { // loop through brushes in the leaf
		int brush_index = leafbrushes[brush_start + brush_count];
		BSPBrush& brush = brushes[brush_index];
		
		// this needs to be figured out
		/*if(bsptextures[brush.texture].flags == 16528) // ignore the 'hint' texture
			continue;

		if(bsptextures[brush.texture].flags == 16544) // ignore the 'cluster portal' texture
			continue;

		if(bsptextures[brush.texture].flags == 16512) // ignore the 'nodrawsolid' texture
			continue; */

		if(bsptextures[brush.texture].flags & 0x4000) // ignore the 'hint' texture
			continue;

		int side_start = brush.brushside;
		int side_index = brush.numbrushsides;

		float enter_frac = -1.0f;
		float exit_frac = 1.0f;
		bool start_out = false;
		bool end_out = false;
		bool full_out = false;

		D3DXVECTOR3 hit_normal(0, 0, 0);
		//LOG("[BSP::collideLeaf] checking brush %i", brush_index);

		while(side_index--) { // loop through sides of the brush
			BSPPlane& plane = planes[brushsides[side_start + side_index].plane];
			
			D3DXVECTOR3 size = -collider.size;
			if(plane.nrm.x < 0) size.x = -size.x;
			if(plane.nrm.y < 0) size.y = -size.y;
			if(plane.nrm.z < 0) size.z = -size.z;

			float dist = plane.dst - D3DXVec3Dot(&size, &plane.nrm);
			float d1 = D3DXVec3Dot(&collider.start, &plane.nrm) - dist;
			float d2 = D3DXVec3Dot(&collider.end, &plane.nrm) - dist;
			float f;

			//if((d1 >= -SMALL_FLOAT) && (d1 < 0))
			//	d1 += SMALL_FLOAT;

			//if((d2 < SMALL_FLOAT) && (d2 > 0))
			//	d2 -= SMALL_FLOAT;

			if(d1 >= -SMALL_FLOAT) start_out = true;
			if(d2 >= SMALL_FLOAT) end_out = true;

			//if(1) console::log(console::FLAG_DEBUG, "[bspcollide] d1,d2 = (%f, %f)", d1, d2);

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
					hit_normal = plane.nrm;
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
				collider.brush_contents = brush.texture;
				collider.brush_id = brush_index;
				collider.in_solid = true;
// 				FRAMEDO(LOG("[q3bsp::collideLeaf] ERROR: totally in \"%s\" (%i)", 
// 					bsptextures[brush.texture].name, 
// 					bsptextures[brush.texture].flags));
				return;
			}

			if(enter_frac < exit_frac) { // moved into a brush
				if((enter_frac > -1.0f) && (enter_frac < collider.fraction)) { // haven't already hit a brush, update collision
						if(enter_frac < 0.0f)
							enter_frac = 0.0f;
						collider.fraction = enter_frac;
						collider.normal = hit_normal;
						collider.brush_contents = brush.texture;
						collider.brush_id = brush_index;
						//LOG("[bspcollide] collided with \"%s\" (%i)", bsp_textures[brush.texture].strName, bsp_textures[brush.texture].flags);
				}
			}
		}

		if(collider.fraction == 0.0f) // can't move any more, return
			return;
	}
}