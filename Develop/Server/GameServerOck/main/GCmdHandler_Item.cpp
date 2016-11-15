#include "StdAfx.h"
#include "GCmdHandler_Item.h"
#include "CCommandTable.h"
#include "GItemSystem.h"
#include "GEntityPlayer.h"
#include "GPlayerObjectManager.h"
#include "GGlobal.h"
#include "GCommand.h"
#include "GItemUser.h"
#include "GItemEquiper.h"
#include "GItemUnequiper.h"
#include "GItemActableChecker.h"
#include "GItemHolder.h"
#include "CCommandResultTable.h"
#include "GItemRemover.h"
#include "GItemMover.h"
#include "GSystem.h"
#include "GItemSorter.h"
#include "GItemEnchant.h"
#include "GItemDyer.h"
#include "GPlayerDoing.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"

GCmdHandler_Item::GCmdHandler_Item(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_ITEM_EQUIP_REQ,			OnRequestEquip);
	SetCmdHandler(MC_ITEM_UNEQUIP_REQ,			OnRequestUnEquip);

	SetCmdHandler(MC_ITEM_DYE_PREPARE_REQ,		OnRequestDyePrepare);
	SetCmdHandler(MC_ITEM_DYE_PREPARECANCEL_REQ,OnRequestDyePrepareCancel);
	SetCmdHandler(MC_ITEM_DYE_REQ,				OnRequestDye);
	SetCmdHandler(MC_ITEM_USE_REQ,				OnRequestUse);
	SetCmdHandler(MC_ITEM_DROP_REQ,				OnRequestDrop);

	SetCmdHandler(MC_ITEM_SORT_INVEN_SLOT_REQ,	OnRequestSortInventorySlot);

	SetCmdHandler(MC_ITEM_MOVE_REQ,				OnRequestMove);

	SetCmdHandler(MC_ITEM_INTERACT_NPC_REQ,		OnRequsetItemInteractNPC);

	SetCmdHandler(MC_ENCHANT_REQ,				OnRequsetEnchant);
	SetCmdHandler(MC_ENCHANT_CHECK_REQ,			OnRequsetEnchantCheck);
	SetCmdHandler(MC_ENCHANT_PREPARE,			OnRequsetEnchantPrepare);
	SetCmdHandler(MC_ENCHANT_CANCEL,			OnRequsetEnchantCancel);
	
}

MCommandResult GCmdHandler_Item::OnRequestEquip(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nSlotTypeFrom;
	int nSlotIDFrom;
	int8 nSlotIDTo;

	if (pCommand->GetParameter(&nSlotTypeFrom,	0, MPT_INT)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nSlotIDFrom,	1, MPT_INT)==false) return CR_FALSE;
	if (pCommand->GetParameter(&nSlotIDTo,		2, MPT_CHAR)==false) return CR_FALSE;

	MUID uidPlayer = pCommand->GetSenderUID();
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (pPlayer == NULL) return CR_FALSE;
	GItem* pItem = pPlayer->GetItemHolder()->GetItem(static_cast<SH_ITEM_SLOT_TYPE>(nSlotTypeFrom), nSlotIDFrom);
	if (pItem == NULL) return CR_FALSE;

	gsys.pItemSystem->GetEquiper().Equip(pPlayer, pItem, (SH_ITEM_SLOT)nSlotIDTo);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequestUnEquip(MCommand* pCommand, MCommandHandler* pHandler)
{
	int8 nSlot;
	if (pCommand->GetParameter(&nSlot,	0, MPT_CHAR)==false) return CR_FALSE;

	MUID uidPlayer = pCommand->GetSenderUID();
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);
	if (pPlayer == NULL) return CR_FALSE;
	GItem* pItem = pPlayer->GetItemHolder()->GetItem(SLOTTYPE_EQUIP, nSlot);
	if (pItem == NULL) return CR_FALSE;

	gsys.pItemSystem->GetUnequiper().Unequip(pPlayer, pItem);	
	
	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequestDyePrepare(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nTargetSlotType;
	int nTargetSlotID;
	int nDyeSlotID;

	if (pCmd->GetParameter(&nTargetSlotType,	0, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nTargetSlotID,		1, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nDyeSlotID,			2, MPT_INT)==false) return CR_FALSE;

	gsys.pItemSystem->GetDyer().Prepare(pPlayer, static_cast<SH_ITEM_SLOT_TYPE>(nTargetSlotType), nTargetSlotID, nDyeSlotID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequestDyePrepareCancel(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	gsys.pItemSystem->GetDyer().PrepareCancel(pPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequestDye(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nTargetSlotType;
	int nTargetSlotID;
	int nDyeSlotID;

	if (pCmd->GetParameter(&nTargetSlotType,	0, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nTargetSlotID,		1, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nDyeSlotID,			2, MPT_INT)==false) return CR_FALSE;

	gsys.pItemSystem->GetDyer().Dye(pPlayer, static_cast<SH_ITEM_SLOT_TYPE>(nTargetSlotType), nTargetSlotID, nDyeSlotID);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequestUse(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	int nInvenSlotID;
	if (pCmd->GetParameter(&nInvenSlotID,	0, MPT_INT)==false) return CR_FALSE;

	if (false == gsys.pItemSystem->GetUser().Use(pPlayer, nInvenSlotID))
	{	
		return CR_FALSE;
	}

	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequestDrop(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	SH_ITEM_SLOT_TYPE nSlotType;
	int nSlotID;
	int nDropQuantity;

	if (pCmd->GetParameter(&nSlotType,		0, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nSlotID,		1, MPT_INT)==false) return CR_FALSE;
	if (pCmd->GetParameter(&nDropQuantity,	2, MPT_INT)==false) return CR_FALSE;
	
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	GItem* pItem = pItemHolder->GetItem(nSlotType, nSlotID);
	if (NULL == pItem) return CR_FALSE;
	if (false == pItem->m_pItemData->m_bDiscardable)
	{
		pPlayer->RouteSystemMsg(CR_FAIL_ITEM_NOT_REMOVABLE);
		return CR_FALSE;
	}
	// 인터랙션중일때만 보관함에서 아이템을 버릴수 있다.
	if (SLOTTYPE_STORAGE == nSlotType)
	{
		if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_STORAGE))
			return CR_FALSE;
	}
	// 길드 보관에서는 아이템을 버릴수 없다.
	if (SLOTTYPE_GUILDSTORAGE == nSlotType)
	{
		return CR_FALSE;
	}

	if (false == gsys.pItemSystem->GetActableChecker().IsDropable(pPlayer, pItem))	return CR_FALSE;

	gsys.pItemSystem->GetRemover().Remove(pPlayer, pItem->m_nSlotType, pItem->m_nSlotID, nDropQuantity);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequestSortInventorySlot(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	gsys.pItemSystem->GetSorter().Sort(pPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequestMove(MCommand* pCmd, MCommandHandler* pHandler)
{	
	SH_ITEM_SLOT_TYPE nFromSlotType;
	int nFromSlotID;
	int nFromItemID;
	SH_ITEM_SLOT_TYPE nToSlotType;
	int nToSlotID;
	int nToItemID;
	int nAmount;

	if (!pCmd->GetParameter(&nFromSlotType, 0, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&nFromSlotID,	1, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&nFromItemID,	2, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&nToSlotType,	3, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&nToSlotID,		4, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&nToItemID,		5, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&nAmount,		6, MPT_INT)) return CR_FALSE;

	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer)
		return CR_FALSE;

	// 인터랙션중일때만 보관함 아이템을 이동할수 있다.
	if (SLOTTYPE_STORAGE == nFromSlotType || SLOTTYPE_STORAGE == nToSlotType)
	{
		if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pEntityPlayer, IT_STORAGE))
			return CR_FALSE;
	}
	if (SLOTTYPE_GUILDSTORAGE == nFromSlotType || SLOTTYPE_GUILDSTORAGE == nToSlotType)
	{
		if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pEntityPlayer, IT_GUILD_STORAGE))
			return CR_FALSE;
	}

	if (!gsys.pItemSystem->GetMover().Move(pEntityPlayer, nFromSlotType, nFromSlotID, nFromItemID, nToSlotType, nToSlotID, nToItemID, nAmount))
		return CR_FALSE;

	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequsetItemInteractNPC(MCommand* pCmd, MCommandHandler* pHandler)
{	
	int nInvenSlotID;
	MUID uidNPC;

	if (!pCmd->GetParameter(&nInvenSlotID, 0, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&uidNPC, 1, MPT_UID)) return CR_FALSE;

	GEntityPlayer* pEntityPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pEntityPlayer)
		return CR_FALSE;

	if (!gsys.pItemSystem->GetUser().Use(pEntityPlayer, nInvenSlotID, uidNPC)) 
		return CR_FALSE;

	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequsetEnchant(MCommand* pCmd, MCommandHandler* pHandler)
{	
	int nTargetSlotType;
	int nTargetSlotID;
	int nEnchantStoneSlotID;
	int nEnchantAgentSlotID;

	if (!pCmd->GetParameter(&nTargetSlotType, 0, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&nTargetSlotID, 1, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&nEnchantStoneSlotID, 2, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&nEnchantAgentSlotID, 3, MPT_INT)) return CR_FALSE;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (!pPlayer)
		return CR_FALSE;

	gsys.pItemSystem->GetEnchant().Enchant(pPlayer, nEnchantStoneSlotID, (SH_ITEM_SLOT_TYPE)nTargetSlotType, nTargetSlotID, nEnchantAgentSlotID);
	pPlayer->GetDoing().EndDoing();
	
	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequsetEnchantCheck(MCommand* pCmd, MCommandHandler* pHandler)
{	
	int nTargetSlotType;
	int nTargetSlotID;
	int nEnchantStoneSlotID;
	int nEnchantAgentSlotID;

	if (!pCmd->GetParameter(&nTargetSlotType, 0, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&nTargetSlotID, 1, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&nEnchantStoneSlotID, 2, MPT_INT)) return CR_FALSE;
	if (!pCmd->GetParameter(&nEnchantAgentSlotID, 3, MPT_INT)) return CR_FALSE;

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (!pPlayer)
		return CR_FALSE;

	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	GItem* pEStone = pItemHolder->GetItem(SLOTTYPE_INVENTORY, nEnchantStoneSlotID);
	VALID_RET(pEStone, CR_FALSE);
	GItem* pEAgent = pItemHolder->GetItem(SLOTTYPE_INVENTORY, nEnchantAgentSlotID);
	VALID_RET(pEAgent, CR_FALSE);

	int nErrorCode =
		gsys.pItemSystem->GetEnchant().EnchantCheck(pPlayer, pEStone, (SH_ITEM_SLOT_TYPE)nTargetSlotType, nTargetSlotID, pEAgent);
		
	gsys.pItemSystem->GetEnchant().RouteEnchantCheckResult(pPlayer, nErrorCode);

	if (nErrorCode != CR_SUCCESS)
	{
		pPlayer->GetDoing().EndDoing();
	}
	
	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequsetEnchantPrepare(MCommand* pCmd, MCommandHandler* pHandler)
{	
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (!pPlayer)
		return CR_FALSE;

	gsys.pItemSystem->GetEnchant().EnchantPrepare(pPlayer);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Item::OnRequsetEnchantCancel(MCommand* pCmd, MCommandHandler* pHandler)
{	
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (!pPlayer)
		return CR_FALSE;

	gsys.pItemSystem->GetEnchant().EnchantCancel(pPlayer);

	return CR_TRUE;
}
