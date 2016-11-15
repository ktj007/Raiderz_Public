#include "stdafx.h"
#include "XTalentEffectInfo.h"
#include "XTalentInfoDef.h"
#include "XTalentInfoMgr.h"
#include "XTalentInfo.h"
#include "CSTalentInfoParser.h"


//////////////////////////////////////////////////////////////////////////
// 전역 함수

vec3 MoveParabola( vec3 vStartPos, vec3 vTargetPos, vec3 vCurPos, vec3 vVelocity, float fTickTime, float fMaxHeight )
{
	// 방향 회전
	vec3 vDir			= (vTargetPos - vCurPos).Normalize();

	// 위치
	float fSpeed		= vVelocity.Length();
	vec3 vCalPos		= vCurPos + (vDir * fSpeed * fTickTime);

	// 포물선 계산
	float fFullLength	= (vTargetPos - vStartPos).Length();
	float fCulLength	= (vCalPos - vStartPos).Length();

	float fHeight		= CalParabola(fFullLength, fCulLength, fMaxHeight);

	vCalPos.z			+= fHeight;

	return vCalPos;
}

float CalParabola( float fFullLength, float fCurrLength, float fMaxHeight )
{
	float fAngle			= (180.0f * fCurrLength) / fFullLength;
	float fPI				= 3.1415926535f;
	float fDegreeToRadian	= (fPI * fAngle / 180.0f);

	return (sin(fDegreeToRadian) * fMaxHeight);
}

XTalentEffectInfo* XTalentEffectDataMgr::Get( tstring& strEffectInfoName )
{
	vector<XTalentEffectInfo *>::iterator itInfo = begin();
	while(itInfo != end())
	{
		if((*itInfo)->m_strTalentEffectInfoName == strEffectInfoName)
		{
			return (*itInfo);
		}

		itInfo++;
	}

	return NULL;
}