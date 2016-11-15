#include "StdAfx.h"
#include "GNPCShopShower.h"
#include "GNPCShopInfo.h"
#include "GNPCShopInfoMgr.h"
#include "GGlobal.h"
#include "GNPCShopSystem.h"
#include "GEntityPlayer.h"
#include "GPlayerFactions.h"
#include "CSNPCShopCalculator.h"
#include "GConditionsSystem.h"
#include "CCommandTable.h"
#include "GCommand.h"

bool GNPCShopShower::Show(GEntityPlayer* pPlayer, int nNPCShopID)
{
	VALID_RET(pPlayer, false);
	GNPCShopInfo* pNPCShopInfo = gmgr.pNPCShopInfoMgr->Get(nNPCShopID);
	VALID_RET(pNPCShopInfo, false);

	Route(pPlayer, pNPCShopInfo);

	return true;
}

void GNPCShopShower::Route(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo)
{
	VALID(pPlayer);
	VALID(pNPCShopInfo);

	FACTION_RELATION_TYPE nFRT = pPlayer->GetPlayerFactions().GetRelation(pNPCShopInfo->m_nFactionID);
	float fBuyMod = CSNPCShopCalculator::CalcBuyMod(pNPCShopInfo->m_fBaseBuyMod, nFRT);
	float fSellMod = CSNPCShopCalculator::CalcSellMod(pNPCShopInfo->m_fBaseSellMod, nFRT);

	vector<int> vecBuyableUsableItemID;
	vector<int> vecBuyableUnusableItemID;
	vector<int> vecUnbuyableItemID;
	CategorizeShopItem(pPlayer, pNPCShopInfo, vecBuyableUsableItemID, vecBuyableUnusableItemID, vecUnbuyableItemID);

	MCommand* pNewCmd = MakeNewCommand(MC_NPCSHOP_SHOW, 
		6, 
		NEW_BLOB(vecBuyableUsableItemID),
		NEW_BLOB(vecBuyableUnusableItemID),
		NEW_BLOB(vecUnbuyableItemID),
		NEW_FLOAT(fBuyMod),
		NEW_FLOAT(fSellMod),
		NEW_BOOL(pNPCShopInfo->m_bRepairable));

	pPlayer->RouteToMe(pNewCmd);
}

void GNPCShopShower::CategorizeShopItem(GEntityPlayer* pPlayer, GNPCShopInfo* pNPCShopInfo, vector<int>& vecBuyableUsableItemID, vector<int>& vecBuyableUnusableItemID, vector<int>& vecUnbuyableItemID)
{
	VALID(pPlayer);
	VALID(pNPCShopInfo);

	vector<GItemData*> vecBuyableItemData;
	vecBuyableItemData.insert(vecBuyableItemData.end(), pNPCShopInfo->m_vecNormalItem.begin(), pNPCShopInfo->m_vecNormalItem.end());

	for each (const map<GItemData*, GConditionsInfo*>::value_type& val in pNPCShopInfo->m_mapConditionsItem)
	{		
		GItemData* pItemData = val.first;
		if (NULL == pItemData) continue;
		GConditionsInfo* pBuyCondition = val.second;		
		if (NULL == pBuyCondition) continue;

		if (true == gsys.pConditionsSystem->Check(pPlayer, pBuyCondition))
		{
			vecBuyableItemData.push_back(pItemData);			
		}
		else
		{
			vecUnbuyableItemID.push_back(pItemData->m_nID);
		}
	}

	for each (GItemData* pBuyableItemData in vecBuyableItemData)
	{
		if (true == gsys.pConditionsSystem->CheckForItem(pPlayer, pBuyableItemData->m_nConditionsInfoID))
		{
			vecBuyableUsableItemID.push_back(pBuyableItemData->m_nID);
		}
		else
		{
			vecBuyableUnusableItemID.push_back(pBuyableItemData->m_nID);
		}			
	}
}