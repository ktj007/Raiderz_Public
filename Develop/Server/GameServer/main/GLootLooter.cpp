#include "StdAfx.h"
#include "GLootLooter.h"
#include "GGlobal.h"
#include "GNPCMgr.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "GPlayerDoing.h"
#include "CCommandResultTable.h"
#include "GInteractionSystem.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GDropItems.h"
#include "GDropItem.h"
#include "GItemHolder.h"
#include "GDBManager.h"
#include "GPlayerObjectManager.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GMasterServerFacade.h"
#include "CCommandTable_Master.h"
#include "AStlUtil.h"
#include "GPartySystem.h"
#include "GConfig.h"
#include "GLootSystem.h"
#include "GItemAdder.h"
#include "GLootItemGetter.h"
#include "GLootModifyHandler.h"


bool GLootLooter::LootItem(GEntityPlayer* pPlayer, const MUID& uidNPC, const vector<MUID>& vecDropItemUID)
{
	VALID_RET(pPlayer, false);
	
	GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(uidNPC);
	RVALID_RET(pNPC, false);

	if (false == LootItem_Check(pPlayer, pNPC, vecDropItemUID)) return false;

	LootItem_Apply(pPlayer, pNPC, vecDropItemUID);

	return true;
}

bool GLootLooter::LootItem_Check(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<MUID>& vecDropItemUID)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pNPC, false);

	if (true == vecDropItemUID.empty()) return false;
	if (false == pPlayer->GetDoing().IsNowLooting()) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_NOT_LOOTING);
	if (false == gsys.pInteractionSystem->IsInteractableDistance(pPlayer, pNPC)) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_TARGET_DISTANCE);

	for each (const MUID& nDropItemUID in vecDropItemUID)
	{
		GDropItem* pDropItem = pNPC->GetNPCLoot().GetDropList().GetDropItems().GetDropItem(nDropItemUID);
		if (NULL == pDropItem) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_NOT_EXIST_ITEM);
		if (false == pDropItem->IsViewableCID(pPlayer->GetCID())) return pPlayer->FailAndRouteSystemMsg(CR_FAIL_LOOT_NOT_EXIST_ITEM);
	}	

	return true;
}
void GLootLooter::LootItem_Apply(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<MUID>& vecDropItemUID)
{
	VALID(pPlayer);
	VALID(pNPC);

	vector<GDropItem*> vecGettableItem;
	vector<GDropItem*> vecMasterLootableItem;
	vector<GDropItem*> vecRoolableItem;
	for each (const MUID& nDropItemUID in vecDropItemUID)
	{
		GDropItem* pDropItem = pNPC->GetNPCLoot().GetDropList().GetDropItems().GetDropItem(nDropItemUID);
		if (NULL == pDropItem) continue;

		if (true == pDropItem->IsGettable())
		{
			vecGettableItem.push_back(pDropItem);
		}
		else if (true == pDropItem->IsMasterLootable())
		{
			vecMasterLootableItem.push_back(pDropItem);
		}
		else if (true == pDropItem->IsRollable())
		{
			vecRoolableItem.push_back(pDropItem);
		}
	}

	set<int> setPreViewableCID;
	pNPC->GetNPCLoot().GetDropList().GetViewableCID(setPreViewableCID);

	GetItem(pPlayer, pNPC, vecGettableItem);
	RollItem(pPlayer, pNPC, vecRoolableItem);
	PrepareMasterLootItem(pPlayer, pNPC, vecMasterLootableItem);

	gsys.pLootSystem->GetModifyHandler().HandleDropListModification(pNPC, setPreViewableCID);
}

UNGETTABLE_ITEM_REASON GLootLooter::GetItem(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem)
{
	return gsys.pLootSystem->GetItemGetter().GetItem(pPlayer->GetCID(), pNPC, vecDropItem);	
}

void GLootLooter::RollItem(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem)
{

	VALID(pPlayer);
	VALID(pNPC);

	if (true == vecDropItem.empty()) return;

	const set<int>& setBeneficiaryCID = pNPC->GetNPCLoot().GetDropList().GetBeneficiaryCID();

	vector<TD_LOOT_ROLL_ITEM> vecTDRollItem;
	vector<TD_LOOT_ROLL_RESULT> vecTDRollResult;	
	MAP_ROLLWINNERITEM mapRollWinnerItem;

	RollItem_MakeData(pNPC, vecDropItem, setBeneficiaryCID, vecTDRollResult, vecTDRollItem, mapRollWinnerItem);
	RollItem_Route(setBeneficiaryCID, vecTDRollItem, vecTDRollResult);
	RollItem_Apply(pNPC, mapRollWinnerItem);
}

void GLootLooter::RollItem_MakeData(GEntityNPC* pNPC, const vector<GDropItem*> &vecDropItem, const set<int>& setBeneficiaryCID, vector<TD_LOOT_ROLL_RESULT> &outvecTDRollResult, vector<TD_LOOT_ROLL_ITEM> &outvecTDRollItem, MAP_ROLLWINNERITEM& outmapRollWinnerItem)
{
	VALID(pNPC);	

	int nDropItemSize = vecDropItem.size();
	for (int i=0; i<nDropItemSize; i++)
	{
		GDropItem* pDropItem = vecDropItem[i];

		int nTopCID = 0;
		int nTopResult = 0;
		for each (int nBeneficiaryCID in setBeneficiaryCID)
		{
			int nRollResult = RandomNumber(1, 100);
			if (nTopResult < nRollResult)
			{
				nTopCID = nBeneficiaryCID;
				nTopResult = nRollResult;	
			}
			else if (nTopResult == nRollResult)
			{
				nRollResult = max(nRollResult-1, 0);
			}

			MUID nBeneficiaryUID = MUID::ZERO;			
			GEntityPlayer* pBeneficiary = gmgr.pPlayerObjectManager->GetEntity(nBeneficiaryCID); 
			if (NULL != pBeneficiary)
			{
				nBeneficiaryUID = pBeneficiary->GetUID();
			}
			else
			{
				GParty* pParty = gsys.pPartySystem->FindParty(pNPC->GetNPCLoot().GetDropList().GetPartyUID());
				if (NULL != pParty)
				{
					nBeneficiaryUID = pParty->FindMemberUID(nBeneficiaryCID);
				}
			}

			outvecTDRollResult.push_back(TD_LOOT_ROLL_RESULT(i, nBeneficiaryUID, nRollResult));
		}

		outvecTDRollItem.push_back(TD_LOOT_ROLL_ITEM(i, pDropItem->GetID(), pDropItem->GetAmount()));
		MapSecondVectorInserter(outmapRollWinnerItem, nTopCID, pDropItem);
	}
}

void GLootLooter::RollItem_Route( const set<int>& setBeneficiaryCID, const vector<TD_LOOT_ROLL_ITEM>& vecTDRollItem, const vector<TD_LOOT_ROLL_RESULT>& vecTDRollResult )
{
	set<int> setOtherServerBeneficiaryCID;
	for each (int nBeneficiaryCID in setBeneficiaryCID)
	{
		GEntityPlayer* pBeneficiary = gmgr.pPlayerObjectManager->GetEntity(nBeneficiaryCID);
		if (NULL != pBeneficiary)
		{
			MCommand* pNewCmd = MakeNewCommand(MC_LOOT_ROLLRESULT, 2,
				NEW_BLOB(vecTDRollItem),
				NEW_BLOB(vecTDRollResult));

			pBeneficiary->RouteToMe(pNewCmd);
		}
		else
		{
			setOtherServerBeneficiaryCID.insert(nBeneficiaryCID);
		}
	}

	if (false == setOtherServerBeneficiaryCID.empty())
	{
		MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_LOOT_ROLLRESULT_REQ,
			3,
			NEW_BLOB(SetToVector(setOtherServerBeneficiaryCID)),
			NEW_BLOB(vecTDRollItem),
			NEW_BLOB(vecTDRollResult));

		gsys.pMasterServerFacade->Route(pNewCmd);
	}
}

void GLootLooter::RollItem_Apply(GEntityNPC* pNPC, const MAP_ROLLWINNERITEM& mapRollWinnerItem)
{
	VALID(pNPC);

	for each (const MAP_ROLLWINNERITEM::value_type& val in mapRollWinnerItem)
	{
		gsys.pLootSystem->GetItemGetter().GetItem(val.first, pNPC, val.second);
	}
}


void GLootLooter::PrepareMasterLootItem(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem)
{
	VALID(pPlayer);
	VALID(pNPC);

	if (true == vecDropItem.empty()) return;

	int nMasterCID = pNPC->GetNPCLoot().GetDropList().GetMasterCID();
	if (NULL ==  pNPC->FindPlayerByCID(nMasterCID)) return pPlayer->RouteSystemMsg(CR_FAIL_LOOT_MASTER_FAR_DISTANCE);

	PreapareMasterLootItem_Apply(vecDropItem);
	PrepareMasterLootItem_Route(nMasterCID, pNPC, vecDropItem);
}

void GLootLooter::PreapareMasterLootItem_Apply( const vector<GDropItem*>& vecDropItem )
{
	for each (GDropItem* pDropItem in vecDropItem)
	{
		pDropItem->Unvisible();
	}
}

void GLootLooter::PrepareMasterLootItem_Route( int nMasterCID, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem )
{
	vector<TD_LOOT_MASTERLOOT_UI>		vecTDLootMasterLootUI;
	vector<TD_LOOT_MASTERLOOT_NOTIFY>	vecTDLootMasterLootNotify;	
	for each (GDropItem* pDropItem in vecDropItem)
	{
		vecTDLootMasterLootUI.push_back(pDropItem->MakeTDMasterLootUI(pNPC));
		vecTDLootMasterLootNotify.push_back(pDropItem->MakeTDMasterLootNotify());
	}

	const set<int>& setBeneficiaryCID = pNPC->GetNPCLoot().GetDropList().GetBeneficiaryCID();

	set<int> setOtherServerBeneficiaryCID;
	for each (int nBeneficiaryCID in setBeneficiaryCID)
	{
		GEntityPlayer* pBeneficiary = gmgr.pPlayerObjectManager->GetEntity(nBeneficiaryCID);
		if (NULL != pBeneficiary)
		{
			if (nBeneficiaryCID == nMasterCID)
			{
				MCommand* pNewCmd = MakeNewCommand(MC_LOOT_MASTERLOOT_UI,
					4, 
					NEW_INT(GConfig::m_nMyServerID),
					NEW_UID(pNPC->GetFieldUID()),
					NEW_UID(pNPC->GetUID()), 
					NEW_BLOB(vecTDLootMasterLootUI));
				pBeneficiary->RouteToMe(pNewCmd);
			}
			else
			{
				MCommand* pNewCmd = MakeNewCommand(MC_LOOT_MASTERLOOT_NOTIFY,
					1, 
					NEW_BLOB(vecTDLootMasterLootNotify));
				pBeneficiary->RouteToMe(pNewCmd);
			}
		}
		else
		{
			setOtherServerBeneficiaryCID.insert(nBeneficiaryCID);
		}		
	}

	if (false == setOtherServerBeneficiaryCID.empty())
	{
		MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_LOOT_MASTERLOOT_NOTIFY_REQ,
			2,
			NEW_BLOB(SetToVector(setOtherServerBeneficiaryCID)),
			NEW_BLOB(vecTDLootMasterLootNotify));

		gsys.pMasterServerFacade->Route(pNewCmd);
	}
}