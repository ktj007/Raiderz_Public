#include "StdAfx.h"
#include "GItemMoverForDBTask.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GItem.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"
#include "GItemAdder.h"
#include "GItemRemover.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GDBCacheSystem.h"
#include "GItemManager.h"
#include "GMasterServerFacade.h"
#include "GServerGuildRouter.h"
#include "GGuild.h"
#include "GGuildMgr.h"
#include "GGuildSystem.h"


bool GItemMoverForDBTask::Move(const MUID& uidPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount, int64 nSplitIUID)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (NULL == pPlayer) return false;
	GItem* pFromItem = pPlayer->GetItemHolder()->GetItem(nFromSlotType, nFromSlotID);
	if (NULL == pFromItem) return false;

	GItem* pToItem = pPlayer->GetItemHolder()->GetItem(nToSlotType, nToSlotID);
	if (NULL == pToItem)
	{
		if (pFromItem->GetAmount() == nAmount)
		{
			_Move(pPlayer, pFromItem, nToSlotType, nToSlotID, nAmount);
		}
		else
		{
			Split(pPlayer, pFromItem, nToSlotType, nToSlotID, nAmount, nSplitIUID);
		}
	}
	else
	{
		if (pFromItem->GetID() == pToItem->GetID() && pToItem->GetAmount() < pToItem->m_pItemData->m_nStackAmount)
		{
			Merge(pPlayer, pFromItem, pToItem, nAmount);
		}
		else
		{
			Swap(pPlayer, pFromItem, nFromSlotType, nFromSlotID, pToItem, nToSlotType, nToSlotID);
		}
	}

	Route(pPlayer, nFromSlotType, nFromSlotID, nToSlotType, nToSlotID, nAmount, nSplitIUID);
	
	return true;
}

void GItemMoverForDBTask::_Move(GEntityPlayer* pPlayer, GItem* pFromItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount)
{
	pPlayer->GetItemHolder()->PopItem(pFromItem->m_nSlotType, pFromItem->m_nSlotID);
	pPlayer->GetItemHolder()->PushItem(nToSlotType, nToSlotID, pFromItem);
}

void GItemMoverForDBTask::Split(GEntityPlayer* pPlayer, GItem* pFromItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount, int64 nSplitIUID)
{
	GItem* pItem = gmgr.pItemManager->MakeNewItem(pFromItem, nSplitIUID, nAmount);
	if (NULL == pItem) return;

	if (!pPlayer->GetItemHolder()->PushItem(nToSlotType, nToSlotID, pItem))
	{
		SAFE_DELETE(pItem);
		return;
	}

	pPlayer->GetItemHolder()->DecreaseAmount(pFromItem->m_nSlotType, pFromItem->m_nSlotID, nAmount);
	gsys.pDBCacheSystem->CreateItem(pPlayer->GetUID(), nToSlotType, nToSlotID, nSplitIUID);
}

void GItemMoverForDBTask::Merge( GEntityPlayer* pPlayer, GItem* pFromItem, GItem* pToItem, int nAmount )
{
	pPlayer->GetItemHolder()->DecreaseAmount(pFromItem->m_nSlotType, pFromItem->m_nSlotID, nAmount);
	pPlayer->GetItemHolder()->IncreaseAmount(pToItem->m_nSlotType, pToItem->m_nSlotID, nAmount);

	if (0 == pFromItem->GetAmount())
	{
		pPlayer->GetItemHolder()->PopItem(pFromItem->m_nSlotType, pFromItem->m_nSlotID);
		gsys.pDBCacheSystem->DeleteItem(pPlayer->GetUID(), pFromItem->m_nIUID);
		SAFE_DELETE(pFromItem);
	}
}

void GItemMoverForDBTask::Swap(GEntityPlayer* pPlayer, GItem* pFromItem, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, GItem* pToItem, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID)
{
	pPlayer->GetItemHolder()->PopItem(nFromSlotType, nFromSlotID);
	pPlayer->GetItemHolder()->PopItem(nToSlotType, nToSlotID);

	// PopItem 하면 Item의 nSlotType, nSlotID 가 변경되서 Item정보로 Push하지 않는다.
	pPlayer->GetItemHolder()->PushItem(nToSlotType, nToSlotID, pFromItem);						
	pPlayer->GetItemHolder()->PushItem(nFromSlotType, nFromSlotID, pToItem);
}

void GItemMoverForDBTask::Route(GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount, int64 nSplitIUID)
{
	MCommand* pNewCmd = MakeNewCommand(MC_ITEM_MOVE,		 
		5,		
		NEW_INT(nFromSlotType),
		NEW_INT(nFromSlotID),
		NEW_INT(nToSlotType),
		NEW_INT(nToSlotID),
		NEW_INT(nAmount));

	pPlayer->RouteToMe(pNewCmd);

	if (SLOTTYPE_GUILDSTORAGE == nFromSlotType || SLOTTYPE_GUILDSTORAGE == nToSlotType)
	{
		GGuild* pGuild = gmgr.pGuildMgr->Get(pPlayer->GetGID());
		if (NULL == pGuild) return;

		vector<TD_ITEM> vecTDModStorageItemForClient;
		vector<TD_PLAYER_GAME_DATA_ITEM_INSTANCE> vecTDModStorageItemForMasterServer;
		if (SLOTTYPE_GUILDSTORAGE == nFromSlotType)
		{
			TD_ITEM tdModItemForClient;
			TD_PLAYER_GAME_DATA_ITEM_INSTANCE tdModItemForMasterServer;

			GItem* pItem = pPlayer->GetItemHolder()->GetItem(nFromSlotType, nFromSlotID);
			if (NULL != pItem)
			{
				tdModItemForClient = pItem->ToTD_ITEM();
				tdModItemForMasterServer = pItem->ToTD_PLAYER_GAME_DATA_ITEM_INSTANCE();
			}
			else
			{
				tdModItemForClient.m_nSlotID = nFromSlotID;
				tdModItemForMasterServer.nSlotID = nFromSlotID;				
			}

			vecTDModStorageItemForClient.push_back(tdModItemForClient);
			vecTDModStorageItemForMasterServer.push_back(tdModItemForMasterServer);
		}		
		if (SLOTTYPE_GUILDSTORAGE == nToSlotType)
		{
			TD_ITEM tdModItemForClient;
			TD_PLAYER_GAME_DATA_ITEM_INSTANCE tdModItemForMasterServer;

			GItem* pItem = pPlayer->GetItemHolder()->GetItem(nToSlotType, nToSlotID);
			if (NULL != pItem)
			{
				tdModItemForClient = pItem->ToTD_ITEM();
				tdModItemForMasterServer = pItem->ToTD_PLAYER_GAME_DATA_ITEM_INSTANCE();
			}
			else
			{
				tdModItemForClient.m_nSlotID = nToSlotID;
				tdModItemForMasterServer.nSlotID = nToSlotID;
			}

			vecTDModStorageItemForClient.push_back(tdModItemForClient);
			vecTDModStorageItemForMasterServer.push_back(tdModItemForMasterServer);
		}

		MCommand* pNewCmd = MakeNewCommand(MC_GUILD_MOVE_STORAGEITEM,
			2,
			NEW_WSTR(pPlayer->GetName()), 
			NEW_BLOB(vecTDModStorageItemForClient));

		gsys.pGuildSystem->RouteToStorageInteractionMember(pGuild, pNewCmd, pPlayer->GetCID());
		gsys.pMasterServerFacade->GetGuildRouter()->MoveStorageItem(pPlayer->GetCID(), pPlayer->GetGID(), vecTDModStorageItemForMasterServer);
	}
}

void GItemMoverForDBTask::OnPreCompleted( const MUID& uidPlayer, SH_ITEM_SLOT_TYPE nFromSlotType, int nFromSlotID, SH_ITEM_SLOT_TYPE nToSlotType, int nToSlotID, int nAmount )
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(uidPlayer);
	if (NULL == pPlayer) return;
	GItem* pFromItem = pPlayer->GetItemHolder()->GetItem(nFromSlotType, nFromSlotID);
	if (NULL == pFromItem) return;

	GItem* pToItem = pPlayer->GetItemHolder()->GetItem(nToSlotType, nToSlotID);
	if (NULL == pToItem)
	{
		pPlayer->GetItemHolder()->CancelReserveSlot(nToSlotType, nToSlotID);
	}
}