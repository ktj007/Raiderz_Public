#include "stdafx.h"
#include "RToolUtil.h"

// 뭔지 모르지만 옮겨가고 싶다
namespace rs3 {

RVector GetMoveVector(UINT nAxisType, const RMatrix &baseMat, const RVector &vPosition
					  , const RVector &p1, const RVector &q1, const RVector &q2)
{
	RVector vBaseXAxis( baseMat._11, baseMat._21, baseMat._31);
	RVector vBaseYAxis( baseMat._12, baseMat._22, baseMat._32);
	RVector vBaseZAxis( baseMat._13, baseMat._23, baseMat._33);

	RPlane XYPlane, YZPlane, XZPlane;
	XYPlane.SetPlane(vBaseZAxis, vPosition);
	YZPlane.SetPlane(vBaseXAxis, vPosition);
	XZPlane.SetPlane(vBaseYAxis, vPosition);

	RPlane *pPlane = NULL;
	switch ( nAxisType )
	{
	case MOVEAXIS_X :
		pPlane = &XZPlane;
		break;
	case MOVEAXIS_Y :
		pPlane = &XYPlane;
		break;
	case MOVEAXIS_Z :
		pPlane = &XZPlane;
		break;
	case MOVEAXIS_XY:
		pPlane = &XYPlane;
		break;
	case MOVEAXIS_YZ:
		pPlane = &YZPlane;
		break;
	case MOVEAXIS_XZ:
		pPlane = &XZPlane;
		break;
	}

	RVector v1, v2;
	if ( !pPlane->Intersect(v1, p1, q1))
		return RVector(0, 0, 0);
	if ( !pPlane->Intersect(v2, p1, q2))
		return RVector(0, 0, 0);

	RVector vDirection = ( v2 - v1 );
	float t;

	switch ( nAxisType )
	{
	case MOVEAXIS_X :
		t = vDirection.DotProduct(vBaseXAxis);
		vDirection = vBaseXAxis*t;
		break;
	case MOVEAXIS_Y :
		t = vDirection.DotProduct(vBaseYAxis);
		vDirection = vBaseYAxis*t;
		break;
	case MOVEAXIS_Z :
		t = vDirection.DotProduct(vBaseZAxis);
		vDirection = vBaseZAxis*t;
		break;
	case MOVEAXIS_XY:
		vDirection.z = 0.0f;
		break;
	case MOVEAXIS_YZ:
		vDirection.x = 0.0f;
		break;
	case MOVEAXIS_XZ:
		vDirection.y = 0.0f;
		break;
	}

	return vDirection;
}

/**
	param
	q2 : 현재 마우스의 3d위치
	q1 : 전   마우스의 3d위치
	
*/
rs3::RVector GetMoveVector( UINT nAxisType,const RVector &q1, const RVector &q2 )
{
	RVector v( q2 - q1 );	// 마우스 움직임 in 3D

	//마우스움직임중 해당 좌표의 원소만 얻어냄
	switch ( nAxisType )
	{
	case MOVEAXIS_X :
		v.y = v.z = 0;
		break;
	case MOVEAXIS_Y :
		v.x = v.z = 0;
		break;
	case MOVEAXIS_Z :
		v.x = v.y = 0;
		break;
	case MOVEAXIS_XY:
		v.z = 0;
		break;
	case MOVEAXIS_YZ:
		v.x = 0;
		break;
	case MOVEAXIS_XZ:
		v.y = 0;
		break;
	}

	return v;

}

}
