#include "stdafx.h"
#include "FMyActionState.h"
#include "XMyPlayer.h"


FMyActionState::FMyActionState()
{
	m_pMyPlayer = GetMyPlayer();

	XUnitTestHelper::CreateItemSwordInfo(1);
	XUnitTestHelper::CreateItemSwordInfo(2);

	XItem* pWeaponItem1 = info.item->MakeNewItem(1);
	XItem* pWeaponItem2 = info.item->MakeNewItem(2);

	gvar.MyInfo.EquipmentSlot.EquipItem(pWeaponItem1,		ITEMSLOT_RWEAPON);
	gvar.MyInfo.EquipmentSlot.EquipItem(pWeaponItem2,		ITEMSLOT_RWEAPON2);

}

FMyActionState::~FMyActionState()
{

}