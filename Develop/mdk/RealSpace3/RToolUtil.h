#pragma once

#include "RTypes.h"

namespace rs3 {
	/**
	 TOOL에서 마우스로 드래그 할 때, 오브젝트의 움직임을 상태에 따라 제한되게 만들어 주는 것 .
	 baseMat를 기준으로 사용하며 드래그의 움직임은 가능한 Max에 맞췄다.
	*/
	const UINT MOVEAXIS_X = 1;
	const UINT MOVEAXIS_Y = 2;
	const UINT MOVEAXIS_Z = 3;
	const UINT MOVEAXIS_XY = 4;
	const UINT MOVEAXIS_YZ = 5;
	const UINT MOVEAXIS_XZ = 6;

	RVector GetMoveVector(	UINT nAxisType, const RMatrix &baseMat, const RVector &vPosition,
							const RVector &p1, const RVector &q1, const RVector &q2);

	RS_API RVector GetMoveVector(	UINT nAxisType,const RVector &q1, const RVector &q2);
}
