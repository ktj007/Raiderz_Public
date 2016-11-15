#include "StdAfx.h"
#include "GUseCostMgr.h"
#include "GTalentInfo.h"
#include "GEntityPlayer.h"
#include "GBuffInfo.h"

GUseCostMgr::GUseCostMgr(void)
{
}

GUseCostMgr::~GUseCostMgr(void)
{
}

bool GUseCostMgr::IsUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	if (pUser->IsPlayer())
	{
		if (pTalentInfo->m_nSTACost > 0 &&
			pUser->GetSTA() < pTalentInfo->m_nSTACost)
			return false;
	}

	if (pTalentInfo->m_nENCost > 0 &&
		pUser->GetEN() < pTalentInfo->m_nENCost)
		return false;
	
	if (pTalentInfo->m_nHPCost > 0 &&
		pUser->GetHP() < pTalentInfo->m_nHPCost)
		return false;

	return true;
}

bool GUseCostMgr::IsMaintenanceBuff( GEntityActor* pUser, GBuffInfo* pTalentInfo )
{
	if (pUser->GetSTA() < pTalentInfo->m_nSTACost)
		return false;
	if (pUser->GetEN() < pTalentInfo->m_nENCost)
		return false;
	if (pUser->GetHP() < pTalentInfo->m_nHPCost)
		return false;

	return true;
}

bool GUseCostMgr::Pay_TalentCost( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	VALID_RET(pUser, false);
	VALID_RET(pTalentInfo, false);

	if (pUser->IsPlayer())
	{
		if (ToEntityPlayer(pUser)->GetPlayerInfo()->IsGod())
			return true;
	}

	if (!IsUseTalent(pUser, pTalentInfo))
		return false;

	Pay_TalentCostForce(pUser, pTalentInfo);

	return true;
}

void GUseCostMgr::Pay_TalentCostForce( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	pUser->SetSTA(pUser->GetSTA() - pTalentInfo->m_nSTACost);
	pUser->SetEN(pUser->GetEN() - pTalentInfo->m_nENCost);
	pUser->SetHP(pUser->GetHP() - pTalentInfo->m_nHPCost);

	// 탤런트 비용으로 죽이지는 않는다.
	if (pUser->GetHP() <= 0)
	{
		pUser->SetHP(1);
	}
}


bool GUseCostMgr::Pay_BuffMaintenanceCost( GEntityActor* pUser, GBuffInfo* pBuffInfo, int nTickCounter )
{
	VALID_RET(pUser, false);
	VALID_RET(pBuffInfo, false);
	VALID_RET(nTickCounter>0, false);

	if (pUser->IsBuffEntity())
		return true; // 엔티티 버프면 비용을 지불하지 않음

	for (int i=0; i<nTickCounter; i++)
	{
		if (!IsMaintenanceBuff(pUser, pBuffInfo))
			return false;

		pUser->SetSTA(pUser->GetSTA() - pBuffInfo->m_nSTACost);
		pUser->SetEN(pUser->GetEN() - pBuffInfo->m_nENCost);
		pUser->SetHP(pUser->GetHP() - pBuffInfo->m_nHPCost);
	}
	
	return true;
}

