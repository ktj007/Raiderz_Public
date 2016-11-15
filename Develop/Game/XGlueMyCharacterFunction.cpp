#include "stdafx.h"
#include "XGlueMyCharacterFunction.h"
#include "XMyPlayer.h"
#include "XModulePlayerControl.h"
#include "XGame.h"
#include "XWorld.h"

void XGlueMyCharacterFunction::DeclGlueFunc( MWLua* pLua )
{
	MWLua::class_<XGlueMyCharacterFunction>(pLua->GetState(), "_shMycharacter")
		// 캐릭터 정보
		.def("GetID", &XGlueMyCharacterFunction::GetID)	
		.def("GetSurName", &XGlueMyCharacterFunction::GetSurName)
		.def("GetLevel", &XGlueMyCharacterFunction::GetLevel)	
		.def("GetBounty", &XGlueMyCharacterFunction::GetBounty)	
		.def("GetExpPercent", &XGlueMyCharacterFunction::GetExpPercent)
		.def("GetExp", &XGlueMyCharacterFunction::GetExp)

		.def("GetItemMaxWeight", &XGlueMyCharacterFunction::GetItemMaxWeight)	
		.def("GetItemWeight", &XGlueMyCharacterFunction::GetItemWeight)	

		// 능력 정보
		.def("GetSTR", &XGlueMyCharacterFunction::GetSTR)	
		.def("GetDEX", &XGlueMyCharacterFunction::GetDEX)	
		.def("GetINT", &XGlueMyCharacterFunction::GetINT)
		.def("GetCON", &XGlueMyCharacterFunction::GetCON)
		.def("GetCHA", &XGlueMyCharacterFunction::GetCHA)	

		// MOD
		.def("GetModSTR", &XGlueMyCharacterFunction::GetModSTR)	
		.def("GetModDEX", &XGlueMyCharacterFunction::GetModDEX)	
		.def("GetModINT", &XGlueMyCharacterFunction::GetModINT)
		.def("GetModCON", &XGlueMyCharacterFunction::GetModCON)
		.def("GetModCHA", &XGlueMyCharacterFunction::GetModCHA)	

		// 저항 정보
		.def("GetMR", &XGlueMyCharacterFunction::GetMR)	
		.def("GetFR", &XGlueMyCharacterFunction::GetFR)	
		.def("GetCR", &XGlueMyCharacterFunction::GetCR)	
		.def("GetLR", &XGlueMyCharacterFunction::GetLR)	
		.def("GetPR", &XGlueMyCharacterFunction::GetPR)	

		// 상태 정보
		.def("GetHP", &XGlueMyCharacterFunction::GetHP)	
		.def("GetEN", &XGlueMyCharacterFunction::GetEN)	
		.def("GetSTA", &XGlueMyCharacterFunction::GetSTA)

		.def("GetMaxHP", &XGlueMyCharacterFunction::GetMaxHP)	
		.def("GetMaxEN", &XGlueMyCharacterFunction::GetMaxEN)	
		.def("GetMaxSTA", &XGlueMyCharacterFunction::GetMaxSTA)	

		// 인벤 정보
		.def("UseItem", &XGlueMyCharacterFunction::UseItem)	
		.def("GetItemInfo", &XGlueMyCharacterFunction::GetItemInfo)
		.def("GetItemName", &XGlueMyCharacterFunction::GetItemName)

		.def("GetInventoryFirstItem", &XGlueMyCharacterFunction::GetInventoryFirstItem)	
		.def("GetInventoryNextItem", &XGlueMyCharacterFunction::GetInventoryNextItem)	

		// 장비착용 정보
		.def("GetEquippedItem", &XGlueMyCharacterFunction::GetEquippedItem)

		// 무기착용 정보

		// 탤런트 정보

		// 아이템 정보

		// 걸린 탤런트

		// 무기 사용에 대한 처리

		// 맵 정보
		.def("GetFieldName", &XGlueMyCharacterFunction::GetFieldName)
		;
}


void XGlueMyCharacterFunction::UseItem(unsigned int nItemIDHigh, unsigned int nItemIDLow)
{
	// [10/5/2007 isnara]
	// ItemKey에서는 XMyAtionUseItem에서 관리한다.
	MUID item_id(nItemIDHigh, nItemIDLow);

	//XPOSTCMD1(MC_ITEM_REQUEST_USE, MCommandParameterUID(item_id));
	gvar.Game.pMyPlayer->GetModulePlayerControl()->RequestUseItem(item_id);
}

void XGlueMyCharacterFunction::GetItemInfo(unsigned int nItemID_High, unsigned int nItemID_Low)
{

}

const char* XGlueMyCharacterFunction::GetItemName(unsigned int nItemID_High, unsigned int nItemID_Low)
{
	return gvar.MyInfo.Inventory.GetItem(MUID(nItemID_High, nItemID_Low))->m_pItemData->m_strName.c_str();
}


const char* XGlueMyCharacterFunction::GetInventoryFirstItem()
{
	XItem* i = gvar.MyInfo.Inventory.FirstItem();
	if (i==NULL)
	{
		return "";
	}

	return i->m_pItemData->m_strName.c_str();
}

const char* XGlueMyCharacterFunction::GetInventoryNextItem()
{
	XItem* i = gvar.MyInfo.Inventory.NextItem();
	if (i == NULL)
	{
		return "";
	}

	return i->m_pItemData->m_strName.c_str();
}

const char* XGlueMyCharacterFunction::GetEquippedItem(const char* slot)
{
	return gvar.MyInfo.EquipmentSlot.GetItemString(slot);
}

const char* XGlueMyCharacterFunction::GetFieldName()
{
	if(global.game && global.game->GetWorld())
	{
		return global.game->GetWorld()->GetInfo()->m_strFieldName.c_str();
	}

	return NULL;
}