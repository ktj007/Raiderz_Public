#pragma once
#include "MCylinder.h"

class XCylinder : public MCylinder
{
	RBoundingBox m_aabb;

public:
	XCylinder(void) : MCylinder() { m_aabb.Initialize(); }
	XCylinder(const MVector3& _bottom, float _height, float _radius)
		: MCylinder(_bottom, _height, _radius) { m_aabb.Initialize(); }

	RBoundingBox GetAABB();
};
