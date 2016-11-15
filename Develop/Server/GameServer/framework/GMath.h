#ifndef _GMATH_H_
#define _GMATH_H_

#include "AMath.h"
#include "GFrameworkLib.h"

/// 서버에서만 사용하는 수학 관련 함수 모음
class GFRAMEWORK_API GMath : public crl::AMath
{
public:
	static vec3 LocalPosToWorldPos(vec3 vDir, vec3 vCenterPos, vec3 vAdjPos);	
	static bool Angle(vec3 v1, vec3 v2);

	/// x를 소수점 nDecimalPlaces 자리수까지 반올림한다.
	static float Round4Combat(float x, int nDecimalPlaces);

	static float PercentToFactor(float fPercent);
	static void GetAnglePos(vec3 vCenter, vec3 vDir, float fDistance, int nAngleQty, vector<vec3>& vecoutPos);
	static float GetActorMinRadiusFromCapsule(MCapsule cap);

	template<typename T>
	static float NumberToFloatPercent(T number)
	{
		if (100 < number)
		{
			return 1.0f;
		}
		else if (0 > number)
		{
			return 0.0f;
		}
		else
		{
			return static_cast<float>(number) / 100;
		}
	}	
};


#endif // _GMATH_H_