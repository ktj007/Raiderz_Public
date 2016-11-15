#pragma once

#include "crlLib.h"
#include "MMath.h"

namespace crl {

class CRL_API AMath : public MMath
{
public:
	/// Z값을 0으로 만들고 정규화
	static void NormalizeZ0(vec3& v)
	{
		v.z = 0.0f;
		v.Normalize();
	}

	static bool IsEqual2DPos(const vec3& vTarPos, const vec3& vCurPos, float fTolerDistance);
	static bool IsEqual2DPosSQ(const vec3& vTarPos, const vec3& vCurPos, float fTolerDistanceSQ);

	template<typename T>
	static typename T RandomPickUp(vector<typename T>& vecNumber)
	{
		if (true == vecNumber.empty()) return 0;

		int nIndex = RandomNumber(0, vecNumber.size()-1);

		return vecNumber[nIndex];
	}
};


} // namespace crl