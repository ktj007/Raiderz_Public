#include "stdafx.h"
#include "GTaskUtil.h"
#include "GDef.h"
#include "GEntityNPC.h"
#include "GMath.h"

vec3 GTaskUtil::CalcDir(float fDelta, float fRotateRadianSpeed, vec3& vCurrDir, vec3& vTarDir, float& foutRemainedDelta)
{
	foutRemainedDelta = 0.0f;

	vec3 vResultDir = vec3::ZERO;
	if (0.0f < fRotateRadianSpeed)
	{
		vec3 vCross = vCurrDir.CrossProduct(vec3::AXISZ);
		float fDot = vCross.DotProduct(vTarDir);

		if( fDot > 0)
		{
			fRotateRadianSpeed *= -1;
		}

		float fRotateRadian = fRotateRadianSpeed * fDelta;
		float fCurToTarRadian = vCurrDir.AngleToXY(vTarDir);

		if (abs(fRotateRadian) > abs(fCurToTarRadian))
		{
			float fOverAngleRadian = fRotateRadian - fCurToTarRadian;
			foutRemainedDelta = abs(fOverAngleRadian / fRotateRadianSpeed);

			fRotateRadian = fCurToTarRadian;
		}

		vResultDir.x = vCurrDir.x * cos(fRotateRadian) - vCurrDir.y * sin(fRotateRadian);
		vResultDir.y = vCurrDir.y * cos(fRotateRadian) + vCurrDir.x * sin(fRotateRadian);
	}
	else
	{
		vResultDir = vTarDir;
		foutRemainedDelta += fDelta;
	}

	return vResultDir;
}

vec3 GTaskUtil::CalcPos(float fDelta, float fMoveSpeed, vec3& vCurPos, vec3& vTarPos, vec3& vDir)
{	
	vec3 vNextPos = vCurPos + (fMoveSpeed * fDelta * vDir);

	vec3 tar_pos, next_pos;

	float fPosZ = max(vCurPos.z, vTarPos.z);
	
	tar_pos.x = vTarPos.x;
	tar_pos.y = vTarPos.y;
	tar_pos.z = vCurPos.z;

	next_pos.x = vNextPos.x;
	next_pos.y = vNextPos.y;
	next_pos.z = vCurPos.z;

	float fCurToTarDistanceSq = vCurPos.DistanceToSq(tar_pos);
	float fCurToNextDistanceSq = vCurPos.DistanceToSq(next_pos);

	tar_pos.z = fPosZ;
	next_pos.z = fPosZ;

	if (fCurToTarDistanceSq <= fCurToNextDistanceSq)
	{
		return tar_pos;
	}

	return next_pos;
}
