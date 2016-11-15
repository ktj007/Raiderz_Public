#include "stdafx.h"
#include "AMath.h"

namespace crl {

bool AMath::IsEqual2DPos( const vec3& vTarPos, const vec3& vCurPos, float fTolerDistance )
{
	/// 위에 있는지 체크
	float fHeightDiff = abs(vTarPos.z - vCurPos.z);
	if (200 < fHeightDiff) return false;

	/// 2D로 위치 체크
	vec3 v2DTarPos = vTarPos;
	v2DTarPos.z = 0;

	vec3 v2DCurPos = vCurPos;
	v2DCurPos.z = 0;

	float fDiff = v2DTarPos.DistanceTo(v2DCurPos);
	if (fTolerDistance < fDiff) return false;

	return true;
}

bool AMath::IsEqual2DPosSQ( const vec3& vTarPos, const vec3& vCurPos, float fTolerDistanceSQ )
{
	/// 위에 있는지 체크
//	float fHeightDiff = abs(vTarPos.z - vCurPos.z);
//	if (200 < fHeightDiff) return false;

	/// 2D로 위치 체크
	vec3 v2DTarPos = vTarPos;
	v2DTarPos.z = 0;

	vec3 v2DCurPos = vCurPos;
	v2DCurPos.z = 0;

	float x1 = vTarPos.x - vCurPos.x;
	float y1 = vTarPos.y - vCurPos.y;

	float fDiffSq = (x1*x1+y1*y1);

	if (fTolerDistanceSQ < fDiffSq) return false;

	return true;

}

} // namespace crl