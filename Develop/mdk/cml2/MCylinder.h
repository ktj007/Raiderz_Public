#ifndef _MCYLINDER_H
#define _MCYLINDER_H

#include "MVector3.h"

class CML2_API MCylinder
{
public:
	MVector3	bottom;
	float		height;
	float		radius;

	MCylinder()
		: bottom(MVector3::ZERO), height(0), radius(0) {}
	MCylinder(const MVector3& _bottom, float _height, float _radius)
		: bottom(_bottom), height(_height), radius(_radius) {}

	bool		CollisionPoint(const MVector3& vPos) const;

};


#endif	// _MCYLINDER_H