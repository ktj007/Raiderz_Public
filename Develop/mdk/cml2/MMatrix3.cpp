#include "stdafx.h"
#include "MMatrix3.h"

static float _matIdentity[] = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1
} ;

const MMatrix3 MMatrix3::IDENTITY(_matIdentity);

bool MMatrix3::GetInverse(MMatrix3 *pOut, float *fDet) const
{
	float det = 
		_11*(_33*_22-_32*_23) 
		- _21*(_33*_12-_32*_13) 
		+ _31*(_23*_12-_22*_13);

	if ( det == 0.0f )
		return false;

	if ( fDet )
		*fDet = det;

	if ( pOut )
	{
		MMatrix3 mat;
		mat._11 = (_33*_22 - _32*_23);
		mat._12 =-(_33*_12 - _32*_13);
		mat._13 = (_23*_12 - _22*_13);
		mat._21 =-(_33*_21 - _31*_23);
		mat._22 = (_33*_11 - _31*_13);
		mat._23 =-(_23*_11 - _21*_13);
		mat._31 = (_32*_21 - _31*_22);
		mat._32 =-(_32*_11 - _31*_12);
		mat._33 = (_22*_11 - _21*_12);

		float invDet = 1.0f / det;

		*pOut = mat * invDet;
	}

	return true;
}