#include "stdafx.h"
#include "XHitEffectSelecter.h"
#include "XTalentHitParam.h"
#include "XNonPlayer.h"

XHitEffectSelecter::XHitEffectSelecter()
{

}

void XHitEffectSelecter::AddHitInfo( XTalentHitParam* pHitParam, unsigned int nDelayTime )
{
	if(pHitParam->nMFState == MF_GRAPPLED ||
		pHitParam->nMFState == MF_SWALLOWED)
		return;

	bool bNew = false;
	stHitInfo* pHitEffectInfo = CreateHitInfo(pHitParam->nAttackerUIID, pHitParam->nTalentID, bNew);
	
	// 조건에 관계없이 무조건 1빠다!!!
	if(CheckForceFirtstHitEffectByGrade(pHitParam->nVictimUIID))
		pHitParam->bSecondaryEffect = false;

	// DelayTime이 0인가?
	if(nDelayTime == 0)
	{
		// 그럼 선착순으로 하자
		pHitEffectInfo->eType = HEST_ORDER;

		if(bNew)
		{
			pHitEffectInfo->fRemainedTime = HIT_INFO_AUTO_DELETE_TIME;
			pHitParam->bSecondaryEffect = false;
		}

		return;
	}
	else if(pHitEffectInfo->eType == HEST_ORDER)
	{
		// 정렬해서 거리순으로 하자.
		pHitEffectInfo->eType = HEST_DISTANCE_SORT;
		pHitEffectInfo->fRemainedTime = HIT_INFO_AUTO_DELETE_TIME;
	}

	// 자동 삭제 시간보다 크다면...
	float fDelayTime = (float)nDelayTime / 1000.0f;
	if(pHitEffectInfo->fRemainedTime < fDelayTime)
		pHitEffectInfo->fRemainedTime = fDelayTime;

	pHitEffectInfo->vecHitParam.push_back(pHitParam);
}

void XHitEffectSelecter::Update( float fDelta )
{
	for(vector<stHitInfo>::iterator it = m_vecHitbyTalentInfo.begin(); it != m_vecHitbyTalentInfo.end();)
	{
		it->fRemainedTime -= fDelta;
		if(it->fRemainedTime <= 0.0f)
		{
			it = m_vecHitbyTalentInfo.erase(it);
			continue;
		}

		++it;
	}
}

void XHitEffectSelecter::DelHitInfobyAttacker( UIID uiidAttacker, int nTalentID )
{
	for(vector<stHitInfo>::iterator it = m_vecHitbyTalentInfo.begin(); it != m_vecHitbyTalentInfo.end();)
	{
		if(it->uiidAttacker == uiidAttacker &&
			it->nTalentID == nTalentID)
		{
			it = m_vecHitbyTalentInfo.erase(it);
			continue;
		}

		 ++it;
	}
}

XHitEffectSelecter::stHitInfo* XHitEffectSelecter::GetHitInfo( UIID uiidAttacker, int nTalentID )
{
	for(vector<stHitInfo>::iterator it = m_vecHitbyTalentInfo.begin(); it != m_vecHitbyTalentInfo.end(); ++it)
	{
		if(it->uiidAttacker == uiidAttacker &&
			it->nTalentID == nTalentID)
		{
			return &(*it);
		}
	}

	return NULL;
}

void XHitEffectSelecter::StartHitEffectSelect( XTalentHitParam* pHitParam )
{
	stHitInfo* pHitInfo = GetHitInfo(pHitParam->nAttackerUIID, pHitParam->nTalentID);
	XObject* pAttacker = gg.omgr->FindActor_UIID(pHitParam->nAttackerUIID);

	if(pHitInfo == NULL ||
		pAttacker == NULL)
	{
		DelHitInfobyAttacker(pHitParam->nAttackerUIID, pHitParam->nTalentID);
		return;
	}

	if(pHitInfo->eType == HEST_ORDER)
		return;

	vec3 vAttacker = pAttacker->GetPosition();
	float fMinLen = FLT_MAX;
	XTalentHitParam* pMinHitParam = NULL;

	for(vector<XTalentHitParam*>::iterator it = pHitInfo->vecHitParam.begin(); it != pHitInfo->vecHitParam.end(); ++it)
	{
		XObject* pHiter = gg.omgr->FindActor_UIID((*it)->nVictimUIID);
		if(pHiter == NULL)
			continue;

		float fLen = (pHiter->GetPosition() - vAttacker).Length();
		if(fLen < fMinLen)
		{
			fMinLen = fLen;
			pMinHitParam = *it;
		}
	}

	if(pMinHitParam)
	{
		pMinHitParam->bSecondaryEffect = false;
	}

	DelHitInfobyAttacker(pHitParam->nAttackerUIID, pHitParam->nTalentID);
}

XHitEffectSelecter::stHitInfo* XHitEffectSelecter::CreateHitInfo( UIID uiidAttacker, int nTalentID, bool& bNew )
{
	bNew = false;
	stHitInfo* pHitEffectInfo = GetHitInfo(uiidAttacker, nTalentID);
	if(pHitEffectInfo == NULL)
	{
		stHitInfo hitInfo;
		hitInfo.uiidAttacker	= uiidAttacker;
		hitInfo.nTalentID		= nTalentID;
		m_vecHitbyTalentInfo.push_back(hitInfo);

		bNew = true;

		pHitEffectInfo = GetHitInfo(uiidAttacker, nTalentID);
	}

	return pHitEffectInfo;
}

bool XHitEffectSelecter::CheckForceFirtstHitEffectByGrade( UIID uiidVictim )
{
	// 등급에 의한 First 이펙트 체크
	XObject* pHiter = gg.omgr->FindActor_UIID(uiidVictim);
	if(pHiter == NULL)
		return false;

	// NPC 이외는 무조건 First
	if(pHiter->GetType() != XOBJ_NPC)
		return true;

	XNonPlayer* pNonPlayer = pHiter->AsNPC();
	XNPCInfo* pNonInfo = pNonPlayer->GetInfo();
	if(pNonInfo == NULL)
		return false;

	// 4등급 이상에는 무조건 First
	if(pNonInfo->nGrade >= 4)
		return true;

	return false;
}