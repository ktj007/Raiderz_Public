#include "stdafx.h"
#include "XCmdHandler_Item.h"
#include "XMyPlayer.h"
#include "XItem.h"
#include "XItemManager.h"
#include "XModuleEntity.h"
#include "XModuleMotion.h"
#include "XEventID.h"
#include "XPost_Interaction.h"
#include "XStrings.h"
#include "XTalentInfoMgr.h"
#include "CCommandResultTable.h"
#include "XItemHelper.h"
#include "XConditionHelper.h"

XCmdHandler_Item::XCmdHandler_Item(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_ITEM_EQUIP,				OnEquip);
	SetCmdHandler(MC_ITEM_EQUIP_SWAPTOINVEN,	OnEquipSwapToInven);

	SetCmdHandler(MC_ITEM_SORT_INVEN_SLOT,		OnSortInvenSlot);
	SetCmdHandler(MC_ITEM_UNEQUIP,				OnUnEquip);
	SetCmdHandler(MC_ITEM_TALENT,				OnItemTalent);
	
	SetCmdHandler(MC_ITEM_ADD,					OnAdd);
	SetCmdHandler(MC_ITEM_REMOVE,				OnRemove);

	SetCmdHandler(MC_ITEM_CHANGE_LOOK_EQUIP_ITEM,			OnChangeLookEquipItem);
	SetCmdHandler(MC_ITEM_CHANGE_LOOK_UNEQUIP_ITEM,			OnChangeLookUnEquipItem);

	SetCmdHandler(MC_ACTION_SWITCHING_WEAPON_BEGIN,			OnSwitchingWeaponSetBegin);
	SetCmdHandler(MC_ACTION_SWITCHING_WEAPON,				OnSwitchingWeaponSet);

	SetCmdHandler(MC_ITEM_DYE,					OnDye);
	SetCmdHandler(MC_ITEM_UPDATE_DURABILITY,	OnUpdateDurability);
	SetCmdHandler(MC_ITEM_MOVE,					OnMove);
	SetCmdHandler(MC_ITEM_USE_FAIL,				OnUseFail);
}

MCommandResult XCmdHandler_Item::OnEquip(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nSlotTypeFrom;
	int nSlotIDFrom;
	int8 nSlotIDTo;

	if (pCommand->GetParameter(&nSlotTypeFrom,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSlotIDFrom,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSlotIDTo,		2, MPT_CHAR)==false) return CR_ERROR;

	if (NULL == gvar.Game.pMyPlayer) return CR_ERROR;

	if (gvar.Game.pMyPlayer->EquipItem(static_cast<SH_ITEM_SLOT_TYPE>(nSlotTypeFrom), nSlotIDFrom, static_cast<SH_ITEM_SLOT>(nSlotIDTo)) == false) return CR_FALSE;

	gvar.MyInfo.EquipmentSlot.RefreshUI();
	global.ui->CraftUIRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Item::OnEquipSwapToInven(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nEquipSlotIDFrom;
	int8 nEquipSlotIDTo;
	int8 nUnequipSlotIDFrom;
	int nUnequipSlotIDTo;	

	if (pCommand->GetParameter(&nEquipSlotIDFrom,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nEquipSlotIDTo,		1, MPT_CHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nUnequipSlotIDFrom,	2, MPT_CHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nUnequipSlotIDTo,	3, MPT_INT)==false) return CR_ERROR;

	if (NULL == gvar.Game.pMyPlayer) return CR_ERROR;

	XItem* pEquipItem = gvar.MyInfo.Inventory.PopItem(nEquipSlotIDFrom);

	if (gvar.Game.pMyPlayer->UnEquipItem(SH_ITEM_SLOT(nUnequipSlotIDFrom), nUnequipSlotIDTo, true) == false) return CR_FALSE;
	if (gvar.Game.pMyPlayer->EquipItem(pEquipItem, static_cast<SH_ITEM_SLOT>(nEquipSlotIDTo)) == false) return CR_FALSE;

	gvar.MyInfo.EquipmentSlot.RefreshUI();
	global.ui->CraftUIRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Item::OnUnEquip(MCommand* pCommand, MCommandHandler* pHandler)
{	
	int nEquipSlotID = 0;
	int nInvenSlotID;

	if (pCommand->GetParameter(&nEquipSlotID,	0, MPT_CHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nInvenSlotID,	1, MPT_INT)==false) return CR_ERROR;
	
	if (NULL == gvar.Game.pMyPlayer) return CR_ERROR;	

	if (gvar.Game.pMyPlayer->UnEquipItem(SH_ITEM_SLOT(nEquipSlotID), nInvenSlotID) == false) return CR_FALSE;

	gvar.MyInfo.EquipmentSlot.RefreshUI();
	global.ui->CraftUIRefresh();

	return CR_TRUE;
}

// TODO : 소모성 아이템 테스트도 해보아야 한다.
MCommandResult XCmdHandler_Item::OnItemTalent(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nAmount=0;
	int nInvenSlotID;

	if (pCommand->GetParameter(&nAmount,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nInvenSlotID,	1, MPT_INT)==false) return CR_ERROR;

	XItem* pItem = gvar.MyInfo.Inventory.GetItem(nInvenSlotID);
	if (pItem == NULL) return CR_TRUE;

	gvar.MyInfo.Inventory.Use(nInvenSlotID, &gvar.MyInfo.ChrStatus, nAmount);

	gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionUseTalent(pItem->m_pItemData->m_vecUsableParam.front());

	return CR_TRUE;
}

MCommandResult XCmdHandler_Item::OnAdd(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	vector<TD_ITEM_INSERT> vecTDItemInsert;
	vector<TD_ITEM_INCREASE> vecTDItemIncrease;
	if (pCommand->GetBlob(vecTDItemInsert, 0)==false) return CR_ERROR;
	if (pCommand->GetBlob(vecTDItemIncrease, 1)==false) return CR_ERROR;

	XPlayer* pMyPlayer = static_cast<XPlayer*>(gg.omgr->Find(XGetMyUID()));
	if (pMyPlayer == NULL) return CR_FALSE;

	vector<int>	vecJustCraftableRecipeID;

	for each(const TD_ITEM_INSERT& tdItemInsert in vecTDItemInsert)
	{
		gvar.MyInfo.RecipeInfo.CollectJustCraftableRecipeID(vecJustCraftableRecipeID, tdItemInsert.m_nID, tdItemInsert.m_nAmount);

		XItem* pItem = gvar.MyInfo.Inventory.InsertItem(tdItemInsert);
		if (NULL == pItem) continue;	

		gvar.Game.SoundLootItem.PushBack(pItem);

		global.ui->OnSystemMsg( XGetStr( L"ITEM_GAIN"), FSParam(pItem->m_pItemData->GetName(), tdItemInsert.m_nAmount));

		if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "ITEM", "GAIN", tdItemInsert.m_nSlotID, tdItemInsert.m_nAmount);

		if(pItem->IsUsableItem() == true && pItem->IsQuestRelated() == true)
			global.ui->SetPaletteItem(tdItemInsert.m_nID);
	}

	for each(const TD_ITEM_INCREASE& tdItemIncrease in vecTDItemIncrease)
	{
		XItem* pItem = gvar.MyInfo.Inventory.GetItem(tdItemIncrease.m_nSlotID);
		if (NULL == pItem) continue;

		pItem->m_bNewItem = true;
		gvar.Game.SoundLootItem.PushBack(pItem);

		gvar.MyInfo.RecipeInfo.CollectJustCraftableRecipeID(vecJustCraftableRecipeID, pItem->m_pItemData->m_nID, tdItemIncrease.m_nAmount);
		gvar.MyInfo.Inventory.IncreaseItemAmount(tdItemIncrease.m_nSlotID, tdItemIncrease.m_nAmount);

		global.ui->OnSystemMsg( XGetStr( L"ITEM_GAIN"), FSParam(pItem->m_pItemData->GetName(), tdItemIncrease.m_nAmount));

		if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "ITEM", "GAIN", tdItemIncrease.m_nSlotID, tdItemIncrease.m_nAmount);
	}

	if (false == vecJustCraftableRecipeID.empty())
	{
		if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "CRAFTING", "COLLECTED", vecJustCraftableRecipeID.front(), gvar.MyInfo.RecipeInfo.GetCraftableRecipeCount());
	}	

	if (global.ui)
	{
		gvar.MyInfo.EquipmentSlot.RefreshUI();
		global.ui->InventoryUIRefresh();
		global.ui->CraftUIRefresh();
		global.ui->PaletteUIRefresh();
	}
	
	return CR_TRUE;
}

MCommandResult XCmdHandler_Item::OnRemove(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (gg.game == NULL) return CR_ERROR;

	vector<TD_ITEM_DELETE>		vecTDDeleteItem;
	vector<TD_ITEM_DECREASE>	vecTDDecreaseItem;
	
	if (pCommand->GetBlob(vecTDDeleteItem, 0) == false)	return CR_ERROR;
	if (pCommand->GetBlob(vecTDDecreaseItem, 1) == false)	return CR_ERROR;


	XPlayer* pMyPlayer = static_cast<XPlayer*>(gg.omgr->Find(XGetMyUID()));
	if (pMyPlayer == NULL) return CR_FALSE;


	for each(TD_ITEM_DELETE tdItemDelete in vecTDDeleteItem)
	{
		SH_ITEM_SLOT_TYPE nSlotType = (SH_ITEM_SLOT_TYPE)tdItemDelete.m_nSlotType;
		int nSlotID = tdItemDelete.m_nSlotID;

		XItem* pItem = XItemHelper::GetItem(nSlotType, nSlotID);
		if (NULL == pItem) continue;
		XItemData* pItemData = pItem->m_pItemData;
		if (NULL == pItemData) continue;			
		int nDeleteAmount = pItem->m_nAmount;

		if (!XItemHelper::PopItem(nSlotType, nSlotID))
			continue;

		global.ui->OnSystemMsg( XGetStr( L"ITEM_LOST"), FSParam(pItemData->GetName(), nDeleteAmount));

		if (global.script) global.script->GetGlueGameEvent().OnGameEvent( "ITEM", "LOSS", pItemData->m_nID, nDeleteAmount);

		if(pItem->IsUsableItem() == true && pItem->IsQuestRelated() == true)
			global.ui->RemovePaletteItem(pItemData->m_nID);
	}

	for each (TD_ITEM_DECREASE tdItemDecrease in vecTDDecreaseItem)
	{
		SH_ITEM_SLOT_TYPE nSlotType = (SH_ITEM_SLOT_TYPE)tdItemDecrease.m_nSlotType;
		int nSlotID = tdItemDecrease.m_nSlotID;

		XItem* pItem = XItemHelper::GetItem(nSlotType, nSlotID);
		if (NULL == pItem) continue;
		XItemData* pItemData = pItem->m_pItemData;
		if (NULL == pItemData) continue;

		if (!XItemHelper::DecreaseItemAmount(nSlotType, nSlotID, tdItemDecrease.m_nAmount))
			continue;

		global.ui->OnSystemMsg( XGetStr( L"ITEM_LOST"), FSParam(pItemData->GetName(), tdItemDecrease.m_nAmount));

		if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent( "ITEM", "LOSS", pItem->m_pItemData->m_nID, tdItemDecrease.m_nAmount);
	}	

	if (global.ui)
	{
		gvar.MyInfo.EquipmentSlot.RefreshUI();
		global.ui->InventoryUIRefresh();
		global.ui->CraftUIRefresh();
		global.ui->PaletteUIRefresh();
		global.script->GetGlueGameEvent().OnStorageRefresh();
	}
	
	return CR_TRUE;
}

MCommandResult XCmdHandler_Item::OnChangeLookEquipItem(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nSlotTypeFrom;
	int nSlotIDFrom;
	int8 nSlotIDTo;
	MUID uidPlayer;
	int nItemID;
	int nColor;
	int nEnchantBuff;

	if (pCommand->GetParameter(&nSlotTypeFrom,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSlotIDFrom,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSlotIDTo,		2, MPT_CHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidPlayer,		3, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nItemID,		4, MPT_INT)==false) return CR_ERROR;	
	if (pCommand->GetParameter(&nColor,			5, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nEnchantBuff,	6, MPT_INT)==false) return CR_ERROR;

	XNetPlayer* user = gg.omgr->FindNetPlayer(uidPlayer);
	if (user == NULL) return CR_FALSE;

	if(!user->IsModuleEntity()) return CR_FALSE;

	CSItemData* item_data = info.item->GetItemData(nItemID);

	wstring mesh_name = L"";
	if (item_data)
		mesh_name = static_cast<XItemData*>(item_data)->m_strMeshName;

	SH_FEATURE_ITEMSLOT nEquipFeatureItemSlot = XPlayerInfoFeature::TransItemSlotToNetSlot(SH_ITEM_SLOT(nSlotIDTo));
	SH_FEATURE_ITEMSLOT nUnEquipFeatureItemSlot = FEATURE_ITEMSLOT_NONE;
	
	if(nSlotIDFrom > -1 && static_cast<SH_ITEM_SLOT_TYPE>(nSlotTypeFrom) == SLOTTYPE_EQUIP)
		nUnEquipFeatureItemSlot = XPlayerInfoFeature::TransItemSlotToNetSlot(SH_ITEM_SLOT(nSlotIDFrom));

	user->EquipItem(nEquipFeatureItemSlot, item_data->m_nID, nUnEquipFeatureItemSlot, nColor, nEnchantBuff);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Item::OnChangeLookUnEquipItem(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidPlayer;
	int8 nSlot=0;
	if (pCommand->GetParameter(&uidPlayer,	0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSlot,		1, MPT_CHAR)==false) return CR_ERROR;

	XNetPlayer* user =	gg.omgr->FindNetPlayer(uidPlayer);
	if (user == NULL) return CR_FALSE;

	if(!user->IsModuleEntity()) return CR_FALSE;

	SH_FEATURE_ITEMSLOT nFeatureItemSlot = XPlayerInfoFeature::TransItemSlotToNetSlot(SH_ITEM_SLOT(nSlot));
	user->UnEquipItem(nFeatureItemSlot);

	return CR_TRUE;
}


MCommandResult XCmdHandler_Item::OnSwitchingWeaponSet(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nPlayerUIID;
	int8 nWeaponSet=0;
	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nWeaponSet,		1, MPT_CHAR)==false) return CR_ERROR;

	XPlayer* pPlayer = gg.omgr->FindPlayer_UIID(nPlayerUIID);
	if(pPlayer)
	{
		pPlayer->GetModuleActorControl()->OnSwitchingWeapon(nWeaponSet);
	}

	if (XGetMyUIID() == nPlayerUIID)
	{
		gvar.MyInfo.PaletteList.ChangeWeaponBindingPalette(nWeaponSet);

		global.ui->PaletteUIRefresh();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Item::OnSwitchingWeaponSetBegin(MCommand* pCommand, MCommandHandler* pHandler)
{
	UIID nPlayerUIID;

	if (pCommand->GetParameter(&nPlayerUIID,	0, MPT_USHORT)==false) return CR_ERROR;

	XPlayer* pPlayer = gg.omgr->FindPlayer_UIID(nPlayerUIID);
	if(pPlayer)
	{
		pPlayer->GetModuleActorControl()->OnSwitchingWeaponBegin();
	}

	return CR_TRUE;
}

MCommandResult XCmdHandler_Item::OnDye(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nSlotType;
	int nSlotID;
	int nColor;

	if (pCommand->GetParameter(&nSlotType,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSlotID,	1, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nColor,		2, MPT_INT)==false) return CR_ERROR;
	
	if(gvar.Game.pMyPlayer->OnDyeEquipItem(static_cast<SH_ITEM_SLOT_TYPE>(nSlotType), nSlotID, nColor) == false)
		return CR_ERROR;

	global.ui->OnSystemMsg( XGetStr( L"ITEM_DYED"));

	if (global.ui)
		global.ui->InventoryUIRefresh();

	XEvent msg;
	msg.m_nID = XEVTL_DYE_END_ITEM;
	gg.omgr->PostEvent(gvar.MyInfo.MyUID, msg);

	return CR_TRUE;
}


MCommandResult XCmdHandler_Item::OnUpdateDurability(MCommand* pCommand, MCommandHandler* pHandler)
{
	vector<TD_UPDATE_DURABILITY> vecTDUpdateDurability;

	if (pCommand->GetBlob(vecTDUpdateDurability, 0)==false) return CR_ERROR;

	bool bRepair = false;
	for each (TD_UPDATE_DURABILITY tdUpdateDurability in vecTDUpdateDurability)
	{
		XItem* pItem = NULL;

		if (tdUpdateDurability.m_nSlotType == SLOTTYPE_INVENTORY)
		{
			pItem = gvar.MyInfo.Inventory.GetItem(tdUpdateDurability.m_nSlotID);		
		}
		else if (tdUpdateDurability.m_nSlotType == SLOTTYPE_EQUIP)
		{
			pItem = gvar.MyInfo.EquipmentSlot.GetItem((SH_ITEM_SLOT)tdUpdateDurability.m_nSlotID);
		}

		if (NULL == pItem) continue;


		pItem->m_nDurability = tdUpdateDurability.m_nDurability;

		int nDurbility = pItem->m_pItemData->m_nMaxDurability *0.2;
		if (nDurbility > pItem->m_nDurability || pItem->m_nDurability == 0)
		{
			bRepair = true;
		}
	}

	if (bRepair)
	{
		global.ui->OnSystemMsg( XGetStr( L"ITEM_REQUIREDREPAIR"));
	}

	if (global.ui)
		global.ui->InventoryUIRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Item::OnChangeInvenSlot(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nSourceInvenSlotID = 0;
	int nTargetInvenSlotID = 0;

	if (pCommand->GetParameter(&nSourceInvenSlotID,	0, MPT_INT)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nTargetInvenSlotID,	1, MPT_INT)==false) return CR_ERROR;

	if (NULL == gvar.Game.pMyPlayer) return CR_ERROR;

	gvar.MyInfo.Inventory.ChangeSlot(nSourceInvenSlotID, nTargetInvenSlotID);

	if (global.ui) global.ui->InventoryUIRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Item::OnSortInvenSlot(MCommand* pCommand, MCommandHandler* pHandler)
{
	if (NULL == gvar.Game.pMyPlayer) return CR_ERROR;

	gvar.MyInfo.Inventory.SortSlot();

	if (global.ui) global.ui->InventoryUIRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Item::OnMove(MCommand* pCommand, MCommandHandler* pHandler)
{
	SH_ITEM_SLOT_TYPE nFromSlotType;
	int nFromSlotID;
	SH_ITEM_SLOT_TYPE nToSlotType;
	int nToSlotID;
	int nAmount;

	if (!pCommand->GetParameter(&nFromSlotType, 0, MPT_INT)) return CR_FALSE;
	if (!pCommand->GetParameter(&nFromSlotID,	1, MPT_INT)) return CR_FALSE;
	if (!pCommand->GetParameter(&nToSlotType,	2, MPT_INT)) return CR_FALSE;
	if (!pCommand->GetParameter(&nToSlotID,		3, MPT_INT)) return CR_FALSE;
	if (!pCommand->GetParameter(&nAmount,		4, MPT_INT)) return CR_FALSE;

	if (NULL == gvar.Game.pMyPlayer) return CR_ERROR;
	XItem* pFromItem  = XItemHelper::GetItem(nFromSlotType, nFromSlotID);
	if (NULL == pFromItem) return CR_ERROR;

	XItem* pToItem  = XItemHelper::GetItem(nToSlotType, nToSlotID);
	if (NULL == pToItem)
	{
		if (pFromItem->m_nAmount == nAmount)
		{
			XItemHelper::MoveItem(pFromItem, nFromSlotType, nToSlotType, nToSlotID, nAmount);
		}
		else
		{
			XItemHelper::SplitItem(pFromItem, nFromSlotType, nToSlotType, nToSlotID, nAmount);
		}		
	}
	else
	{
		if (pFromItem->GetID() ==  pToItem->GetID() && pToItem->m_nAmount < pToItem->m_pItemData->m_nStackAmount)
		{
			XItemHelper::MergeItem(pFromItem, nFromSlotType, pToItem, nToSlotType, nAmount);
		}
		else
		{
			XItemHelper::SwapItem(pFromItem, nFromSlotType, nFromSlotID, pToItem, nToSlotType, nToSlotID);
		}
	}

	if (global.ui) global.ui->InventoryUIRefresh();
	global.script->GetGlueGameEvent().OnStorageRefresh();
	global.script->GetGlueGameEvent().OnGuildStorageRefresh();

	return CR_TRUE;
}

MCommandResult XCmdHandler_Item::OnUseFail(MCommand* pCommand, MCommandHandler* pHandler)
{
	int nInvenSlotID;
	CONDITION_TYPE nConditionType;
	vector<int> vecParam1;
	vector<int> vecParam2;

	if (!pCommand->GetParameter(&nInvenSlotID,		0, MPT_INT)) return CR_FALSE;
	if (!pCommand->GetParameter(&nConditionType,	1, MPT_INT)) return CR_FALSE;
	if (!pCommand->GetBlob(vecParam1, 2)) return CR_FALSE;
	if (!pCommand->GetBlob(vecParam2, 3)) return CR_FALSE;

	//만약 부활아이템의 사용실패이면 부활창 다시띄워줘라.
	if (gvar.Game.pMyPlayer->IsDead())
	{
		gvar.MyInfo.fAutoRebirthRemainCoolTime
			= (GAME_AUTO_REBIRTH_CHECK_TICK - ((XGetNowTime() - gvar.MyInfo.nDieTime) / 1000.0f));

		gvar.Game.pMyPlayer->GetModuleMyControl()->OnDie();
	}
	else
	{
		gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionIdle();
	}


	{
		XItem* pItem  = XItemHelper::GetItem(SLOTTYPE_INVENTORY, nInvenSlotID);
		if (NULL == pItem) return CR_ERROR;

		CSCondition_Leaf Condition;
		Condition.m_nCT = nConditionType;
		Condition.m_vecParam1 = vecParam1;
		Condition.m_vecParam2 = vecParam2;

		wstring strText;
		XConditionHelper::CheckItemCondition(&Condition, strText);
		global.ui->OnSystemMsg( strText);
	}

	return CR_TRUE;
}