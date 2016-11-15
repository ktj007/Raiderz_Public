#include "stdafx.h"
#include "GFactionSystem.h"
#include "GDef.h"
#include "GGlobal.h"
#include "GFactionInfoMgr.h"
#include "GEntityPlayer.h"
#include "GPlayerFactions.h"
#include "GFactionInfo.h"
#include "CSFactionCalculator.h"
#include "GDBManager.h"
#include "GCommand.h"
#include "CCommandTable.h"
#include "GNPCInfo.h"
#include "GEntityNPC.h"
#include "GDBTaskDataFaction.h"
#include "GPlayerNPCIconSender.h"
#include "CSConditionInfo.h"
#include "GPlayerFaction.h"

void GFactionSystem::RewardFaction(const vector<GEntityPlayer*>& vecBeneficiary, GNPCInfo* pNPCInfo)
{
	if (NULL == pNPCInfo) return;

	for each (GEntityPlayer* pBeneficiary in vecBeneficiary)
	{
		Increase(pBeneficiary, pNPCInfo->m_nFactionGainID, pNPCInfo->m_nFactionGainQuantity);
		Decrease(pBeneficiary, pNPCInfo->m_nFactionLossID, pNPCInfo->m_nFactionLossQuantity);
	}
}

bool GFactionSystem::Increase(GEntityPlayer* pPlayer, const uint8 nID, const uint16 nQuantity)
{
	if (NULL == pPlayer)
		return false;

	if (0 == nQuantity)
		return true;

	GFactionInfo* pFactionInfo = gmgr.pFactionInfoMgr->Get(nID);
	if (NULL == pFactionInfo)
		return false;

	const bool bExistsFaction = pPlayer->GetPlayerFactions().IsExist(nID);
	const uint16 nCurrentQuantiy = pPlayer->GetPlayerFactions().GetQuantity(nID);	

	if (CSFactionCalculator::GetMaxQuantity() == nCurrentQuantiy)
		return true;
	
	const uint16 nBeforeQuantityPercent = pPlayer->GetPlayerFactions().GetQuantityPercent(nID);

	// 처음 등록되었을 때와 퍼센트가 변경되었을때만 클라이언트에 알려준다.
	if ((nBeforeQuantityPercent != IncreasePlayer(pPlayer, nID, CalcIncreasableQuantity(nQuantity, nCurrentQuantiy))) || !bExistsFaction)
	{
		Increase_Route(pPlayer, nID);
		pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_FACTION);
	}

	GDBT_DATA_FACTION data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetCID(), nID, pPlayer->GetPlayerFactions().GetQuantity(nID));
	
	if (!bExistsFaction)
		gsys.pDBManager->FactionInsert(data);			
	else
		gsys.pDBManager->FactionUpdate(data);			
	
	return true;
}

uint16 GFactionSystem::CalcIncreasableQuantity( const uint16 nQuantity, const uint16 nCurrentQuantiy )
{
	if (CSFactionCalculator::GetMaxQuantity() < (nCurrentQuantiy + nQuantity))
		return CSFactionCalculator::GetMaxQuantity() - nCurrentQuantiy;

	return nQuantity;
}

uint16 GFactionSystem::IncreasePlayer(GEntityPlayer* pPlayer, const uint8 nID, const uint16 nQuantity)
{
	pPlayer->GetPlayerFactions().Increase(nID, nQuantity);	
	
	return pPlayer->GetPlayerFactions().GetQuantityPercent(nID);
}

void GFactionSystem::Increase_Route(GEntityPlayer* pPlayer, const uint8 nID)
{
	uint16 nQuantity = pPlayer->GetPlayerFactions().GetQuantity(nID);

	MCommand* pNewCmd = MakeNewCommand(MC_FACTION_INCREASE,
		2,
		NEW_UCHAR(nID),
		NEW_USHORT(nQuantity));

	pPlayer->RouteToMe(pNewCmd);
}

bool GFactionSystem::Decrease(GEntityPlayer* pPlayer, uint8 nID, uint16 nQuantity)
{
	if (NULL == pPlayer)
		return false;

	if (0 == nQuantity)
		return true;

	GFactionInfo* pFactionInfo = gmgr.pFactionInfoMgr->Get(nID);
	if (NULL == pFactionInfo)
		return false;

	const uint16 nCurrentQuantiy = pPlayer->GetPlayerFactions().GetQuantity(nID);	
	if (0 == nCurrentQuantiy) 
		return true;

	const uint16 nBeforeQuantityPercent = pPlayer->GetPlayerFactions().GetQuantityPercent(nID);
	
	if (nBeforeQuantityPercent != DecreasePlayer(pPlayer, nID, CalcDecreasableQuantity(nQuantity, nCurrentQuantiy)))
	{
		Decrease_Route(pPlayer, nID);
		pPlayer->GetNPCIconSender().SendByPlayerConditionChange(CT_FACTION);
	}

	GDBT_DATA_FACTION data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetCID(), nID, pPlayer->GetPlayerFactions().GetQuantity(nID));

	gsys.pDBManager->FactionUpdate(data);

	return true;
}

uint16 GFactionSystem::CalcDecreasableQuantity( uint16 nQuantity, const uint16 nCurrentQuantiy )
{
	if (nQuantity > nCurrentQuantiy)
		return nCurrentQuantiy;
	
	return nQuantity;
}

uint16 GFactionSystem::DecreasePlayer( GEntityPlayer* pPlayer, const uint8 nID, const uint16 nQuantity )
{
	pPlayer->GetPlayerFactions().Decrease(nID, nQuantity);
	return pPlayer->GetPlayerFactions().GetQuantityPercent(nID);
}

void GFactionSystem::Decrease_Route(GEntityPlayer* pPlayer, uint8 nID)
{
	uint16 nQuantity = pPlayer->GetPlayerFactions().GetQuantity(nID);

	MCommand* pNewCmd = MakeNewCommand(MC_FACTION_DECREASE,
		2,
		NEW_UCHAR(nID),
		NEW_USHORT(nQuantity));

	pPlayer->RouteToMe(pNewCmd);
}

bool GFactionSystem::Set(GEntityPlayer* pPlayer, const uint8 nID, const uint16 nQuantity)
{
	if (NULL == pPlayer) return false;

	GPlayerFactions& factions = pPlayer->GetPlayerFactions();

	int nCurrentQuantity = factions.GetQuantity(nID);

	if (nQuantity < nCurrentQuantity)
	{
		Decrease(pPlayer, nID, nCurrentQuantity - nQuantity);
	}
	else if (nCurrentQuantity < nQuantity)
	{
		Increase(pPlayer, nID, nQuantity - nCurrentQuantity);
	}

	return true;
}

void GFactionSystem::Reset( GEntityPlayer* pPlayer )
{
	VALID(pPlayer);

	vector<GPlayerFaction*> vecPlayerFaction;
	pPlayer->GetPlayerFactions().GetAll(vecPlayerFaction);

	for each (GPlayerFaction* pPlayerFaction in vecPlayerFaction)
	{
		int nCurrentQuantity = pPlayerFaction->GetQuantity();
		int nDefaultQuantity = pPlayerFaction->GetInfo()->m_nDefaultQuantity;

		if (nCurrentQuantity < nDefaultQuantity)
		{
			Increase(pPlayer, pPlayerFaction->GetID(), nCurrentQuantity - nDefaultQuantity);
		}
		else if (nCurrentQuantity > nDefaultQuantity)
		{
			Decrease(pPlayer, pPlayerFaction->GetID(), nDefaultQuantity - nCurrentQuantity);
		}
	}
}

