#include "stdafx.h"
#include "GDBTaskItemEnchant.h"
#include "GCharacterDBCache.h"
#include "GItemDBCache.h"
#include "GDBCacheSystem.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GCommandTable.h"
#include "GItemHolder.h"
#include "GItem.h"
#include "GCommand.h"
#include "GItemRemover.h"
#include "GPlayerDoing.h"
#include "GDBManager.h"
#include "GBuffInfo.h"


GDBTaskItemEnchant::GDBTaskItemEnchant( const MUID& uidReqPlayer )
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_ITEM_ENCHANT), m_SyncingList(uidReqPlayer)
{

}

bool GDBTaskItemEnchant::Input( const GDBT_ITEM_ENCH& data )
{
	if (!CharSync(data.m_uidPlayer))
		return false;

	if (!ItemSync(data.m_uidPlayer, SLOTTYPE_INVENTORY, data.m_nEStoneItemSlotID, data.m_nEStoneIUID))
		return false;

	if (!ItemSync(data.m_uidPlayer, SLOTTYPE_INVENTORY, data.m_nEAgentItemSlotID, data.m_nEAgentIUID))
		return false;

	if (!ItemSync(data.m_uidPlayer, (SH_ITEM_SLOT_TYPE)data.m_nTargetItemSlotType, data.m_nTargetItemSlotID, data.m_nTargetIUID))
		return false;
	
	m_Data = data;

	return true;
}

void GDBTaskItemEnchant::OnExecute( mdb::MDatabase& rfDB )
{
	mdb::MDatabaseQuery q(&rfDB);
	ExecuteW(q, GetSQLW(ENCHANT));
}

mdb::MDB_THRTASK_RESULT GDBTaskItemEnchant::_OnCompleted()
{
	gsys.pDBManager->ItemEnchLog(m_Data);

	// 이벤트 호출
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (NULL == pPlayer)
		return mdb::MDBTR_SUCESS;

	GItem* pItem = pPlayer->GetItemHolder()->GetItem((SH_ITEM_SLOT_TYPE)m_Data.m_nTargetItemSlotType, m_Data.m_nTargetItemSlotID);
	if (NULL == pItem)
		return mdb::MDBTR_SUCESS;
	
	GItem* pStone = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, m_Data.m_nEStoneItemSlotID); 
	if (NULL == pStone) 
		return mdb::MDBTR_SUCESS;

	GItem* pAgent = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, m_Data.m_nEAgentItemSlotID); 
	if (NULL == pStone)
		return mdb::MDBTR_SUCESS;

	vector<REMOVE_ITEM> vecRemoveItem;
	vecRemoveItem.push_back(REMOVE_ITEM(SLOTTYPE_INVENTORY, m_Data.m_nEStoneItemSlotID, 0, pStone->GetAmount()));		

	if (1 == pAgent->GetAmount())
	{
		vecRemoveItem.push_back(REMOVE_ITEM(SLOTTYPE_INVENTORY, m_Data.m_nEAgentItemSlotID, 0, pStone->GetAmount()));		
	}
	else if (1 < pAgent->GetAmount())
	{
		vecRemoveItem.push_back(REMOVE_ITEM(SLOTTYPE_INVENTORY, m_Data.m_nEAgentItemSlotID, pStone->GetAmount(), 1));		
	}	
	gsys.pItemSystem->GetRemover().RemoveForDBTask(pPlayer->GetUID(), vecRemoveItem);

	// 귀속 처리
	pItem->m_bBind = true;

	if (m_Data.m_nTargetEnchItemID == ENCHANT_BROKEN_SLOT)
	{
		// 강화 실패
		{
			int nBuffID = pStone->m_pItemData->m_EnchantStone.nLinkedBuffID;
			GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
			VALID_RET(pBuffInfo, mdb::MDBTR_SUCESS);

			int8 nEnchantSlotID = pItem->EnchantBreak(pBuffInfo);
			if (nEnchantSlotID == -1)
				return mdb::MDBTR_SUCESS;

			bool bEquipSlot = (pItem->m_nSlotType == SLOTTYPE_EQUIP);
			int8 nItemSlotID = (int8)pItem->m_nSlotID;
			MCommand* pNewCommand = MakeNewCommand(MC_ENCHANT_BROKEN,
				3, NEW_BOOL(bEquipSlot), NEW_INT(nItemSlotID), NEW_CHAR(m_Data.m_nTargetEnchSlotID));
			pPlayer->RouteToMe(pNewCommand);
		}
	}
	else
	{
		// 강화 성공
		int nBuffID = pStone->m_pItemData->m_EnchantStone.nLinkedBuffID;
		GBuffInfo* pBuffInfo = gmgr.pBuffInfoMgr->Get(nBuffID);
		VALID_RET(pBuffInfo, mdb::MDBTR_SUCESS);
		ENCHANT_COLOR nEnchantColor = pStone->m_pItemData->m_EnchantStone.nColor;

		pItem->Enchant(pBuffInfo, pStone->m_pItemData->m_nID, nEnchantColor);

		pPlayer->OnItemEnchantSuccess(pItem, m_Data.m_nTargetEnchItemID);

		// 결과 패킷
		{
			bool bEquipSlot = (pItem->m_nSlotType == SLOTTYPE_EQUIP);
			int8 nItemSlotID = (int8)pItem->m_nSlotID;
			MCommand* pNewCommand = MakeNewCommand(MC_ENCHANT_SUCCESS,
				4, NEW_BOOL(bEquipSlot), NEW_INT(nItemSlotID), NEW_CHAR(m_Data.m_nTargetEnchSlotID), NEW_INT(m_Data.m_nTargetEnchItemID));
			pPlayer->RouteToMe(pNewCommand);
		}

		// 주변에게 룩 갱신 알림
		{
			MCommand* pNewCommand = MakeNewCommand(MC_ITEM_CHANGE_LOOK_EQUIP_ITEM,
				7, NEW_INT(-1), NEW_INT(-1), NEW_CHAR((char)pItem->m_nSlotID), NEW_UID(pPlayer->GetUID()), NEW_INT(pItem->m_pItemData->m_nID), NEW_INT(pItem->m_nDyedColor), NEW_INT(pItem->GetActiveEnchantBuffID()));

			pPlayer->RouteToThisCellExceptMe(pNewCommand);
		}
	}
	
	return mdb::MDBTR_SUCESS;
}

void GDBTaskItemEnchant::_OnPreCompleted()
{
	__super::_OnPreCompleted();
	m_SyncingList.EndCacheSync(IsSuccess());
}

bool GDBTaskItemEnchant::CharSync( const MUID& uidPlayer )
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(uidPlayer);
	if (NULL == cc)
		return false;
	else
	{
		if (!cc->BeginCacheSync())
		{
			m_SyncingList.EndCacheSync(false);
			return false;
		}

		if (NULL == m_SyncingList.PushChar())
		{
			m_SyncingList.EndCacheSync(false);
			return false;
		}
	}	

	return true;
}

bool GDBTaskItemEnchant::ItemSync( const MUID& uidPlayer, const SH_ITEM_SLOT_TYPE nSlotType, const int16 nSlotID, const IUID nIUID )
{
	GItemDBCache* ic = gsys.pDBCacheSystem->FindItemCache(uidPlayer, nSlotType, nSlotID, nIUID);
	if (NULL == ic)
	{
		m_SyncingList.EndCacheSync(false);
		return false;
	}
	else
	{
		if (!ic->BeginCacheSync())
		{
			m_SyncingList.EndCacheSync(false);
			return false;
		}

		if (NULL == m_SyncingList.PushItem(nIUID))
		{
			m_SyncingList.EndCacheSync(false);
			return false;
		}
	}	

	return true;
}