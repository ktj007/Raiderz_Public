#include "stdafx.h"
#include "XGlueGameRepairFunction.h"
#include "XPost_NPCShop.h"

void XGlueGameRepairFunction::ReqRepairItemList()
{
	
}

void XGlueGameRepairFunction::RepairAllItem()
{	
	XPostNPCShop_RepairAll();
}

void XGlueGameRepairFunction::RepairItem( const char* szuid )
{
	// ! GetItem(MUID) 함수가 제거되어, slotID로만 접근할 수 있습니다. - praptor, 2010.03.03
	// ! 사용하지 않는 함수로 판단되어 빌드에러 막고자, 이하 주석처리합니다. - praptor, 2010.03.03

	//if( szuid == NULL) return;
	//MUID uidItem = _atoi64(szuid);

	//XItem* pItem = gvar.MyInfo.Inventory.GetItem(uidItem);
	//if (pItem != NULL)
	//{
	//	XPostNPCShop_RepairOne(SLOTTYPE_INVENTORY, pItem->m_nSlotID);
	//	return;
	//}

	//pItem = gvar.MyInfo.EquipmentSlot.GetItemByMUID(uidItem);
	//if (pItem != NULL)
	//{
	//	XPostNPCShop_RepairOne(SLOTTYPE_EQUIP, pItem->m_nSlotID);
	//	return;
	//}
}