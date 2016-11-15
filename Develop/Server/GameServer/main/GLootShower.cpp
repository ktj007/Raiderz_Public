#include "StdAfx.h"
#include "GLootShower.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GGlobal.h"
#include "GNPCMgr.h"
#include "GPlayerDoing.h"
#include "GInteractionSystem.h"
#include "CCommandResultTable.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GDropItems.h"
#include "GLootSystem.h"
#include "GDropSilver.h"
#include "GDBManager.h"
#include "GMoneySystem.h"
#include "GLootModifyHandler.h"
#include "AStlUtil.h"
#include "GPlayerObjectManager.h"
#include "GDBTaskData.h"
#include "GDBCacheData.h"


bool GLootShower::Show(GEntityPlayer* pPlayer, const MUID& uidNPC)
{
	VALID_RET(pPlayer, false);

	GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidNPC);
	VALID_RET(pNPC, false);

	if (false == Check(pPlayer, pNPC)) return false;
	if (false == Apply(pPlayer, pNPC)) return false;
	Route(pPlayer, pNPC);	

	return true;
}

bool GLootShower::Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	if (false == pPlayer->GetDoing().IsNowLooting()) return false;
	if (false == gsys.pInteractionSystem->IsInteractableDistance(pPlayer, pNPC)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_TARGET_DISTANCE);

	if (true == pNPC->GetNPCLoot().GetDropList().IsEmpty())return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_NOT_EXIST_DROPITEM);
	if (false == pNPC->GetNPCLoot().GetDropList().IsViewableCID(pPlayer->GetCID())) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_NOT_EXIST_DROPITEM);

	return true;
}

bool GLootShower::Apply( GEntityPlayer* pPlayer, GEntityNPC* pNPC )
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	set<int> setPreViewableCID;
	pNPC->GetNPCLoot().GetDropList().GetViewableCID(setPreViewableCID);

	GLootMoneyDistributor moneyDistributor;
	if (false == moneyDistributor.Distribute(pPlayer, pNPC)) return false;

	set<int> setPostVisbleCID;
	pNPC->GetNPCLoot().GetDropList().GetViewableCID(setPostVisbleCID);
	gsys.pLootSystem->GetModifyHandler().HandleUnviewablePlayer(setPreViewableCID, setPostVisbleCID, pNPC);
	gsys.pLootSystem->GetModifyHandler().HandleEmptyNPC(pNPC);

	return true;
}

void GLootShower::Route(GEntityPlayer* pPlayer, GEntityNPC* pNPC)
{
	VALID(pPlayer);
	VALID(pNPC);

	if (true == pNPC->GetNPCLoot().GetDropList().IsEmpty()) return;

	vector<TD_ITEM_DROP> vecTDDropItem = pNPC->GetNPCLoot().GetDropList().GetDropItems().MakeVisbleTDDropItem(pPlayer->GetCID());
	MCommand* pSeeItemCmd = MakeNewCommand(MC_LOOT_SEE_ITEM, 
		1,				
		NEW_BLOB(vecTDDropItem));
	pPlayer->RouteToMe(pSeeItemCmd);
}

bool GLootMoneyDistributor::Distribute( GEntityPlayer* pPlayer, GEntityNPC* pNPC )
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	set<int> setMoneyAuthorizedCID;
	pNPC->GetNPCLoot().GetDropList().GetDropMoney().GetAuthorizedCID(setMoneyAuthorizedCID);

	int nDistributedMoney = CalcDistributedMoney(pNPC, setMoneyAuthorizedCID);

	if (false == DB(setMoneyAuthorizedCID, nDistributedMoney, pNPC->GetID())) return false;

	Server(pNPC);

	return true;
}

int GLootMoneyDistributor::CalcDistributedMoney( GEntityNPC* pNPC, set<int> setMoneyAuthorizedCID )
{
	int nDropMoney = pNPC->GetNPCLoot().GetDropList().GetDropMoney().GetQuantity();
	int nDistributedMoney = nDropMoney;
	if (0 < setMoneyAuthorizedCID.size())
	{
		nDistributedMoney = static_cast<int>(nDropMoney / setMoneyAuthorizedCID.size());
	}

	return nDistributedMoney;
}

bool GLootMoneyDistributor::DB(set<int> setMoneyAuthorizedCID, int nDistributedMoney, int nNpcID)
{
	vector<GDBTASKDATA_LOOTINCREASEMONEY> vecDBTaskData;
	vector<GDBCACHEDATA_CHARACTER> vecDBCacheData;

	for each (int nCID in setMoneyAuthorizedCID)
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nCID);
		if (NULL == pPlayer) continue;

		if (MAX_MONEY <= pPlayer->GetMoney())
		{
			pPlayer->RouteSystemMsg(CR_FAIL_SYSTEM_OVER_MONEY);
			continue;
		}

		int nUpdateMoney = pPlayer->GetMoney() + nDistributedMoney;
		if (MAX_MONEY - nDistributedMoney< pPlayer->GetMoney())
		{
			nUpdateMoney = MAX_MONEY;
		}

		GDBTASKDATA_LOOTINCREASEMONEY dbTaskData(pPlayer->GetAID()
			, pPlayer->GetUID()
			, nDistributedMoney
			, nUpdateMoney
			, pPlayer->GetPlayerInfo()->nPlayTimeSec
			, pPlayer->GetPlayerInfo()->GetDeltaPlayTime()
			, pPlayer->GetLevel()
			, pPlayer->GetPlayerInfo()->nXP
			, pPlayer->GetCID()
			, nNpcID);

		vecDBTaskData.push_back(dbTaskData);

		GDBCACHEDATA_CHARACTER dbCacheData;
		pPlayer->MakeDBCacheData(dbCacheData);
		vecDBCacheData.push_back(dbCacheData);
	}

	if (1 == vecDBTaskData.size())
	{
		return gsys.pDBManager->LootIncreaseMoney(vecDBTaskData[0], vecDBCacheData[0]);
	}
	else
	{
		return gsys.pDBManager->PartyLootIncreaseMoney(vecDBTaskData, vecDBCacheData);
	}
}

void GLootMoneyDistributor::Server(GEntityNPC* pNPC)
{
	pNPC->GetNPCLoot().GetDropList().GetDropMoney().Clear();
}