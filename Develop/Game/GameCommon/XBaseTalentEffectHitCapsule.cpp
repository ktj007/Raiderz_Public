#include "stdafx.h"
#include "XBaseTalentEffectHitCapsule.h"


XBaseTalentEffectHitCapsule::XBaseTalentEffectHitCapsule()
{
	ClearAllTalentHitCapsuleImpact();
}

XBaseTalentEffectHitCapsule::~XBaseTalentEffectHitCapsule()
{
	ClearAllTalentHitCapsuleImpact();
}

void XBaseTalentEffectHitCapsule::Update( float fDelta )
{
	UpdateTalentHitCapsuleImpact(fDelta);
}

void XBaseTalentEffectHitCapsule::UpdateTalentHitCapsuleImpact( float fDelta )
{
	vector<TALENT_HIT_CAPSULE_IMPACT>::iterator it = m_vecTalentHitCapsuleImpact.begin();
	while(it != m_vecTalentHitCapsuleImpact.end())
	{
		// 사용자가 유효한지... 탤런트가 유효한지 확인한다.
		if(CheckExistActor(it->uidAttacker) == false || it->pTalentInfo == NULL)
		{
			// 없다. 삭제
			it = m_vecTalentHitCapsuleImpact.erase(it);
			continue;
		}

		float fPreTime = it->fElapsedTime;
		it->fElapsedTime += fDelta;

		CheckTalentHitCapsuleEffect(it->pTalentInfo, it->pTalentEffectInfo, it->nType, fPreTime, it->fElapsedTime, it->matAttackerWorld, it->vDirAttacker);

		if(it->fMaxTime <= it->fElapsedTime)
		{
			// 삭제
			it = m_vecTalentHitCapsuleImpact.erase(it);
			continue;
		}

		it++;
	}
}

void XBaseTalentEffectHitCapsule::CheckTalentHitCapsuleEffect( XTalentInfo* pTalentInfo, XTalentEffectInfo* pTalentEffectInfo, TALENT_EFFECT_TYPE eType, float fPreTime, float fCurTime, MMatrix& matWorld, vec3& vDir )
{
	for(vector<CSHitSegment>::iterator it = pTalentInfo->m_HitInfo.m_vSegments.begin() ; it != pTalentInfo->m_HitInfo.m_vSegments.end(); it++)
	{
		if(fPreTime < it->m_fCheckTime &&
			fCurTime >= it->m_fCheckTime)
		{
			CreateTalentHitCapsuleEffect(pTalentInfo, pTalentEffectInfo, eType, it->m_vCapsules, matWorld, vDir);
		}
	}
}

void XBaseTalentEffectHitCapsule::CreateTalentHitCapsuleEffect( XTalentInfo* pTalentInfo, XTalentEffectInfo* pTalentEffectInfo, TALENT_EFFECT_TYPE eType, vector<MCapsule>& vecTalentHitCapsule, MMatrix& matWorld, vec3& vDir )
{
	for(vector<MCapsule>::iterator it = vecTalentHitCapsule.begin(); it != vecTalentHitCapsule.end(); it++)
	{
		vec3 vEffectPos;
		MMatrix mat = matWorld;
		mat.TransformVect(it->GetCenter(), vEffectPos);

		CreateEffect(pTalentEffectInfo->m_strEffectName, vEffectPos, vDir);
	}
}

void XBaseTalentEffectHitCapsule::AddTalentHitCapsuleImpact( RActor* pActor, XTalentInfo* pTalentInfo, XTalentEffectInfo* pTalentEffectInfo, TALENT_EFFECT_TYPE eType, vec3 vTalentDir, MUID uid /*= MUID::ZERO*/ )
{
	if(pActor == NULL)
		return;

	float fMaxTime = GetMaxCheckTime(pTalentInfo);

	MMatrix matWorld;
	matWorld.SetLocalMatrix(pActor->GetWorldPosition(), vTalentDir, vec3::AXISZ);
	TALENT_HIT_CAPSULE_IMPACT hitCapsuleType;
	hitCapsuleType.pTalentInfo			= pTalentInfo;
	hitCapsuleType.pTalentEffectInfo	= pTalentEffectInfo;
	hitCapsuleType.nType				= eType;
	hitCapsuleType.uidAttacker			= uid;
	hitCapsuleType.fMaxTime				= fMaxTime;
	hitCapsuleType.matAttackerWorld		= matWorld;
	hitCapsuleType.vDirAttacker			= vTalentDir;
	m_vecTalentHitCapsuleImpact.push_back(hitCapsuleType);
}

float XBaseTalentEffectHitCapsule::GetMaxCheckTime( XTalentInfo* pTalentInfo )
{
	float fMaxTime = 0.0f;
	for(vector<CSHitSegment>::iterator it = pTalentInfo->m_HitInfo.m_vSegments.begin() ; it != pTalentInfo->m_HitInfo.m_vSegments.end(); it++)
	{
		if(fMaxTime <= it->m_fCheckTime)
			fMaxTime = it->m_fCheckTime;
	}	
	
	return fMaxTime;
}

void XBaseTalentEffectHitCapsule::ClearAllTalentHitCapsuleImpact()
{
	m_vecTalentHitCapsuleImpact.clear();
}