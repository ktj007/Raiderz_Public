#include "stdafx.h"
#include "XMath.h"

void XMath::MakeModelMatrix(mat4* pout, const vec3& pos, const vec3& dir, const vec3& up)
{
	vec3 x, y, z;

	y = -dir;
	y.Normalize();

	x = CrossProduct(y, up);
	x.Normalize();

	z = CrossProduct(x, y);
	z.Normalize();

	pout->_11 = x.x;
	pout->_12 = x.y;
	pout->_13 = x.z;
	pout->_14 = 0.0f;

	pout->_21 = y.x;
	pout->_22 = y.y;
	pout->_23 = y.z;
	pout->_24 = 0.0f;

	pout->_31 = z.x;
	pout->_32 = z.y;
	pout->_33 = z.z;
	pout->_34 = 0.0f;

	pout->_41 = pos.x;
	pout->_42 = pos.y;
	pout->_43 = pos.z;
	pout->_44 = 1.0f;
}

bool XMath::RandomBool()
{
	if (0 == XMath::RandomNumber(0, 1))
	{
		return false;
	}

	return true;
}

vec3 XMath::MoveParabolaInTime( vec3 vStartPos, vec3 vTargetPos, vec3& vCalParabola, float fMaxHeight, float fTickTime, float& fRemainTime )
{
	// RemainTime에서 TickTime 빼기는 이 함수 이후에 해주세요.

	// 직선 위치 구하기
	vec3 vDir = vTargetPos - vCalParabola;
	float dist = vDir.Length();
	vDir.Normalize();

	if (fRemainTime <= 0.0f || (fRemainTime - fTickTime) <= 0.0f ) 
	{
		return vTargetPos;
	}

	float fSpeed = dist * fTickTime / fRemainTime;
	vec3 vPos = vCalParabola + (vDir * fSpeed);
	vCalParabola = vPos;

	// 포물선 위치 구하기
	float fFullLen = (vTargetPos - vStartPos).Length();
	float fCurLen = (vCalParabola - vStartPos).Length();
	float fAngle = (180.0f * fCurLen) / fFullLen;

	float fDegree = DegreeToRadian(fAngle);
	float fTestSin = sin(DegreeToRadian(fAngle));
	float fTestZ = vPos.z;
	vPos.z += sin(DegreeToRadian(fAngle)) * fMaxHeight;
	//mlog("테스트 %f - %f(%f) --> %f - %f 각(%f)\n", fDegree, fTestSin, fTestSin * fMaxHeight, fTestZ, vPos.z, fAngle);

	if(vPos.z < 0.0f)
	{
		int a = 0;
	}

	return vPos;
}


