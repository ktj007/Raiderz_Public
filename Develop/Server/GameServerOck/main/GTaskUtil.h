#pragma once

class GEntityNPC;

class GTaskUtil
{
public:
	static vec3 CalcDir(float fDelta, float fRotateRadianSpeed, vec3& vCurrDir, vec3& vTarDir, float& foutRemainedDelta);
	static vec3 CalcPos(float fDelta, float fMoveSpeed, vec3& vCurPos, vec3& vTarPos, vec3& vDir);
};