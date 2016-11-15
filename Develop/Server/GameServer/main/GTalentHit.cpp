#include "StdAfx.h"
#include "GTalentHit.h"
#include "GTalentInfo.h"
#include "GTalent.h"
#include "GEntityActor.h"
#include "GCollision.h"
#include "GField.h"
#include "GTalentInfoDef.h"
#include "GConst.h"

//////////////////////////////////////////////////////////////////////////
//
//	GTalentHit
//
//////////////////////////////////////////////////////////////////////////

GTalentHit::GTalentHit()
: m_nHitIndex(0)
{

}

void GTalentHit::UpdateHit(GTalent* pTalent, float fElapsedTime)
{
	/*PFC_THISFUNC;*/
	PFI_BLOCK_THISFUNC(710);

	VALID(pTalent);
	GTalentInfo* pTalentInfo = pTalent->GetInfo();
	VALID(pTalentInfo);

	int nTalentInfoHitCount = (int)pTalentInfo->m_HitInfo.m_vSegments.size();
	while (m_nHitIndex < nTalentInfoHitCount)
	{
		float fCheckTime = max( 0.0f, pTalentInfo->m_HitInfo.m_vSegments[m_nHitIndex].m_fCheckTime);

		// 실제 판정 시간보다 약간 먼저 검사한다
		if (fElapsedTime < fCheckTime - GConst::PRE_HIT_CHECK_TIME )
			break;

		int nAddtiveDamage = pTalentInfo->m_HitInfo.m_vSegments[m_nHitIndex].m_nAddtiveDamage;
		UpdateHitSeg(pTalent, m_nHitIndex, nAddtiveDamage, fCheckTime);
		m_nHitIndex++;
	}
}

void GTalentHit::UpdateHitSeg(GTalent* pTalent, int nHitColIndex, int nAddtiveDamage, float fCheckTime)
{
	/*PFC_THISFUNC;*/
	PFI_BLOCK_THISFUNC(704);

	VALID(pTalent);
	GEntityActor* pOwner = pTalent->GetOwner();
	VALID(pOwner);
	GTalentInfo* pTalentInfo = pTalent->GetInfo();
	VALID(pTalentInfo);
	GField* pField = pOwner->GetField();
	VALID(pField);

	vector<TALENT_TARGET_INFO> vecHittedTargets;
	set<MUID>* psetVictim = NULL;
	if (pTalentInfo->m_bSeparateHit == false)
	{
		psetVictim = &m_VictimUIDSet;
	}
	GCollision::HitTestTalent(pTalent, nHitColIndex, psetVictim, vecHittedTargets);

	for (size_t i = 0; i < vecHittedTargets.size(); i++)
	{
		GEntityActor* pVictim = pField->FindActor(vecHittedTargets[i].uidTarget);
		if (!pVictim)		continue;

		vecHittedTargets[i].nCheckTime = (int)(fCheckTime*1000.0f);
		pTalent->HitArea(pVictim, vecHittedTargets[i].nCheckTime, vecHittedTargets[i].nCapsuleGroupIndex, vecHittedTargets[i].nCapsuleIndex);

		m_VictimUIDSet.insert(pVictim->GetUID());

		// 히트 카운터 증가
		map<MUID, int>::iterator it = m_mapHitTable.find(pVictim->GetUID());
		if (it == m_mapHitTable.end())
		{
			m_mapHitTable.insert(std::make_pair(pVictim->GetUID(), 0));
		}
		m_mapHitTable[pVictim->GetUID()]++;
	}
}

void GTalentHit::OnGainDamage( GEntityActor* pTarget )
{
	VALID(pTarget);
	m_setHitsegment.insert(m_nHitIndex);
}

void GTalentHit::OnExit( GTalent* pTalent )
{
	GEntityActor* pOwner = pTalent->GetOwner();
	VALID(pOwner);
	GTalentInfo* pTalentInfo = pTalent->GetInfo();
	VALID(pTalentInfo);

	// OnHitAll
	//
	//////////////////////////////////////////////////////////////////////////
	if ((int)pTalentInfo->m_HitInfo.m_vSegments.size() == m_setHitsegment.size() || // 모두 맞췄거나
		(!pTalentInfo->m_bSeparateHit && !m_setHitsegment.empty()))	// 분할 판정이 아닐 때 한번이라도 맞춘 경우
	{
		pOwner->OnHitAll(pTalentInfo);
	}

	// OnHitSuccss
	//
	//////////////////////////////////////////////////////////////////////////
	if (!m_setHitsegment.empty())
	{
		pOwner->OnHitSuccess(pTalentInfo);
	}
}
