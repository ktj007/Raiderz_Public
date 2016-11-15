#include "stdafx.h"
#include "MCylinder.h"

bool MCylinder::CollisionPoint( const MVector3& vPos ) const
{
	if ( vPos.z < bottom.z || vPos.z > (bottom.z + height) )
		return false;

	float dist = bottom.XYDistanceTo(vPos);
	if (dist < radius)
		return true;

	return false;
}