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


TEST(TestTaskUtilCalcDir)
{
	vec3 vMyPos = vec3(0,0,0);
	vec3 vMyDir = vec3(1,0,0);

	vec3 vTarPos = vec3(0,100,0);
	vec3 vTarDir = vTarPos - vMyPos;
	GMath::NormalizeZ0(vTarDir);

	// 1초에 한바퀴돌고 90도 회전했을 때의 테스트
	float fRotateRadianSpeed = 2 * GMath::PI;	// 1초에 360도 회전
	float fRemainedDelta = 0.0f;
	vec3 vNextDir = GTaskUtil::CalcDir(0.25f, fRotateRadianSpeed, vMyDir, vTarDir, fRemainedDelta);
	CHECK_CLOSE(vNextDir.x, 0.0f, 0.01f);
	CHECK_CLOSE(vNextDir.y, 1.0f, 0.01f);
	CHECK_CLOSE(vNextDir.z, 0.0f, 0.01f);
	CHECK_CLOSE(fRemainedDelta, 0.0f, 0.01f);

	// delta가 클때도 목표방향을 벗어나서까지 회전하는지 테스트
	fRotateRadianSpeed = 2 * GMath::PI;	// 1초에 360도 회전
	vNextDir = GTaskUtil::CalcDir(1.f, fRotateRadianSpeed, vMyDir, vTarDir, fRemainedDelta);
	CHECK_CLOSE(vNextDir.x, vTarDir.x, 0.01f);
	CHECK_CLOSE(vNextDir.y, vTarDir.y, 0.01f);
	CHECK_CLOSE(vNextDir.z, vTarDir.z, 0.01f);
	CHECK_CLOSE(fRemainedDelta, 0.75f, 0.01f);

	// 회전을 안하면 바로 vTarDir로 세팅되는지 테스트
	fRotateRadianSpeed = 0.0f;
	vNextDir = GTaskUtil::CalcDir(0.25f, fRotateRadianSpeed, vMyDir, vTarDir, fRemainedDelta);
	CHECK_CLOSE(vNextDir.x, vTarDir.x, 0.01f);
	CHECK_CLOSE(vNextDir.y, vTarDir.y, 0.01f);
	CHECK_CLOSE(vNextDir.z, vTarDir.z, 0.01f);
}


