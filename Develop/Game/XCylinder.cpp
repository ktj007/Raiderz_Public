#include "StdAfx.h"
#include "XCylinder.h"

RBoundingBox XCylinder::GetAABB()
{
	m_aabb.vmin = vec3(bottom.x - radius, bottom.y - radius, bottom.z);
	m_aabb.vmax = vec3(bottom.x + radius, bottom.y + radius, bottom.z+height);

	return m_aabb;
}