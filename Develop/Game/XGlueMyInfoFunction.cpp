#include "stdafx.h"
#include "XGlueMyInfoFunction.h"
#include "XMyPlayer.h"
#include "XModuleActorControl.h"
#include "XModulePost.h"
#include "XGame.h"
#include "XWorld.h"
#include "XStrings.h"
#include "XController.h"
#include "XItemManager.h"
#include "XGuildMember.h"
#include "XPost_Item.h"
#include "XPost_Palette.h"
#include "XTalentInfoMgr.h"
#include "XPlayerFaction.h"
#include "CSItemHelper.h"

void XGlueMyInfoFunction::DeclGlueFunc( MWLua* pLua )
{
	MWLua::class_<XGlueMyInfoFunction>(pLua->GetState(), "_shMycharacter")
		// 캐릭터 정보
		.def("GetUID",			&XGlueMyInfoFunction::GetUID)	
		.def("GetID",			&XGlueMyInfoFunction::GetID)	
		.def("GetSurName",		&XGlueMyInfoFunction::GetSurName)
		.def("GetLevel",		&XGlueMyInfoFunction::GetLevel)	
		.def("GetSilver",		&XGlueMyInfoFunction::GetSilver)	
		.def("GetExpPercent",	&XGlueMyInfoFunction::GetExpPercent)
		.def("GetExp",			&XGlueMyInfoFunction::GetExp)
		.def("GetFieldName",	&XGlueMyInfoFunction::GetFieldName)
		.def("GetFieldID",		&XGlueMyInfoFunction::GetFieldID)
		.def("GetChannelID",	&XGlueMyInfoFunction::GetChannelID)
		.def("IsDynamicField",	&XGlueMyInfoFunction::IsDynamicField)
		.def("GetPlayerGrade",	&XGlueMyInfoFunction::GetPlayerGrade)	


		// 능력 정보
		.def("GetSTR", 			&XGlueMyInfoFunction::GetSTR)	
		.def("GetDEX", 			&XGlueMyInfoFunction::GetDEX)	
		.def("GetINT", 			&XGlueMyInfoFunction::GetINT)
		.def("GetCON", 			&XGlueMyInfoFunction::GetCON)
		.def("GetCHA", 			&XGlueMyInfoFunction::GetCHA)	

		// MOD
		.def("GetModSTR", 		&XGlueMyInfoFunction::GetModSTR)	
		.def("GetModDEX", 		&XGlueMyInfoFunction::GetModDEX)	
		.def("GetModINT", 		&XGlueMyInfoFunction::GetModINT)
		.def("GetModCON", 		&XGlueMyInfoFunction::GetModCON)
		.def("GetModCHA", 		&XGlueMyInfoFunction::GetModCHA)	

		// 저항 정보
		.def("GetFR", 			&XGlueMyInfoFunction::GetFR)	
		.def("GetCR", 			&XGlueMyInfoFunction::GetCR)	
		.def("GetLR", 			&XGlueMyInfoFunction::GetLR)	
		.def("GetPR", 			&XGlueMyInfoFunction::GetPR)	
		.def("GetHR", 			&XGlueMyInfoFunction::GetUR)	
		.def("GetUR", 			&XGlueMyInfoFunction::GetUR)	

		.def("GetModFR",		&XGlueMyInfoFunction::GetModFR)	
		.def("GetModCR",		&XGlueMyInfoFunction::GetModCR)	
		.def("GetModLR",		&XGlueMyInfoFunction::GetModLR)	
		.def("GetModPR",		&XGlueMyInfoFunction::GetModPR)	
		.def("GetModHR",		&XGlueMyInfoFunction::GetModHR)	
		.def("GetModUR",		&XGlueMyInfoFunction::GetModUR)	


		// 상태 정보
		.def("GetHP",			&XGlueMyInfoFunction::GetHP)	
		.def("GetEN",			&XGlueMyInfoFunction::GetEN)	
		.def("GetSTA",			&XGlueMyInfoFunction::GetSTA)
		.def("GetMaxHP",		&XGlueMyInfoFunction::GetMaxHP)	
		.def("GetMaxEN",		&XGlueMyInfoFunction::GetMaxEN)	
		.def("GetMaxSTA",		&XGlueMyInfoFunction::GetMaxSTA)	
		.def("GetFocusType",	&XGlueMyInfoFunction::GetFocusType)	
		.def("GetFocusPercent",	&XGlueMyInfoFunction::GetFocusPercent)		

		// 인벤 정보
		.def("GetItemName",				&XGlueMyInfoFunction::GetItemName)
		.def("GetInventoryItemCount",	&XGlueMyInfoFunction::GetInventoryItemCount)
		.def("GetInventoryItemIDCount",	&XGlueMyInfoFunction::GetInventoryItemIDCount)

		.def("GetInventoryUIDItemCount",&XGlueMyInfoFunction::GetInventoryUIDItemCount)
		.def("GetInventoryItemName",	&XGlueMyInfoFunction::GetInventoryItemName)
		.def("GetInventoryItemType",	&XGlueMyInfoFunction::GetInventoryItemType)
		.def("GetInventoryItemID",		&XGlueMyInfoFunction::GetInventoryItemID)

		// 장비착용 정보
		.def("GetEquippedItem",		&XGlueMyInfoFunction::GetEquippedItem)
		.def("GetEquippedItemUID",	&XGlueMyInfoFunction::GetEquippedItemUID)
		.def("GetEquippedItemID",	&XGlueMyInfoFunction::GetEquippedItemID)		
		.def("SetEquipItem",		&XGlueMyInfoFunction::SetEquipItem)
		.def("SetUnEquipItem",		&XGlueMyInfoFunction::SetUnEquipItem)
		.def("GetEquippedItemName",	&XGlueMyInfoFunction::GetEquippedItemName)
		
		// 무기착용 정보
		.def("GetWeaponType",	&XGlueMyInfoFunction::GetWeaponType)
		
		// 탤런트 정보
		.def("GetTP",								&XGlueMyInfoFunction::GetTP)
		.def("GetLearnedTalentList",				&XGlueMyInfoFunction::GetLearnedTalentList)
		.def("IsLearnedTalent",						&XGlueMyInfoFunction::IsLearnedTalent)
		.def("GetTalentCooldownRemainPercent",		&XGlueMyInfoFunction::GetTalentCooldownRemainPercent)
		.def("GetStyleUsedTalent",					&XGlueMyInfoFunction::GetStyleUsedTalent)		
		
		// 아이템 정보
		.def("IsQuestItem",				&XGlueMyInfoFunction::IsQuestItem)		

		// 걸린 탤런트

		// 무기 사용에 대한 처리

		// 거래
		.def("GetMyTradingContainerSize",		&XGlueMyInfoFunction::GetMyTradingContainerSize)
		.def("GetMyTradingContainerItemUID",	&XGlueMyInfoFunction::GetMyTradingContainerItemUID)
		.def("GetMyTradingSilver",				&XGlueMyInfoFunction::GetMyTradingSilver)

		.def("GetOtherTradingContainerSize",	&XGlueMyInfoFunction::GetOtherTradingContainerSize)
		.def("GetOtherTradingContainerItemUID",	&XGlueMyInfoFunction::GetOtherTradingContainerItemUID)
		.def("GetOtherTradingContainerItemName",&XGlueMyInfoFunction::GetOtherTradingContainerItemName)
		.def("GetOtherTradingSilver",				&XGlueMyInfoFunction::GetOtherTradingSilver)

		// 팔레트
		.def("AddPaletteTalent",		&XGlueMyInfoFunction::AddPaletteTalent)
		.def("AddPaletteItem",			&XGlueMyInfoFunction::AddPaletteItem)
		//.def("GetPaletteList",			&XGlueMyInfoFunction::GetPaletteList)
		.def("GetPaletteListIndex",		&XGlueMyInfoFunction::GetPaletteListIndex)
		.def("UsePaletteIndex",			&XGlueMyInfoFunction::UsePaletteIndex)
		.def("TakeAwayPalette",			&XGlueMyInfoFunction::TakeAwayPalette)
		.def("ChangePalette",			&XGlueMyInfoFunction::ChangePalette)
		.def("ChangePaletteList",		&XGlueMyInfoFunction::ChangePaletteList)
		.def("WeaponBinding",			&XGlueMyInfoFunction::WeaponBinding)
		
		.def("PrimaryBinding",			&XGlueMyInfoFunction::PrimaryBinding)
		.def("SecondaryBinding",		&XGlueMyInfoFunction::SecondaryBinding)

		
		//Buff
		//.def("GetBuffList",				&XGlueMyInfoFunction::GetBuffList)

		//EnemyInfo
		.def("GetEnemyInfoTable",		&XGlueMyInfoFunction::GetEnemyInfoTable)		

		//파티정보
		//.def("GetPartyMemberBuffIDList", &XGlueMyInfoFunction::GetPartyMemberBuffIDList)

		//Quest
		.def("GetQuestIDList",			&XGlueMyInfoFunction::GetQuestIDList)
		.def("GetQuestProgress",		&XGlueMyInfoFunction::GetQuestProgress)
		.def("GetQuestRemainTime",		&XGlueMyInfoFunction::GetQuestRemainTime)
		.def("IsQuestFail",				&XGlueMyInfoFunction::IsQuestFail)
		.def("IsQuestTypeTimeAttack",	&XGlueMyInfoFunction::IsQuestTypeTimeAttack)

		.def("GetFieldMyPosX",			&XGlueMyInfoFunction::GetFieldMyPosX)
		.def("GetFieldMyPosY",			&XGlueMyInfoFunction::GetFieldMyPosY)

		//수리창
		.def("GetInvenEquipItemDurability",	&XGlueMyInfoFunction::GetInvenEquipItemDurability)
		.def("GetInvenEquipItemID",			&XGlueMyInfoFunction::GetInvenEquipItemID)
		.def("GetRepairableItemCount",		&XGlueMyInfoFunction::GetRepairableItemCount)
		.def("GetTotalRepairPrice",			&XGlueMyInfoFunction::GetTotalRepairPrice)
		
		//길드
		.def("GetGuildMemberList",		&XGlueMyInfoFunction::GetGuildMemberList)
		.def("GetGuildMemberGrade",		&XGlueMyInfoFunction::GetGuildMemberGrade)
		.def("GetGuildMemberFieldName",	&XGlueMyInfoFunction::GetGuildMemberFieldName)
		.def("GetGuildMemberChannelID",	&XGlueMyInfoFunction::GetGuildMemberChannelID)
		.def("IsGuildMemberOnline",		&XGlueMyInfoFunction::IsGuildMemberOnline)
		.def("GetGuildName",			&XGlueMyInfoFunction::GetGuildName)		
		.def("IsGuildMaster",			&XGlueMyInfoFunction::IsGuildMaster)		
		
		//Faction
		.def("GetFactionInfoTable",		&XGlueMyInfoFunction::GetFactionInfoTable)		
		
		// 기타
		;
}

const char* XGlueMyInfoFunction::GetItemName(unsigned int nItemID_High, unsigned int nItemID_Low)
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//return gvar.MyInfo.Inventory.GetItem(MUID(nItemID_High, nItemID_Low))->m_pItemData->m_strName.c_str();
	return "NotUsedFunCall";
}

const char* XGlueMyInfoFunction::GetInventoryItemName(const char* szuid )
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03
	
	//if( szuid == NULL) return NULL;

	//MUID uidItem = _atoi64(szuid);

	//XItem* pItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if( pItem == NULL) return NULL;

	//return pItem->m_pItemData->m_strName.c_str();
	return "NotUsedFunCall";
}

int XGlueMyInfoFunction::GetInventoryItemCount()
{
	return gvar.MyInfo.Inventory.GetSize();
}

int XGlueMyInfoFunction::GetInventoryItemIDCount(int nItemID)
{
	return gvar.MyInfo.Inventory.GetItemAmountByID(nItemID);
}

const char* XGlueMyInfoFunction::GetInventoryItemType( const char* szuid)
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return XStrings::ItemTypeToString(ITEMTYPE_NONE);

	//MUID uidItem = _atoi64(szuid);

	//XItem* pItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if( pItem == NULL) return XStrings::ItemTypeToString(ITEMTYPE_NONE);

	//if ((int)pItem->m_pItemData->m_ItemType < 0) return XStrings::ItemTypeToString(ITEMTYPE_NONE);

	//return XStrings::ItemTypeToString(pItem->m_pItemData->m_ItemType);

	return "NotUsedFunCall";
}

int XGlueMyInfoFunction::GetInventoryUIDItemCount(const char* szuid)
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return 0;

	//MUID uidItem = _atoi64(szuid);

	//XItem* pItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if( pItem == NULL) return 0;

	//return (int)pItem->m_nAmount;

	return 0;
}

int XGlueMyInfoFunction::GetInventoryItemID( const char* szuid )
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return NULL;

	//MUID uidItem = _atoi64(szuid);

	//XItem* pItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if( pItem == NULL) return NULL;

	//return pItem->m_pItemData->m_nID;

	return 0;
}

int XGlueMyInfoFunction::GetMyTradingContainerSize()
{
	return (int)gvar.MyInfo.TradeContents.m_MyTradeContainer.GetSize();
}

__int64 XGlueMyInfoFunction::GetMyTradingContainerItemUID(int index )
{
	// ! XItem::m_uidID가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( index < 0 || index >= (int)gvar.MyInfo.TradeContents.m_MyTradeContainer.GetSize()) return 0;

	//XItem* pItem = gvar.MyInfo.TradeContents.m_MyTradeContainer.GetItem(index);
	//if( pItem == NULL)
	//	return 0;

	//return pItem->m_uidID.Value;

	return MUID::ZERO.Value;
}

int XGlueMyInfoFunction::GetOtherTradingContainerSize()
{
	return (int)gvar.MyInfo.TradeContents.m_OtherTradeContainer.GetSize();
}

__int64 XGlueMyInfoFunction::GetOtherTradingContainerItemUID(int index )
{
	// ! XItem::m_uidID가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( index < 0 || index >= (int)gvar.MyInfo.TradeContents.m_OtherTradeContainer.GetSize()) return 0;

	//XItem* pItem = gvar.MyInfo.TradeContents.m_OtherTradeContainer.GetItem(index);
	//if( pItem == NULL)
	//	return 0;

	//return pItem->m_uidID.Value;

	return MUID::ZERO.Value;
}

const char* XGlueMyInfoFunction::GetOtherTradingContainerItemName( const char* szuid)
{	
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return NULL;

	//MUID uidItem = _atoi64(szuid);

	//XItem* pItem = gvar.MyInfo.TradeContents.m_OtherTradeContainer.GetItem(uidItem);
	//if( pItem == NULL) return NULL;

	//XItemData* pItemData = info.item->GetItemData(pItem->m_pItemData->m_nID);
	//if( pItemData == NULL) return NULL;

	//return pItemData->m_strName.c_str();	

	return "NotUsedFunCall";
}

int XGlueMyInfoFunction::GetMyTradingSilver()
{
	return (int)gvar.MyInfo.TradeContents.m_nMySilver;
}

int XGlueMyInfoFunction::GetOtherTradingSilver()
{
	return (int)gvar.MyInfo.TradeContents.m_nOtherSilver;
}

int XGlueMyInfoFunction::GetEquippedItemID( const char* szSlot )
{
	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem(MLocale::ConvAnsiToUTF16(szSlot).c_str());
	if( pItem == NULL) return 0;

	return pItem->m_pItemData->m_nID;
}

__int64 XGlueMyInfoFunction::GetEquippedItemUID( const char* szSlot )
{
	// ! XItem::m_uidID가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem(szSlot);
	//if( pItem == NULL) return 0;

	//return pItem->m_uidID.Value;
	
	return MUID::ZERO.Value;
}

const char* XGlueMyInfoFunction::GetEquippedItemName( const char* szuid )
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return NULL;
	//MUID uidItem = _atoi64(szuid);


	//XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItemByMUID(uidItem);
	//if (pItem == NULL) return NULL;

	//return pItem->m_pItemData->m_strName.c_str();
	return "NotUsedFunCall";
}

const char* XGlueMyInfoFunction::GetEquippedItem( const char* szSlot )
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gvar.MyInfo.EquipmentSlot.GetItemName(MLocale::ConvAnsiToUTF16(szSlot).c_str()));

	return gvar.Temp.szTemp.c_str();
}

void XGlueMyInfoFunction::SetEquipItem( const char* szSlot, const char* szuid, bool bPass /*= false */ )
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return;
	//if( szSlot == NULL) return;

	//MUID uidItem = _atoi64(szuid);

	//XItem* pItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if( pItem ==NULL) return;
	//CSItemData* pItemData = pItem->m_pItemData;
	//if( pItemData == NULL) return;

	////착용시 귀속 아이템을 첨 착용하는것이라면
	//if (pItemData->m_bClaimRequired && (pItem->m_bClaimed == false) && bPass == false)
	//{
	//	global.script->GetGlueGameEvent().OnEquipBind(pItemData->m_strName.c_str(), szSlot, szuid);		
	//	return;
	//}

	//// 무기류
	//// for문이 ITEMSLOT_MAX까지 도는 건 문제가 있는데...
	//for (int i=ITEMSLOT_LWEAPON; i < (int)ITEMSLOT_MAX; i++)
	//{
	//	if (_stricmp(szSlot, CSItemSlotStr[i]) == 0)
	//	{
	//		// 지금 사용하는 무기와 교체하려는 무기 슬롯이 같은지 확인하고, 같으면 스탠스를 바꿔주기 위해서 옮김
	//		if ( pItemData->m_nWeaponType != WEAPON_NONE)
	//		{
	//			gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionEquipItem( (SH_ITEM_SLOT)i, uidItem);
	//		}
	//		else
	//		{
	//			gvar.Game.pMyPlayer->GetModulePost()->PostEquipItem( (SH_ITEM_SLOT)i, uidItem);
	//		}

	//		return;
	//	}
	//}

	//// 장비류
	//SH_ITEM_SLOT item_slot = pItemData->m_nItemSlot;
	//int nInvenSlotID = pItem->m_nSlotID;
	//XPostItem_EquipItem(nInvenSlotID, item_slot);
}

void XGlueMyInfoFunction::SetUnEquipItem( const char* szSlot)
{
	if( szSlot == NULL) return;

	if(XGetMyPlayer()->IsUsableBreakablePartsWeapon())
		return;

	for (int i=0; i<(int)ITEMSLOT_MAX; i++)
	{
		if (_wcsicmp(MLocale::ConvAnsiToUTF16(szSlot).c_str(), CSItemSlotStr[i]) == 0)
		{
			XPostItem_Unequip((SH_ITEM_SLOT)i);			
			break;
		}
	}
}

bool XGlueMyInfoFunction::IsDynamicField()
{
	if(gg.currentgamestate && gg.currentgamestate->GetWorld() && gg.currentgamestate->GetWorld()->GetInfo())
	{
		CSFieldFileInfo* pFieldInfo = info.field->GetFieldList().Find(gg.currentgamestate->GetWorld()->GetInfo()->m_nFieldID);
		if (pFieldInfo) return pFieldInfo->bDynamic;
	}

	return false;
}

int XGlueMyInfoFunction::GetChannelID()
{
	return gvar.World.nChannelID;
}

int XGlueMyInfoFunction::GetFieldID()
{
	if(gg.currentgamestate && gg.currentgamestate->GetWorld() && gg.currentgamestate->GetWorld()->GetInfo())
	{
		return gg.currentgamestate->GetWorld()->GetInfo()->m_nFieldID;
	}

	return 0;
}

const char* XGlueMyInfoFunction::GetFieldName()
{
	if(gg.currentgamestate && gg.currentgamestate->GetWorld() && gg.currentgamestate->GetWorld()->GetInfo())
	{
		gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gg.currentgamestate->GetWorld()->GetInfo()->GetName().c_str());
		return gvar.Temp.szTemp.c_str();
	}

	return "맵로딩중";
}

const char* XGlueMyInfoFunction::GetLearnedTalentList()
{
	char text[256]="";
	gvar.Temp.szTemp = "";

	vector<XTalentInfo*> qTestDisplayItems;
	gvar.MyInfo.Talent.GetHighestRankListOfTalent(qTestDisplayItems);

	for (size_t i=0; i<qTestDisplayItems.size(); i++)
	{
		XTalentInfo* pTalentInfo = qTestDisplayItems[i];

		sprintf_s(text, 256, "%d", pTalentInfo->m_nID);

		if( i != 0 )	gvar.Temp.szTemp += ",";

		gvar.Temp.szTemp += text;
	}

	return gvar.Temp.szTemp.c_str();
}

bool XGlueMyInfoFunction::IsLearnedTalent(int nTalentID)
{
	if( nTalentID == 0 ) return false;

	return gvar.MyInfo.Talent.IsLearnedTalent(nTalentID);
}

//const char* XGlueMyInfoFunction::GetBuffList()
//{
//	wstring strItemList;
//	gvar.MyInfo.BuffList.GetItemIDList(strItemList);
//
//	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strItemList.c_str());
//	return gvar.Temp.szTemp.c_str();
//}

MWLua::table XGlueMyInfoFunction::GetEnemyInfoTable()
{
	return NULL;//gvar.Game.EnemyInfo.GetEnemyInfoTable(0);

	//MWLua::table t(global.script->GetLua()->GetState());

	//char buff[128]="";
	//_itoa_s(1, buff, 10);
	//t.set(buff, gvar.Game.EnemyInfo.GetEnemyInfoTable(0));

	//return t;
}

//const char* XGlueMyInfoFunction::GetPartyMemberBuffIDList(const char* szuid)
//{
//	if( szuid == NULL) return "";
//	MUID uid = _atoi64(szuid);
//
//	gvar.MyInfo.Party.GetPartyMemberBuffIDList(uid, gvar.Temp.szTemp);
//
//	return gvar.Temp.szTemp.c_str();
//}

const char* XGlueMyInfoFunction::GetQuestIDList()
{
	wstring strQuestIDList;
	gvar.MyInfo.Quests.GetQuestIDList(strQuestIDList);

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strQuestIDList.c_str());

	return gvar.Temp.szTemp.c_str();
}

int XGlueMyInfoFunction::GetQuestProgress(int nQuestID, int nObjID )
{
	XPlayerQuest* pPlayerQuest = gvar.MyInfo.Quests.Get(nQuestID);
	if (pPlayerQuest == NULL) return 0;

	XPlayerQObjective* pPlayerQObjective = pPlayerQuest->Get(nObjID);
	if (pPlayerQObjective == NULL) return 0;

	return pPlayerQObjective->nProgress;
}

bool XGlueMyInfoFunction::IsQuestTypeTimeAttack( int nQuestID )
{
	XPlayerQuest* pPlayerQuest = gvar.MyInfo.Quests.Get(nQuestID);
	if (pPlayerQuest == NULL) return false;

	return pPlayerQuest->IsTimeAttackType();
}

bool XGlueMyInfoFunction::IsQuestFail( int nQuestID )
{
	XPlayerQuest* pPlayerQuest = gvar.MyInfo.Quests.Get(nQuestID);
	if (pPlayerQuest == NULL) return false;

	return (QST_FAIL == pPlayerQuest->m_nState);
}

unsigned int XGlueMyInfoFunction::GetQuestRemainTime( int nQuestID )
{
	XPlayerQuest* pPlayerQuest = gvar.MyInfo.Quests.Get(nQuestID);
	if (pPlayerQuest == NULL) return 0;

	return pPlayerQuest->GetRemainTime();
}

int XGlueMyInfoFunction::GetTalentCooldownRemainPercent( int nTalentID )
{
	if ( nTalentID == 0)
		return 0;

	float fRate = gvar.MyInfo.Talent.CoolTime.GetRemainedTimeRatio( nTalentID);
	int nPercent = (int)(fRate * 100);
	return nPercent;
}

float XGlueMyInfoFunction::GetFieldMyPosX()
{
	if (gvar.Game.pMyPlayer)
	{
		return gvar.Game.pMyPlayer->GetPosition().x;
	}

	return 0.f;
}

float XGlueMyInfoFunction::GetFieldMyPosY()
{
	if (gvar.Game.pMyPlayer)
	{
		return gvar.Game.pMyPlayer->GetPosition().y;
	}

	return 0.f;
}

///< 수리창
int XGlueMyInfoFunction::GetInvenEquipItemDurability( const char* szuid)
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return 0;

	//MUID uidItem = _atoi64(szuid);

	//XItem* pItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if( pItem == NULL)
	//{
	//	pItem = gvar.MyInfo.EquipmentSlot.GetItemByMUID(uidItem);
	//	if (pItem == NULL)
	//	{
	//		return 0;
	//	}
	//}

	//return pItem->m_nDurability;
	return 0;
}

int XGlueMyInfoFunction::GetInvenEquipItemID( const char* szuid)
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return 0;

	//MUID uidItem = _atoi64(szuid);

	//XItem* pItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if( pItem == NULL)
	//{
	//	pItem = gvar.MyInfo.EquipmentSlot.GetItemByMUID(uidItem);
	//	if (pItem == NULL)
	//	{
	//		return 0;
	//	}
	//}

	//return pItem->m_pItemData->m_nID;
	return 0;
}

int XGlueMyInfoFunction::GetRepairableItemCount()
{
	return 0;//gvar.MyInfo.RepairableItemList.GetItemCount();
}

int XGlueMyInfoFunction::GetTotalRepairPrice()
{
	return 0;//gvar.MyInfo.RepairableItemList.GetTotalPrice();
}

const char* XGlueMyInfoFunction::GetGuildMemberList()
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gvar.MyInfo.Guild.GetMemberList().c_str());
	return gvar.Temp.szTemp.c_str();
}

int XGlueMyInfoFunction::GetGuildMemberGrade( const char* szName )
{
	XGuildMember* pMember = gvar.MyInfo.Guild.GetMember(MLocale::ConvAnsiToUTF16(szName).c_str());
	if (pMember == NULL) return 0;

	return (int)pMember->m_nGrade;
}

const char* XGlueMyInfoFunction::GetGuildMemberFieldName( const char* szName )
{
	XGuildMember* pMember = gvar.MyInfo.Guild.GetMember(MLocale::ConvAnsiToUTF16(szName).c_str());
	if (pMember == NULL) return 0;

	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(info.field->GetFieldName(pMember->m_nFieldID));
	return gvar.Temp.szTemp.c_str();
}

int XGlueMyInfoFunction::GetGuildMemberChannelID( const char* szName )
{
	XGuildMember* pMember = gvar.MyInfo.Guild.GetMember(MLocale::ConvAnsiToUTF16(szName).c_str());
	if (pMember == NULL) return 0;

	return pMember->m_nChannelID;
}

bool XGlueMyInfoFunction::IsGuildMemberOnline( const char* szName )
{
	XGuildMember* pMember = gvar.MyInfo.Guild.GetMember(MLocale::ConvAnsiToUTF16(szName).c_str());
	if (pMember == NULL) return false;

	return pMember->m_bOnline;
}

const char* XGlueMyInfoFunction::GetGuildName()
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gvar.MyInfo.Guild.m_strName.c_str());

	return gvar.Temp.szTemp.c_str();
}

bool XGlueMyInfoFunction::IsGuildMaster()
{
	if(gvar.MyInfo.ChrInfo.szName == gvar.MyInfo.Guild.m_strMasterName)
		return true;
	else
		return false;
}

//팔레트에 등록된 아이템의 아이디를 서버에 등록한다
void XGlueMyInfoFunction::AddPaletteItem( int index, const char* szuid )
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( index >= gvar.MyInfo.PaletteList.GetContainerSize() || index < 0)
	//{
	//	_ASSERT(NULL);
	//	return;
	//}

	//if( szuid == NULL) return;

	//MUID itemuid = _atoi64(szuid);
	//if (itemuid == MUID::ZERO)	return;

	//XItem* pItem = gvar.MyInfo.Inventory.GetItem(itemuid);
	//if (pItem == NULL) return;

	//XPostPalette_PutUp(gvar.MyInfo.PaletteList.GetCurPaletteListIndex(),
	//	(PALETTE_SLOT)index, PIT_ITEM, pItem->m_pItemData->m_nID);
};

void XGlueMyInfoFunction::AddPaletteTalent( int index, const char* szID )
{
	if( index >= gvar.MyInfo.PaletteList.GetContainerSize() || index < 0)
	{
		_ASSERT(NULL);
		return;
	}

	if( szID== NULL) return;
	int nTalentID = atoi(szID);
	if (nTalentID == 0) return;

	// 탈렌트가 있으면 서버로 날려 데이타 갱신
	XTalentInfo* pTalentInfo = info.talent->Get(nTalentID);
	if (pTalentInfo == NULL) return;

	// TODO : 배운 탈렌트 인지 체크가 필요할까?

	XPostPalette_PutUp(gvar.MyInfo.PaletteList.GetCurPaletteListIndex(), (PALETTE_SLOT)index, PIT_TALENT, nTalentID);
};

//const char* XGlueMyInfoFunction::GetPaletteList()
//{
//	wstring strItemIDList;
//	gvar.MyInfo.PaletteList.GetItemIDList(strItemIDList);
//
//	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(strItemIDList.c_str());
//	return gvar.Temp.szTemp.c_str();
//}

int XGlueMyInfoFunction::GetPaletteListIndex()
{
	return gvar.MyInfo.PaletteList.GetCurPaletteListIndex();
}

void XGlueMyInfoFunction::UsePaletteIndex(int index)
{
	XPaletteItem* pPaletteItem = gvar.MyInfo.PaletteList.GetSelectedItem(index);
	if (pPaletteItem == NULL) return;

	if (pPaletteItem->itemID == 0)
	{
		gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionUseTalent(pPaletteItem->talentID);
	}
	else
	{
		XItem* pItem = gvar.MyInfo.Inventory.GetItemByID(pPaletteItem->itemID);
		if( pItem == NULL) return;

		gvar.Game.pMyPlayer->GetModuleMyControl()->DoActionUseItem(pItem->m_nSlotID);
	}
}

void XGlueMyInfoFunction::ChangePaletteList(int nIndex)
{
	gvar.MyInfo.PaletteList.SetCurPaletteListIndex((PALETTE_NUM)nIndex);
	XPostPalette_Select((PALETTE_NUM)nIndex);
}

void XGlueMyInfoFunction::ChangePalette(int nNum1, int nSlot1, int nSlot2)
{
	int nNum2 = gvar.MyInfo.PaletteList.GetCurPaletteListIndex();

	if ((nNum1 == nNum2) && (nSlot1 == nSlot2)) return;

	XPostPalette_Change((PALETTE_NUM)nNum1, (PALETTE_SLOT)nSlot1,
		(PALETTE_NUM)nNum2, (PALETTE_SLOT)nSlot2);
}

void XGlueMyInfoFunction::TakeAwayPalette(int nSlot)
{
	XPostPalette_PutDown(gvar.MyInfo.PaletteList.GetCurPaletteListIndex(), (PALETTE_SLOT)nSlot);
}

bool XGlueMyInfoFunction::IsQuestItem( const char* szuid )
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return false;
	//MUID uidItem = _atoi64(szuid);

	//XItem* pItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if( pItem == NULL)
	//{
	//	pItem = gvar.MyInfo.EquipmentSlot.GetItemByMUID(uidItem);
	//	if (pItem == NULL)
	//	{
	//		return false;
	//	}
	//}

	//return pItem->m_pItemData->m_bQuestRelated;
	
	return false;
}

int XGlueMyInfoFunction::GetStyleUsedTalent(int index)
{
	return gvar.MyInfo.Talent.GetStyleUsedTP((TALENT_STYLE)index);
}

void XGlueMyInfoFunction::WeaponBinding(int index)
{
	gvar.MyInfo.PaletteList.Binding(index);
}

bool XGlueMyInfoFunction::PrimaryBinding()
{
	return gvar.MyInfo.PaletteList.BindingIndex(0);
}

bool XGlueMyInfoFunction::SecondaryBinding()
{
	return gvar.MyInfo.PaletteList.BindingIndex(1);
}

MWLua::table XGlueMyInfoFunction::GetFactionInfoTable()
{
	return gvar.MyInfo.FactionInfo.GetFactionInfoTable();
}

const char* XGlueMyInfoFunction::GetID()
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gvar.MyInfo.ChrInfo.szName); 
	return gvar.Temp.szTemp.c_str();
}

const char* XGlueMyInfoFunction::GetSurName()
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(gvar.MyInfo.ChrInfo.szSurname);
	return gvar.Temp.szTemp.c_str();
}

const char* XGlueMyInfoFunction::GetFocusType()
{
	// 임시 문자열 테이블
	map<TALENT_FOCUS_TYPE, wstring> mapTempString;
	mapTempString[TFT_NONE] = L"";
	mapTempString[TFT_COUNTER] = XGetStr(L"FOCUS_DEFENDER");
	mapTempString[TFT_BERSERK] = XGetStr(L"FOCUS_BERSERKER");
	mapTempString[TFT_SNEAK] = XGetStr(L"FOCUS_ASSASSIN");
	mapTempString[TFT_PRECISION] = XGetStr(L"FOCUS_RANGER");
	mapTempString[TFT_ENLIGHTEN] = XGetStr(L"FOCUS_SORCERER");
	mapTempString[TFT_ADVENT] = XGetStr(L"FOCUS_CLERIC");
	// ------------------


	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(mapTempString[gvar.MyInfo.nFocusType].c_str());

	return gvar.Temp.szTemp.c_str();
}
