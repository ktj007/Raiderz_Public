#include "StdAfx.h"
#include "XCmdHandler_NPCShop.h"
#include "XPost_Interaction.h"
#include "XStrings.h"
#include "XMyPlayer.h"
#include "XModuleActorControl.h"
#include "XItemManager.h"
#include "XInteractionInfo.h"
#include "XSoundNonPlayer.h"

XCmdHandler_NPCShop::XCmdHandler_NPCShop(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_NPCSHOP_SHOW,		OnShow);
	SetCmdHandler(MC_NPCSHOP_BUY,		OnBuy);
	SetCmdHandler(MC_NPCSHOP_SELL,		OnSell);
	SetCmdHandler(MC_NPCSHOP_REPAIR,	OnRepair);
	SetCmdHandler(MC_NPCSHOP_SELL_VERYCOMMON,	OnSellVeryCommon);	
}

MCommandResult XCmdHandler_NPCShop::OnShow(MCommand* pCommand, MCommandHandler* pHandler)
{
	gvar.Game.NPCShopInfo.Clear();
	vector<int>& vecBuyableUsableItemID		= gvar.Game.NPCShopInfo.GetBuyableUsableItemList();
	vector<int>& vecBuyableUnusableItemID	= gvar.Game.NPCShopInfo.GetBuyableUnusableItemList();
	vector<int>& vecUnbuyableItemID			= gvar.Game.NPCShopInfo.GetUnbuyableItemList();
	float& fBuyMod	= gvar.Game.NPCShopInfo.GetBuyMode();
	float& fSellMod = gvar.Game.NPCShopInfo.GetSellMode();
	bool& bRepairer = gvar.Game.NPCShopInfo.GetRepairer();

	if (pCommand->GetBlob(vecBuyableUsableItemID,		0)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecBuyableUnusableItemID,		1)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecUnbuyableItemID,			2)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fBuyMod,				3, MPT_FLOAT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&fSellMod,				4, MPT_FLOAT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&bRepairer,				5, MPT_BOOL)==false) return CR_ERROR;

	vector<int>& vecNPCShopItem = gvar.Game.NPCShopInfo.GetItemList();
	for each (int nBuyableUsableItemID in vecBuyableUsableItemID)
	{
		vecNPCShopItem.push_back(nBuyableUsableItemID);
	}
	for each (int nBuyableUnusableItemID in vecBuyableUnusableItemID)
	{
		vecNPCShopItem.push_back(nBuyableUnusableItemID);
	}
	for each (int nUnbuyableItemID in vecUnbuyableItemID)
	{
		vecNPCShopItem.push_back(nUnbuyableItemID);
	}

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "SHOP", "BEGIN");

	XGetSoundNonPlayer()->PlayVoice( XGetInteractionInfo().InteractionTargetUID , "SHOP");

	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if (NULL == pModuleMyControl) return CR_FALSE;

	pModuleMyControl->DoActionInteraction(XGetInteractionInfo().InteractionTargetUID);

	return CR_TRUE;
}

MCommandResult XCmdHandler_NPCShop::OnBuy(MCommand* pCommand, MCommandHandler* pHandler)
{
	return CR_TRUE;
}

MCommandResult XCmdHandler_NPCShop::OnSell(MCommand* pCommand, MCommandHandler* pHandler)
{
	return CR_TRUE;
}

MCommandResult XCmdHandler_NPCShop::OnRepair(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_ITEM_SLOT_INFO> vecItemSlotInfo;
	if (pCommand->GetBlob(vecItemSlotInfo, 0)==false) return CR_ERROR;

	for each(TD_ITEM_SLOT_INFO tdSlotInfo in vecItemSlotInfo)
	{
		XItem* pItem = NULL;
		if (tdSlotInfo.nSlotType == SLOTTYPE_INVENTORY)
		{
			pItem = gvar.MyInfo.Inventory.GetItem(tdSlotInfo.nSlotID);
		}
		else
		{
			pItem = gvar.MyInfo.EquipmentSlot.GetItem((SH_ITEM_SLOT)tdSlotInfo.nSlotID);
		}

		if (pItem == NULL)
		{
			_ASSERT(0);
			continue;
		}

		global.ui->OnSystemMsg( XGetStr( L"ITEM_REPAIRED"), FSParam(pItem->m_pItemData->GetName()));
		//global.script->GetGlueGameEvent().OnNoticeDialogShow(szChat);
	}

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "SHOP", "REFRESH");

	return CR_TRUE;
}

MCommandResult XCmdHandler_NPCShop::OnSellVeryCommon(MCommand* pCommand, MCommandHandler* pHandler)
{
	global.ui->OnSystemMsg( XGetStr( L"ITEM_CLEARD"));

	return CR_TRUE;
}