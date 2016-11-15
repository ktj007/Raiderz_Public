#include "stdafx.h"
#include "GMath.h"

vec3 GMath::LocalPosToWorldPos(vec3 vDir, vec3 vCenterPos, vec3 vAdjPos)
{
	GMath::NormalizeZ0(vDir);

	mat4 matTM;
	matTM.SetLocalMatrix(vec3::ZERO, vDir, vec3::AXISZ);

	return (vCenterPos + vAdjPos * matTM);
}

bool GMath::Angle( vec3 v1, vec3 v2 )
{
	if (v1.DotProduct(v2) < 0.0f) return true;
	return false;
}

float GMath::Round4Combat( float x, int nDecimalPlaces )
{
	const float fD = Pow(10.0f, nDecimalPlaces);
	float ret = AMath::Roundf(x * fD);
	return (ret / fD);
}

float GMath::PercentToFactor( float fPercent )
{
	return (fPercent / 100.0f);
}

void GMath::GetAnglePos(vec3 vCenter, vec3 vDir, float fDistance, int nAngleQty, vector<vec3>& vecoutPos)
{
	vecoutPos.clear();
	if (nAngleQty <= 0)		return;

	vDir.z = 0.0f;
	vDir.Normalize();

	vec3 vLocalPoint = (-vDir) * fDistance;
	mat4 matRot(mat4::IDENTITY);

	vec3 vRotatedPoint;
	float fEachDegree = 360.0f/nAngleQty;
	for (int i=0; i<nAngleQty; i++)
	{
		matRot.SetRotationDegrees(vec3(0.0f, 0.0f, fEachDegree*(i+1)));	// 왼쪽으로 회전
		matRot.TransformVect(vLocalPoint, vRotatedPoint);

		vecoutPos.push_back(vRotatedPoint + vCenter);
	}
}

float GMath::GetActorMinRadiusFromCapsule( MCapsule cap )
{
	const float capRadius = cap.radius;

	float fMinX = 0.0f;
	float fMaxX = 0.0f;
	float fMinY = 0.0f;
	float fMaxY = 0.0f;

	fMinX = min(fMinX, cap.bottom.x + capRadius);
	fMinX = min(fMinX, cap.bottom.x - capRadius);
	fMinX = min(fMinX, cap.top.x + capRadius);
	fMinX = min(fMinX, cap.top.x - capRadius);

	fMinY = min(fMinY, cap.bottom.y + capRadius);
	fMinY = min(fMinY, cap.bottom.y - capRadius);
	fMinY = min(fMinY, cap.top.y + capRadius);
	fMinY = min(fMinY, cap.top.y - capRadius);

	fMaxX = max(fMaxX, cap.bottom.x + capRadius);
	fMaxX = max(fMaxX, cap.bottom.x - capRadius);
	fMaxX = max(fMaxX, cap.top.x + capRadius);
	fMaxX = max(fMaxX, cap.top.x - capRadius);

	fMaxY = max(fMaxY, cap.bottom.y + capRadius);
	fMaxY = max(fMaxY, cap.bottom.y - capRadius);
	fMaxY = max(fMaxY, cap.top.y + capRadius);
	fMaxY = max(fMaxY, cap.top.y - capRadius);

	fMinX = abs(fMinX);
	fMinY = abs(fMinY);
	fMaxX = abs(fMaxX);
	fMaxY = abs(fMaxY);


	float fMinRadius = min(fMinX, fMinY);
	fMinRadius = min(fMinRadius, fMaxX);
	fMinRadius = min(fMinRadius, fMaxY);

	fMinRadius = max(0.0f, fMinRadius);
	return fMinRadius;
}
