#include "stdafx.h"
#include "CSMotionFactorInfo.h"

MOTION_FACTOR_VAR& MOTION_FACTOR_VAR::operator+=( const MOTION_FACTOR_VAR& rhs )
{
	nValue += rhs.nValue;
	nWeight += rhs.nWeight;

	return *this;
}

MOTION_FACTOR_GROUP& MOTION_FACTOR_GROUP::operator+=( const MOTION_FACTOR_GROUP& rhs )
{
	for (int i=0; i<MF_PRIORITY_SIZE; i++)
	{
		var[i] += rhs.var[i];
	}

	return *this;
}


