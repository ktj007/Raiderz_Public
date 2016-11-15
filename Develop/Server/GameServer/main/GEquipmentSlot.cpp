#include "stdafx.h"
#include "GEquipmentSlot.h"
#include "GItem.h"
#include "CSItemHelper.h"
#include "GEntityPlayer.h"
#include "AStlUtil.h"
#include "GOverlappedEquipmentSlot.h"
#include "GGlobal.h"
#include "GItemEquiper.h"

GEquipmentSlot::GEquipmentSlot(GEntityPlayer* pOwner)
: GItemContainer(ITEMSLOT_MAX, pOwner)
, m_nWeaponSet(ITEMSWITCH_PRIMARY)
, m_pOverlapped(NULL)
{
	m_pOverlapped = new GOverlappedEquipmentSlot();
}

GEquipmentSlot::~GEquipmentSlot()
{
	Clear();
	SAFE_DELETE(m_pOverlapped);
}

WEAPON_TYPE GEquipmentSlot::GetWeaponType() const
{
	WEAPON_TYPE nLeftWeaponType = GetLeftWeaponType();
	WEAPON_TYPE nRightWeaponType = GetRightWeaponType();
	bool bEquipShield = IsEquipShield();

	return CSItemHelper::GetCurrentWeaponType(nRightWeaponType, nLeftWeaponType, bEquipShield);
}

WEAPON_TYPE GEquipmentSlot::GetLeftWeaponTypeByWeaponSet(SH_ITEM_SWITCH_WEAPON nWeaponSet) const
{
	GItem* pLeftItem = GetLeftWeaponByWeaponSet(nWeaponSet);
	if (NULL == pLeftItem) return WEAPON_NONE;

	// 방패는 둔기로 취급한다.
	if (ARMOR_SHIELD == pLeftItem->m_pItemData->m_nArmorType) return WEAPON_1H_BLUNT;

	return pLeftItem->m_pItemData->m_nWeaponType;
}

WEAPON_TYPE GEquipmentSlot::GetRightWeaponTypeByWeaponSet(SH_ITEM_SWITCH_WEAPON nWeaponSet) const
{
	GItem* pRightItem = GetRightWeaponByWeaponSet(nWeaponSet);
	if (NULL == pRightItem) return WEAPON_NONE;
	
	return pRightItem->m_pItemData->m_nWeaponType;
}

GItem* GEquipmentSlot::GetLeftWeaponByWeaponSet( SH_ITEM_SWITCH_WEAPON nWeaponSet ) const
{
	GItem* pItem = m_pOverlapped->GetLeftWeapon();
	if (pItem)
		return pItem;

	SH_ITEM_SLOT nLeftWeaponSlot = (nWeaponSet == ITEMSWITCH_PRIMARY) ? ITEMSLOT_LWEAPON : ITEMSLOT_LWEAPON2;

	return GetItem(nLeftWeaponSlot);
}

GItem* GEquipmentSlot::GetRightWeaponByWeaponSet( SH_ITEM_SWITCH_WEAPON nWeaponSet ) const
{
	GItem* pItem = m_pOverlapped->GetRightWeapon();
	if (pItem)
		return pItem;

	SH_ITEM_SLOT nRightWeaponSlot = (nWeaponSet == ITEMSWITCH_PRIMARY) ? ITEMSLOT_RWEAPON : ITEMSLOT_RWEAPON2;

	return GetItem(nRightWeaponSlot);
}

WEAPON_TYPE GEquipmentSlot::GetLeftWeaponType() const
{
	return GetLeftWeaponTypeByWeaponSet(m_nWeaponSet);
}

WEAPON_TYPE GEquipmentSlot::GetRightWeaponType() const
{
	return GetRightWeaponTypeByWeaponSet(m_nWeaponSet);
}

ARMOR_TYPE GEquipmentSlot::GetArmorType() const
{
	GItem* pItem = GetArmor();
	if (pItem == NULL)	return ARMOR_NONE;

	VALID_RET(pItem->m_pItemData, ARMOR_NONE);
	return pItem->m_pItemData->m_nArmorType;
}

GItem* GEquipmentSlot::GetLeftWeapon() const
{
	return GetLeftWeaponByWeaponSet(m_nWeaponSet);
}

GItem* GEquipmentSlot::GetRightWeapon() const
{
	return GetRightWeaponByWeaponSet(m_nWeaponSet);
}

GItem* GEquipmentSlot::GetShield() const
{
	GItem* pItem = GetLeftWeapon();
	if (!pItem) 
		return NULL;

	if (pItem->m_pItemData->m_nArmorType != ARMOR_SHIELD)
		return NULL;

	return pItem;
}

GItem* GEquipmentSlot::GetArmor() const
{
	return GetItem(ITEMSLOT_BODY);
}

bool GEquipmentSlot::IsEquipShield() const
{
	if (NULL == GetShield()) return false;

	return true;
}

bool GEquipmentSlot::IsUnarmed() const
{
	return IsUnarmedByWeaponSet(m_nWeaponSet);
}

void GEquipmentSlot::CollectAllItem(vector<GItem*>& outvecItem)
{
	for (int i = ITEMSLOT_HEAD; i <= ITEMSLOT_RWEAPON2; i++)
	{
		GItem* pItem = GetItem(static_cast<SH_ITEM_SLOT>(i));
		if (NULL != pItem)
		{
			outvecItem.push_back(pItem);
		}
	}
}

void GEquipmentSlot::CollectArmorSlotItem(vector<GItem*>& outvecEquipmentItem)
{
	for (int i = ITEMSLOT_ARMOR_START; i <= ITEMSLOT_ARMOR_END; i++)
	{
		GItem* pItem = GetItem(static_cast<SH_ITEM_SLOT>(i));
		if (NULL == pItem) continue;

		outvecEquipmentItem.push_back(pItem);
	}
}

int GEquipmentSlot::GetTotalAP() 
{
	int ret = 0;
	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(i);
		if (NULL == pItem) continue;
		GItemData* pItemData = pItem->m_pItemData;
		if (NULL == pItemData) continue;
		if (0 < pItemData->m_nMaxDurability && 0 >= pItem->m_nDurability) continue;
		if (!gsys.pItemSystem->GetEquiper().CheckHasPassiveTalent(GetOwnerPlayer(), pItem)) continue;

		if ((ITEMSLOT_LWEAPON == i || ITEMSLOT_RWEAPON == i) && 
			ITEMSWITCH_PRIMARY != m_nWeaponSet) continue;

		if ((ITEMSLOT_LWEAPON2 == i || ITEMSLOT_RWEAPON2 == i) && 
			ITEMSWITCH_SECONDARY != m_nWeaponSet) continue;

		ret += pItem->m_pItemData->m_nAP;
	}

	return ret;
}

bool GEquipmentSlot::IsEquipItem( int nItemID )
{
	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(static_cast<SH_ITEM_SLOT>(i));
		if (NULL == pItem) continue;
		GItemData* pItemData = pItem->m_pItemData;
		if (NULL == pItemData) continue;
		if (nItemID != pItemData->m_nID) continue;

		return true;
	}

	return false;
}

bool GEquipmentSlot::IsEquipItem(GItem* pItem)
{
	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pEquipItem = GetItem(static_cast<SH_ITEM_SLOT>(i));
		if (pItem != pEquipItem) continue;

		return true;
	}

	return false;
}

bool GEquipmentSlot::PushItem(int nSlotID, GItem* pItem)
{
	SH_ITEM_SLOT nSlot = (SH_ITEM_SLOT)nSlotID;

	VALID_RET(pItem != NULL, false);

	__super::PushItem(nSlot, pItem);

	pItem->SetEquiped(true);
	pItem->m_nSlotType = SLOTTYPE_EQUIP;

	if ((CSItemHelper::IsPrimaryWeaponSlot(nSlot) && ITEMSWITCH_PRIMARY != m_nWeaponSet) ||
		(CSItemHelper::IsSecondaryWeaponSlot(nSlot) && ITEMSWITCH_SECONDARY != m_nWeaponSet))
	{
		return true;
	}

	// Item 보정 적용	
	AddModifierByEquip(pItem);	

	m_pOwnerPlayer->OnItemEquipped(pItem);

	return true;
}

GItem* GEquipmentSlot::PopItem(int nSlotID)
{
	SH_ITEM_SLOT nSlot = (SH_ITEM_SLOT)nSlotID;

	if (nSlot < 0 || ITEMSLOT_MAX <= nSlot) return NULL;

	GItem* pItem = __super::PopItem(nSlot);
	if (!pItem) return NULL;

	pItem->SetEquiped(false);
	pItem->m_nSlotType = SLOTTYPE_INVENTORY;

	if ((CSItemHelper::IsPrimaryWeaponSlot(nSlot) && ITEMSWITCH_PRIMARY != m_nWeaponSet) ||
	    (CSItemHelper::IsSecondaryWeaponSlot(nSlot) && ITEMSWITCH_SECONDARY != m_nWeaponSet))
	{
		return pItem;
	}
	
	// Item 보정 해제
	EraseModifierByUnequip(pItem);

	m_pOwnerPlayer->OnItemUnequipped(pItem);
	
	return pItem;
}

SH_ITEM_SLOT GEquipmentSlot::GetSubSlot( SH_ITEM_SLOT nSlot )
{
	if (ITEMSLOT_NONE == nSlot) return ITEMSLOT_NONE;

	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(SH_ITEM_SLOT(i));
		if (!pItem)					continue;
		CSItemData* pItemData = pItem->m_pItemData;
		_ASSERT(pItemData);
		if (!pItemData)	continue;

		if (nSlot == CSItemHelper::GetSubSlot(static_cast<SH_ITEM_SLOT>(pItem->m_nSlotID), pItemData))
			return static_cast<SH_ITEM_SLOT>(pItem->m_nSlotID);
	}

	return ITEMSLOT_NONE;
}

GItem* GEquipmentSlot::GetItemFromSubSlot( SH_ITEM_SLOT nSubSlot ) const
{
	if (ITEMSLOT_NONE == nSubSlot) return NULL;

	for (int i = 0; i < GetSlotSize(); i++)
	{
		GItem* pItem = GetItem(SH_ITEM_SLOT(i));
		if (!pItem)					continue;
		CSItemData* pItemData = pItem->m_pItemData;
		_ASSERT(pItemData);
		if (!pItemData)	continue;

		SH_ITEM_SLOT nCompareSlot = pItemData->m_nSubSlot;
		if (true == CSItemHelper::IsSecondaryWeaponSlot(SH_ITEM_SLOT(i)))
		{
			nCompareSlot = CSItemHelper::GetSecondaryWeaponSlot(pItemData->m_nSubSlot);
			if (ITEMSLOT_NONE == nCompareSlot) continue;
		}

		if (nSubSlot == nCompareSlot)
		{
			return pItem;
		}
	}

	return NULL;
}

ARMOR_TYPE GEquipmentSlot::GetSlotMaterial( SH_ITEM_SLOT nSlot )
{
	GItem* pItem = GetItem(nSlot);
	if (pItem == NULL || pItem->m_pItemData == NULL) return ARMOR_NONE;
	return pItem->m_pItemData->m_nArmorType;
}

bool GEquipmentSlot::IsEquipItem( SH_ITEM_SLOT nSlot )
{
	if (NULL == GetItem(nSlot)) return false;

	return true;
}

SH_ITEM_SLOT GEquipmentSlot::GetEquipedSlot(const GItem* pItem) const
{
	for (int i = 0; i < GetSlotSize(); i++)
	{
		SH_ITEM_SLOT nSlot = static_cast<SH_ITEM_SLOT>(i);

		GItem* pEquipItem = GetItem(nSlot);
		if (pItem == pEquipItem)
		{
			return nSlot;
		}
	}

	return ITEMSLOT_NONE;
}

void GEquipmentSlot::SetWeaponSet(SH_ITEM_SWITCH_WEAPON val)
{ 
	m_nWeaponSet = val; 
}

void GEquipmentSlot::SwitchingWeaponSet( SH_ITEM_SWITCH_WEAPON val)
{
	if (m_nWeaponSet == val) return;

	GItem* pUnEquipItem1 = NULL;
	GItem* pUnequipItem2 = NULL;
	GItem* pEquipItem1 = NULL;
	GItem* pEquipItem2 = NULL;

	if (ITEMSWITCH_PRIMARY == val)
	{
		pEquipItem1 = GetItem(ITEMSLOT_RWEAPON);
		pEquipItem2 = GetItem(ITEMSLOT_LWEAPON);
		pUnEquipItem1 = GetItem(ITEMSLOT_RWEAPON2);
		pUnequipItem2 = GetItem(ITEMSLOT_LWEAPON2);
	}
	else
	{
		pUnEquipItem1 = GetItem(ITEMSLOT_RWEAPON);
		pUnequipItem2 = GetItem(ITEMSLOT_LWEAPON);
		pEquipItem1 = GetItem(ITEMSLOT_RWEAPON2);
		pEquipItem2 = GetItem(ITEMSLOT_LWEAPON2);
	}	

	AddModifierByEquip(pEquipItem1);
	AddModifierByEquip(pEquipItem2);
	EraseModifierByUnequip(pUnEquipItem1);
	EraseModifierByUnequip(pUnequipItem2);

	GEntityPlayer* pPlayer = GetOwnerPlayer();
	pPlayer->OnItemUnequipped(pUnEquipItem1);
	pPlayer->OnItemUnequipped(pUnequipItem2);
	SetWeaponSet(val);
	pPlayer->OnItemEquipped(pEquipItem1);
	pPlayer->OnItemEquipped(pEquipItem2);
	
	pPlayer->OnSwitchingWeaponSet(val);
}

SH_ITEM_SWITCH_WEAPON GEquipmentSlot::GetWeaponSet() const	
{
	return m_nWeaponSet; 
}

void GEquipmentSlot::AddModifierByTrain( TALENT_EXTRA_PASSIVE_TYPE nTrainTEPT )
{
	if (TEPT_NONE == nTrainTEPT) return;

	for (int i = 0; i< GetSlotSize(); i++)
	{
		SH_ITEM_SLOT nSlot = static_cast<SH_ITEM_SLOT>(i);
		GItem* pItem = GetItem(nSlot);
		if (pItem == NULL)
			continue;
		if (nTrainTEPT != CSItemHelper::GetTEPTForEquip(pItem->m_pItemData))
			continue;

		AddModifier(pItem);
	}
}

void GEquipmentSlot::EraseModifierByUntrainAll()
{
	for (int i = 0; i < GetSlotSize(); i++)
	{
		SH_ITEM_SLOT nSlot = static_cast<SH_ITEM_SLOT>(i);
		GItem* pItem = GetItem(nSlot);
		if (pItem == NULL)
			continue;
		if (gsys.pItemSystem->GetEquiper().CheckHasPassiveTalent(GetOwnerPlayer(), pItem))
			continue;

		EraseModifier(pItem);
	}
}

void GEquipmentSlot::AddModifierByEquip(GItem* pItem )
{
	if (NULL == pItem) return;
	GEntityPlayer* pPlayer = GetOwnerPlayer();

	if (!gsys.pItemSystem->GetEquiper().CheckHasPassiveTalent(pPlayer, pItem)) return;

	AddModifier(pItem);
}

void GEquipmentSlot::EraseModifierByUnequip(GItem* pItem )
{
	if (NULL == pItem) return;
	GEntityPlayer* pPlayer = GetOwnerPlayer();

	if (!gsys.pItemSystem->GetEquiper().CheckHasPassiveTalent(pPlayer, pItem)) return;

	EraseModifier(pItem);
}

void GEquipmentSlot::AddModifier(GItem* pItem)
{
	GEntityPlayer* pPlayer = GetOwnerPlayer();

	m_ModEffector.AddPlayerModifier(pPlayer->GetPlayerModifier(), pItem->m_pItemData->m_PlayerModifier);
	m_ModEffector.AddActorModifier(pPlayer->GetChrStatus()->ActorModifier, pItem->m_pItemData->m_ActorModifier);
	m_ModEffector.RefreshStatus(pPlayer);
}

void GEquipmentSlot::EraseModifier(GItem* pItem)
{
	GEntityPlayer* pPlayer = GetOwnerPlayer();

	m_ModEffector.ErasePlayerModifier(pPlayer->GetPlayerModifier(), pItem->m_pItemData->m_PlayerModifier);
	m_ModEffector.EraseActorModifier(pPlayer->GetChrStatus()->ActorModifier, pItem->m_pItemData->m_ActorModifier);
	m_ModEffector.RefreshStatus(pPlayer);
}



bool GEquipmentSlot::IsUnarmedByWeaponSet( SH_ITEM_SWITCH_WEAPON nWeaponSet ) const
{
	if (WEAPON_NONE == GetRightWeaponTypeByWeaponSet(nWeaponSet) && 
		(WEAPON_NONE == GetLeftWeaponTypeByWeaponSet(nWeaponSet) || true == IsEquipShield()))
	{
		return true;
	}		

	return false;
}

bool GEquipmentSlot::IsWeaponSwitchable( SH_ITEM_SWITCH_WEAPON nWeaponSet ) const
{
	_ASSERT(nWeaponSet == 0 || nWeaponSet == 1);
	if (nWeaponSet < 0 || nWeaponSet > 1)			return	false;
	if (IsUnarmedByWeaponSet(nWeaponSet))						return	false;

	return true;
}

GOverlappedEquipmentSlot& GEquipmentSlot::GetOverlapped()
{
	return *m_pOverlapped;
}