#ifndef _XMATH_H
#define _XMATH_H

#include "AMath.h"
#include "MVector3.h"
#include "MVector4.h"
#include "MMatrix.h"



// 클라이언트에서 사용하는 수학 관련 함수 모음
class XMath : public crl::AMath
{
public:
	/// 모델의 월드 매트릭스를 만들기 위한 함수
	static void MakeModelMatrix(mat4* pout, const vec3& pos, const vec3& dir, const vec3& up);
	static bool RandomBool();

	static vec3 MoveParabolaInTime(vec3 vStartPos, vec3 vTargetPos, vec3& vCalParabola, float fMaxHeight, float fTickTime, float& fRemainTime);
};












#endif // _XMATH_H