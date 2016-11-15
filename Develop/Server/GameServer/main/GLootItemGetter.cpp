#include "StdAfx.h"
#include "GLootItemGetter.h"
#include "GDropItem.h"
#include "CSDef_Loot.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GNPCLoot.h"
#include "GDropList.h"
#include "GDropItems.h"
#include "GPlayerObjectManager.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GMasterServerFacade.h"
#include "CCommandTable_Master.h"
#include "AStlUtil.h"
#include "GItemAdder.h"
#include "GItemManager.h"
#include "GNPCMgr.h"
#include "GDBTaskItemData.h"


UNGETTABLE_ITEM_REASON GLootItemGetter::GetItem(int nCID, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem)
{
	VALID_RET(pNPC, UIR_UNADDABLEITEM);

	if (true == vecDropItem.empty()) 
		return UIR_UNADDABLEITEM;

	UNGETTABLE_ITEM_REASON nUIR = CheckGettable(nCID, pNPC, vecDropItem);

	switch (nUIR)
	{
	case UIR_NONE:
		{
			_GetItem(nCID, pNPC, vecDropItem);			
		}
		break;
	case UIR_OTHERSERVER:
		{
			HandleUngettableItem(UIR_OTHERSERVER, nCID, vecDropItem);
		}
		break;
	case UIR_OTHERFIELD:
		{
			HandleUngettableItem(UIR_OTHERFIELD, nCID, vecDropItem);
		}
		break;
	}	

	return nUIR;
}

void GLootItemGetter::_GetItem(int nCID, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem)
{
	VALID(pNPC);
	if (true == vecDropItem.empty()) return;
	GEntityPlayer* pPlayer = pNPC->FindPlayerByCID(nCID);
	VALID(pPlayer);	

	DB(pPlayer, pNPC, vecDropItem);	
	Route(pPlayer, pNPC, vecDropItem);
	RemoveDropItem(pNPC, vecDropItem);
}

void GLootItemGetter::DB(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem)
{
	VALID(pPlayer);
	VALID(pNPC);

	// DB에 같은 아이템의 수량을 여러번 업데이트하는것을 막기 위해서 같은 종류의 아이템들의 수량을 합쳐서 처리한다.
	map<int,int> mapCompressedDropItem = CompressDropItem(vecDropItem);

	for each (const map<int,int>::value_type& compressedDropItem in mapCompressedDropItem)
	{
		int nDropItemID = compressedDropItem.first;
		int nDropItemAmount = compressedDropItem.second;

		GItemData* pItemData = gmgr.pItemManager->GetItemData(nDropItemID);
		if (NULL == pItemData) continue;			

		while (0 < nDropItemAmount)
		{
			int nSingleDropItemAmount = nDropItemAmount;
			if (nSingleDropItemAmount > pItemData->m_nStackAmount)
			{
				nSingleDropItemAmount = pItemData->m_nStackAmount;
			}

			GItemHolder* pItemHolder = pPlayer->GetItemHolder();
			GItem* pItem = pItemHolder->GetMergeableItem(SLOTTYPE_INVENTORY, nDropItemID, nSingleDropItemAmount);

			if (NULL == pItem)
			{
				GDBT_ITEM_DATA data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetMoney(), nSingleDropItemAmount, -1, pItemData->m_nTier, pNPC->GetID());

				data.m_Item.Set(pPlayer->GetCID()
					, SLOTTYPE_INVENTORY
					, pItemHolder->ReserveEmptySlot(SLOTTYPE_INVENTORY, 0)
					, nDropItemID
					, nSingleDropItemAmount
					, pPlayer->GetPlayerInfo()->nPlayTimeSec
					, 0
					, 0
					, pItemData->m_nMaxDurability
					, pItemData->m_nMaxDurability);

				gsys.pDBManager->ItemLootInsert(data);			
			}
			else
			{
				GDBT_ITEM_DATA data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetMoney(), nSingleDropItemAmount, -1, pItemData->m_nTier, pNPC->GetID());

				data.m_Item.Set(pPlayer->GetCID()
					, SLOTTYPE_INVENTORY
					, pItem->m_nSlotID
					, nDropItemID
					, pItem->GetAmount() + nSingleDropItemAmount
					, pPlayer->GetPlayerInfo()->nPlayTimeSec
					, pItem->m_nIUID
					, 0
					, pItem->m_nDurability
					, pItemData->m_nMaxDurability);

				gsys.pDBManager->ItemLootUpdate(data);			
			}

			nDropItemAmount -= nSingleDropItemAmount;
		}		
	}
}

void GLootItemGetter::RemoveDropItem(GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem)
{
	for each (GDropItem* pDropItem in vecDropItem)
	{
		pNPC->GetNPCLoot().GetDropList().GetDropItems().Remove(pDropItem->GetUID());
	}	
}

void GLootItemGetter::Route(GEntityPlayer* pPlayer, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem)
{
	VALID(pPlayer);
	VALID(pNPC);

	vector<TD_LOOT_OTHERGAIN_ITEM> vecTDLootOtherGainItem;
	for each (GDropItem* pDropItem in vecDropItem)
	{
		int nDropItemID = pDropItem->GetID();
		int nDropItemAmount = pDropItem->GetAmount();		
		vecTDLootOtherGainItem.push_back(TD_LOOT_OTHERGAIN_ITEM(nDropItemID, nDropItemAmount));
	}

	const set<int>& setBeneficiaryCID = pNPC->GetNPCLoot().GetDropList().GetBeneficiaryCID();
	set<int> setOtherServerBeneficiaryCID;	
	for each (int nBeneficiaryCID in setBeneficiaryCID)
	{
		if (pPlayer->GetCID() == nBeneficiaryCID) continue;

		GEntityPlayer* pBeneficiary = gmgr.pPlayerObjectManager->GetEntity(nBeneficiaryCID);
		if (NULL != pBeneficiary)
		{
			MCommand* pNewCmd = MakeNewCommand(MC_LOOT_OTHER_GAIN_ITEM, 2,
				NEW_UID(pPlayer->GetUID()),
				NEW_BLOB(vecTDLootOtherGainItem));

			pBeneficiary->RouteToMe(pNewCmd);
		}
		else
		{
			setOtherServerBeneficiaryCID.insert(nBeneficiaryCID);
		}
	}

	if (false == setOtherServerBeneficiaryCID.empty())
	{
		MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_LOOT_OTHER_GAIN_ITEM_REQ,
			3,
			NEW_BLOB(SetToVector(setOtherServerBeneficiaryCID)),
			NEW_UID(pPlayer->GetUID()), 
			NEW_BLOB(vecTDLootOtherGainItem));

		gsys.pMasterServerFacade->Route(pNewCmd);
	}
}

map<int,int> GLootItemGetter::CompressDropItem(const vector<GDropItem*>& vecDropItem)
{
	map<int,int> mapCompressedDropItem;
	for each (GDropItem* pDropItem in vecDropItem)
	{
		map<int,int>::iterator itor =  mapCompressedDropItem.find(pDropItem->GetID());
		if (mapCompressedDropItem.end() == itor)
		{
			mapCompressedDropItem.insert(map<int,int>::value_type(pDropItem->GetID(), pDropItem->GetAmount()));
		}
		else
		{
			itor->second += pDropItem->GetAmount();
		}
	}

	return mapCompressedDropItem;
}

UNGETTABLE_ITEM_REASON GLootItemGetter::CheckGettable(int nCID, GEntityNPC* pNPC, const vector<GDropItem*>& vecDropItem)
{
	VALID_RET(pNPC, UIR_UNADDABLEITEM);

	if (NULL == gmgr.pPlayerObjectManager->GetEntity(nCID))
	{
		return UIR_OTHERSERVER;
	}

	GEntityPlayer* pPlayer = pNPC->FindPlayerByCID(nCID);
	if (NULL == pPlayer)
	{
		return UIR_OTHERFIELD;
	}

	vector<CHECK_ADDABLE_ITEM> vecCheckAddableItem;
	for each (GDropItem* pDropItem in vecDropItem)
	{		
		vecCheckAddableItem.push_back(CHECK_ADDABLE_ITEM(pDropItem->GetID(), pDropItem->GetAmount()));
	}

	if (false == gsys.pItemSystem->GetAdder().CheckItemAddable(pPlayer, vecCheckAddableItem))
	{
		return UIR_UNADDABLEITEM;
	}

	return UIR_NONE;
}

void GLootItemGetter::HandleUngettableItem(UNGETTABLE_ITEM_REASON nUIR, int nCID, const vector<GDropItem*>& vecDropItem)
{
	if (true == vecDropItem.empty()) return;

	vector<TD_LOOT_GETTABLE_ITEM_ADD> vecGettableItemAdd;
	for each (GDropItem* pDropItem in vecDropItem)
	{
		pDropItem->ChangeGettableItem(nCID);
		vecGettableItemAdd.push_back(TD_LOOT_GETTABLE_ITEM_ADD(pDropItem->GetID(), pDropItem->GetAmount(), nUIR));
	}

	if (UIR_OTHERSERVER == nUIR)
	{
		MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_LOOT_GETTABLE_ITEM_ADD_REQ,
			2,
			NEW_INT(nCID),
			NEW_BLOB(vecGettableItemAdd));

		gsys.pMasterServerFacade->Route(pNewCmd);
	}
	else
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nCID);
		if (NULL == pPlayer) return;

		if (UIR_OTHERFIELD == nUIR)
		{
			MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MC_LOOT_GETTABLE_ITEM_ADD,
				1,
				NEW_BLOB(vecGettableItemAdd));

			pPlayer->RouteToMe(pNewCmd);			
		}
	}
}



void GLootItemGetter::GetItemForDBTask(const MUID& nPlayerUID, const MUID& nNPCUID, const MUID& nDropItemUID, int16 nSlotID, int16 nToStackAmt, int16 nStackModAmt, int64 nIUID)
{
	GEntityNPC* pNPC = gmgr.pNPCMgr->GetEntityNPC(nNPCUID);
	VALID(pNPC);
	GDropItem* pDropItem = pNPC->GetNPCLoot().GetDropList().GetDropItems().GetDropItem(nDropItemUID); 
	VALID(pDropItem);

	ADD_ITEM addItem = ADD_ITEM(SLOTTYPE_INVENTORY, nSlotID, nToStackAmt, nStackModAmt, nIUID, pDropItem->GetID());
	gsys.pItemSystem->GetAdder().AddForDBTask(nPlayerUID, addItem);

	pNPC->GetNPCLoot().GetDropList().GetDropItems().Remove(nDropItemUID);
}