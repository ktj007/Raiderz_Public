#include "StdAfx.h"
#include "GUTHelper_Faction.h"
#include "GFactionInfoMgr.h"
#include "GFactionRelationInfoMgr.h"
#include "SUnitTestUtil.h"
#include "CSFactionCalculator.h"
#include "GEntityPlayer.h"
#include "GPlayerFactions.h"
#include "GFactionInfo.h"

GFactionInfo* GUTHelper_Faction::NewFactionInfo(uint16 nDefaultQuantity, uint8 nID)
{
	if (nID == (uint8)INVALID_ID)
	{
		nID = SUnitTestUtil::NewUINT8ID();
	}

	GFactionInfo* pFactionInfo = gmgr.pFactionInfoMgr->Get(nID);
	if (NULL != pFactionInfo)
		return pFactionInfo;

	pFactionInfo = gmgr.pFactionInfoMgr->New(nID, nDefaultQuantity);
	gmgr.pFactionInfoMgr->Insert(pFactionInfo);

	return pFactionInfo;
}

GFactionRelationInfo* GUTHelper_Faction::NewFactionRelationInfo(uint8 nID1, uint8 nID2, FACTION_RELATION_TYPE nFRT)
{
	GFactionRelationInfo* pFactionRelationInfo = gmgr.pFactionRelationInfoMgr->New(nID1, nID2, nFRT);
	gmgr.pFactionRelationInfoMgr->Insert(pFactionRelationInfo);

	return pFactionRelationInfo;
}

GFactionInfo* GUTHelper_Faction::SetWorstFaction( GEntityPlayer* pPlayer, int nID )
{
	return SetFaction(pPlayer, CSFactionCalculator::GetMinQuantity(), nID);
}

GFactionInfo* GUTHelper_Faction::SetBadFaction( GEntityPlayer* pPlayer, int nID )
{
	return SetFaction(pPlayer, CSFactionCalculator::GetBadMinQuantity(), nID);
}

GFactionInfo* GUTHelper_Faction::SetGoodFaction(GEntityPlayer* pPlayer, int nID)
{
	return SetFaction(pPlayer, CSFactionCalculator::GetGoodMinQuantity(), nID);
}

GFactionInfo* GUTHelper_Faction::SetExcellentFaction( GEntityPlayer* pPlayer, int nID )
{
	return SetFaction(pPlayer, CSFactionCalculator::GetMaxQuantity(), nID);
}

GFactionInfo* GUTHelper_Faction::SetFaction(GEntityPlayer* pPlayer, uint16 nQuantity, int nID)
{
	GFactionInfo* pNewFactionInfo = NewFactionInfo(CSFactionCalculator::GetNormalMinQuantity(), nID);

	int nPlayerFactionQuantity = pPlayer->GetPlayerFactions().GetQuantity(pNewFactionInfo->m_nID);
	if (nQuantity > nPlayerFactionQuantity)
	{
		pPlayer->GetPlayerFactions().Increase(pNewFactionInfo->m_nID, nQuantity - nPlayerFactionQuantity);
	}
	else
	{
		pPlayer->GetPlayerFactions().Decrease(pNewFactionInfo->m_nID, nPlayerFactionQuantity - nQuantity);
	}

	return pNewFactionInfo;
}