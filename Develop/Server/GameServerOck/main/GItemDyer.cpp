#include "StdAfx.h"
#include "GItemDyer.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"
#include "CCommandResultTable.h"
#include "GGlobal.h"
#include "GDBManager.h"
#include "GItemActableChecker.h"
#include "GConditionsSystem.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GItemRemover.h"
#include "GDBTaskItemData.h"
#include "GPlayerObjectManager.h"
#include "GPlayerDoing.h"

bool GItemDyer::Prepare( GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nDyeSlotID )
{
	VALID_RET(pPlayer, false);

	pPlayer->GetDoing().ChangeDoing(CD_DYEING);

	return true;
}

bool GItemDyer::PrepareCancel( GEntityPlayer* pPlayer )
{
	VALID_RET(pPlayer, false);

	pPlayer->GetDoing().EndDoing();

	return true;
}

bool GItemDyer::Dye( GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nDyeSlotID )
{
	VALID_RET(pPlayer, false);
	GItem* pTargetItem = pPlayer->GetItemHolder()->GetItem(nTargetSlotType, nTargetSlotID);
	VRSM_FAIL(pTargetItem, CR_FAIL_SYSTEM_NOT_EXIST_ITEM);
	GItem* pDyeItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, nDyeSlotID);
	VRSM_FAIL(pDyeItem, CR_FAIL_SYSTEM_NOT_EXIST_ITEM);

	if (false == Check(pPlayer, pTargetItem, pDyeItem)) return false;

	int nColor = GMath::RandomPickUp(pDyeItem->m_pItemData->m_vecDyeColor);

	GDBT_ITEM_DYE data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetCID(), pPlayer->GetPlayerInfo()->nPlayTimeSec,
		pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetMoney(),
		nDyeSlotID, pDyeItem->m_nIUID, pDyeItem->m_pItemData->m_nID,
		nTargetSlotType, nTargetSlotID, pTargetItem->m_nIUID, pTargetItem->m_pItemData->m_nID,
		nColor);

	if (false == gsys.pDBManager->ItemDye(data)) 
		return false;	

	return true;
}

bool GItemDyer::Check( GEntityPlayer* pPlayer, GItem* pTargetItem, GItem* pDyeItem)
{
	VALID_RET(pPlayer, false);
	VALID_RET(pTargetItem, false);
	VALID_RET(pDyeItem, false);

	if (false == pPlayer->GetDoing().IsNowDyeing()) return false;

	if (ITEMTYPE_DYE!= pDyeItem->m_pItemData->m_ItemType) return false;
	if (false == gsys.pConditionsSystem->CheckForItem(pPlayer, pDyeItem->m_pItemData->m_nConditionsInfoID))
	{
		return pPlayer->FailAndRouteSystemMsg(CR_FAIL_ITEM_NEED_CONDITION);
	}

	if (false == gsys.pItemSystem->GetActableChecker().IsDyeable(pPlayer, pTargetItem)) return false;
	if (false == pTargetItem->m_pItemData->m_bDyeable) return false;

	return true;
}

bool GItemDyer::DyeForGM( GEntityPlayer* pPlayer, SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nColor )
{
	VALID_RET(pPlayer, false);
	GItem* pTargetItem = pPlayer->GetItemHolder()->GetItem(nTargetSlotType, nTargetSlotID);
	VRSM_FAIL(pTargetItem, CR_FAIL_SYSTEM_NOT_EXIST_ITEM);

	GDBT_ITEM_DYE data(pPlayer->GetAID(), pPlayer->GetUID(), pPlayer->GetCID(), pPlayer->GetPlayerInfo()->nPlayTimeSec,
		pPlayer->GetPlayerInfo()->GetDeltaPlayTime(), pPlayer->GetLevel(), pPlayer->GetMoney(), 0, 0, 0,
		nTargetSlotType, nTargetSlotID, pTargetItem->m_nIUID, pTargetItem->m_pItemData->m_nID,
		nColor);

	DyeForDBTask(data);

	return true;
}

void GItemDyer::DyeForDBTask(GDBT_ITEM_DYE& data)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(data.m_uidPlayer);
	VALID(pPlayer);
	GItem* pTargetItem = pPlayer->GetItemHolder()->GetItem((SH_ITEM_SLOT_TYPE)data.m_nTargetItemSlotType, data.m_nTargetItemSlotID);
	VALID(pTargetItem);	

	pTargetItem->m_nDyedColor = data.m_nColor;	
	Route(pPlayer, pTargetItem, data.m_nColor);

	pPlayer->GetDoing().EndDoing();

	GItem* pDyeItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_INVENTORY, data.m_nDyeItemSlotID);
	if (NULL != pDyeItem)
	{
		REMOVE_ITEM removeItem(pDyeItem->m_nSlotType, pDyeItem->m_nSlotID, pDyeItem->GetAmount() -1, 1);

		gsys.pItemSystem->GetRemover().RemoveForDBTask(pPlayer->GetUID(), removeItem);
	}
}

void GItemDyer::Route( GEntityPlayer* pPlayer, GItem* pTargetItem, int nColor)
{
	VALID(pPlayer);
	VALID(pTargetItem);

	MCommand* pNewCmd = MakeNewCommand(MC_ITEM_DYE, 
		3, 
		NEW_INT(pTargetItem->m_nSlotType), 
		NEW_INT(pTargetItem->m_nSlotID), 
		NEW_INT(pTargetItem->m_nDyedColor));

	pPlayer->RouteToMe(pNewCmd);

	if (true == pTargetItem->IsEquiped())
	{
		MCommand* pNewCmd = MakeNewCommand(MC_ITEM_CHANGE_LOOK_EQUIP_ITEM,
			7, NEW_INT(-1), NEW_INT(-1), NEW_CHAR(static_cast<uint8>(pTargetItem->m_nSlotID)), NEW_UID(pPlayer->GetUID()), NEW_INT(pTargetItem->m_pItemData->m_nID), NEW_INT(nColor), NEW_INT(pTargetItem->GetActiveEnchantBuffID()));

		pPlayer->RouteToThisCellExceptMe(pNewCmd);
	}
}