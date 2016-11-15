#include "stdafx.h"
#include "CSItemHelper.h"
#include "CSPalette.h"

WEAPON_TYPE CSItemHelper::GetCurrentWeaponType( WEAPON_TYPE nRight, WEAPON_TYPE nLeft, bool bEquipShield )
{
	if (nRight != WEAPON_NONE && 
		nLeft != WEAPON_NONE &&
		bEquipShield == false)
	{
		if (nLeft == WEAPON_1H_PIERCE && nRight == WEAPON_1H_PIERCE)
		{
			return WEAPON_DUAL_PIERCE;
		}

		return WEAPON_DUAL_WIELD;
	}
	return nRight;
}

WEAPON_TYPE CSItemHelper::GetCurrentWeaponType( CSItemData* pRightItemData, CSItemData* pLeftItemData )
{
	WEAPON_TYPE nRightWeaponType = WEAPON_NONE;
	WEAPON_TYPE nLeftWeaponType = WEAPON_NONE;
	bool bEquipShield = false;

	if (pRightItemData) nRightWeaponType = pRightItemData->m_nWeaponType;
	if (pLeftItemData) 
	{
		if (ARMOR_SHIELD == pLeftItemData->m_nArmorType)
		{
			bEquipShield = true;
		}

		nLeftWeaponType = pLeftItemData->m_nWeaponType;
	}

	return GetCurrentWeaponType(nRightWeaponType, nLeftWeaponType, bEquipShield);
}

SH_ITEM_SLOT CSItemHelper::GetSubSlot(SH_ITEM_SLOT nSlot, CSItemData* pItemData)
{
	if (NULL == pItemData) return ITEMSLOT_NONE;

	if (ITEMSLOT_RWEAPON2 == nSlot && ITEMSLOT_LWEAPON == pItemData->m_nSubSlot)
	{
		return ITEMSLOT_LWEAPON2;
	}

	if (ITEMSLOT_LWEAPON2 == nSlot && ITEMSLOT_RWEAPON == pItemData->m_nSubSlot)
	{
		return ITEMSLOT_RWEAPON2;
	}

	return pItemData->m_nSubSlot;
}

bool CSItemHelper::IsDualWieldWeaponType( WEAPON_TYPE nWeaponType )
{
	switch (nWeaponType)
	{
	case WEAPON_1H_SLASH:
	case WEAPON_1H_BLUNT:
		return true;
	}
	return false;
}


bool CSItemHelper::IsShieldItemType(CSItemData* pItemData)
{
	if ((pItemData->m_nItemSlot == ITEMSLOT_LWEAPON) && (pItemData->m_ItemType == ITEMTYPE_ARMOR)) return true;
	return false;
}

bool CSItemHelper::IsGuardableWeaponType( WEAPON_TYPE nWeaponType )
{
	if (nWeaponType == WEAPON_1H_SLASH ||
		nWeaponType == WEAPON_1H_BLUNT ||
		nWeaponType == WEAPON_TWO_HANDED ||
		nWeaponType == WEAPON_2H_BLUNT ||
		nWeaponType == WEAPON_STAFF) return true;

	return false;
}

bool CSItemHelper::IsLeftWeaponSlot( SH_ITEM_SLOT nSlot )
{
	if (nSlot == ITEMSLOT_LWEAPON || nSlot == ITEMSLOT_LWEAPON2) return true;
	return false;
}

bool CSItemHelper::IsRightWeaponSlot( SH_ITEM_SLOT nSlot )
{
	if (nSlot == ITEMSLOT_RWEAPON || nSlot == ITEMSLOT_RWEAPON2) return true;
	return false;
}

bool CSItemHelper::IsWeaponSlot( SH_ITEM_SLOT nSlot )
{
	if (nSlot == ITEMSLOT_RWEAPON || nSlot == ITEMSLOT_RWEAPON2) return true;
	if (nSlot == ITEMSLOT_LWEAPON || nSlot == ITEMSLOT_LWEAPON2) return true;

	return false;
}

bool CSItemHelper::IsPrimaryWeaponSlot( SH_ITEM_SLOT nSlot )
{
	if (nSlot == ITEMSLOT_LWEAPON || nSlot == ITEMSLOT_RWEAPON) return true;

	return false;
}

bool CSItemHelper::IsSecondaryWeaponSlot( SH_ITEM_SLOT nSlot )
{
	if (nSlot == ITEMSLOT_LWEAPON2 || nSlot == ITEMSLOT_RWEAPON2) return true;

	return false;
}

SH_ITEM_SLOT CSItemHelper::GetPrimaryWeaponSlot( SH_ITEM_SLOT nSlot )
{
	if (nSlot == ITEMSLOT_LWEAPON2)
	{
		return ITEMSLOT_LWEAPON;
	}

	if (nSlot == ITEMSLOT_RWEAPON2)
	{
		return ITEMSLOT_RWEAPON;
	}

	return ITEMSLOT_NONE;
}

SH_ITEM_SLOT CSItemHelper::GetSecondaryWeaponSlot( SH_ITEM_SLOT nSlot )
{
	if (nSlot == ITEMSLOT_LWEAPON)
	{
		return ITEMSLOT_LWEAPON2;
	}

	if (nSlot == ITEMSLOT_RWEAPON)
	{
		return ITEMSLOT_RWEAPON2;
	}

	return ITEMSLOT_NONE;
}

SH_ITEM_SLOT CSItemHelper::GetOppositeWeaponSlot( SH_ITEM_SLOT nSlot )
{
	if (nSlot == ITEMSLOT_LWEAPON) return ITEMSLOT_RWEAPON;
	if (nSlot == ITEMSLOT_RWEAPON) return ITEMSLOT_LWEAPON;
	if (nSlot == ITEMSLOT_LWEAPON2) return ITEMSLOT_RWEAPON2;
	if (nSlot == ITEMSLOT_RWEAPON2) return ITEMSLOT_LWEAPON2;
	return ITEMSLOT_NONE;
}

bool CSItemHelper::IsTogetherEquipableItemData(CSItemData* pItemData1, CSItemData* pItemData2)
{
	if (NULL == pItemData1) return false;
	if (NULL == pItemData2) return false;

	WEAPON_TYPE nWeaponType1 = pItemData1->m_nWeaponType;
	WEAPON_TYPE nWeaponType2 = pItemData2->m_nWeaponType;
	ARMOR_TYPE nArmorType1 = pItemData1->m_nArmorType;
	ARMOR_TYPE nArmorType2 = pItemData2->m_nArmorType;

	if (nArmorType1 == ARMOR_SHIELD  || nArmorType2 == ARMOR_SHIELD)
	{
		// 방패는 한손둔기와 한손검과만 함께 착용할수 있다.
		if ((nArmorType1 == ARMOR_SHIELD &&	CSItemHelper::IsEquipableWithShield(nWeaponType2) == false) ||
			(nArmorType2 == ARMOR_SHIELD &&	CSItemHelper::IsEquipableWithShield(nWeaponType1) == false))
		{
			return false;
		}
	}
	else
	{
		if (nWeaponType1 == WEAPON_1H_PIERCE && 
			nWeaponType2 != WEAPON_1H_PIERCE)
		{
			return false;
		}

		if (nWeaponType2 == WEAPON_1H_PIERCE && 
			nWeaponType1 != WEAPON_1H_PIERCE)
		{
			return false;
		}

		if (IsDualWieldWeaponType(nWeaponType1) == true &&
			IsDualWieldWeaponType(nWeaponType2) == false)
		{
			return false;
		}

		if (IsDualWieldWeaponType(nWeaponType2) == true &&
			IsDualWieldWeaponType(nWeaponType1) == false)
		{
			return false;
		}
	}
	
	return true;
}

bool CSItemHelper::IsEquipableWithShield( WEAPON_TYPE nWeaponType )
{
	if (nWeaponType == WEAPON_NONE ||
		nWeaponType == WEAPON_1H_SLASH ||
		nWeaponType == WEAPON_1H_BLUNT)
	{
		return true;
	}
	return false;
}

SH_ITEM_SLOT CSItemHelper::GetProperWeaponSlot( SH_ITEM_SLOT nWishSlot, CSItemData* pItemData, CSItemData* pRightEquipedItemData, CSItemData* pLeftEquipedItemData )
{
	if (!pItemData || pItemData->m_ItemType != ITEMTYPE_WEAPON)
	{
		return nWishSlot;
	}

	WEAPON_TYPE nItemWeaponType = pItemData->m_nWeaponType;
	WEAPON_TYPE nRightEquipedWeaponType = WEAPON_NONE;
	WEAPON_TYPE nLeftEquipedWeaponType = WEAPON_NONE;

	if (pRightEquipedItemData) nRightEquipedWeaponType = pRightEquipedItemData->m_nWeaponType;
	if (pLeftEquipedItemData) nLeftEquipedWeaponType = pLeftEquipedItemData->m_nWeaponType;

	if (IsRightWeaponSlot(nWishSlot)) 
	{
		return nWishSlot;
	}
	else
	{
		if (nRightEquipedWeaponType == WEAPON_NONE) return GetOppositeWeaponSlot(nWishSlot);
		if (nItemWeaponType == WEAPON_1H_PIERCE && nRightEquipedWeaponType != WEAPON_1H_PIERCE)
		{
			return GetOppositeWeaponSlot(nWishSlot);
		}

		if (IsDualWieldWeaponType(nItemWeaponType) && !IsDualWieldWeaponType(nRightEquipedWeaponType))
		{
			return GetOppositeWeaponSlot(nWishSlot);
		}

		if (pLeftEquipedItemData && IsShieldItemType(pLeftEquipedItemData))
		{
			return GetOppositeWeaponSlot(nWishSlot);
		}
	}

	return nWishSlot;
}

bool CSItemHelper::IsReusableItem( CSItemData* pItemData )
{
	if (pItemData->m_ItemType == ITEMTYPE_USABLE &&
		pItemData->m_bUsableConsume == false)
	{
		return true;
	}
	return false;
}

bool CSItemHelper::IsQuestTriggerItem( CSItemData* pItemData )
{
	if (pItemData->m_ItemType == ITEMTYPE_USABLE &&
		pItemData->m_nUsableType == USABLE_QUEST_ADD)
	{
		return true;
	}
	return false;

}

bool CSItemHelper::IsTalentTrainItem( CSItemData* pItemData )
{
	if (pItemData->m_ItemType == ITEMTYPE_USABLE &&
		pItemData->m_nUsableType == USABLE_TALENT_TRAIN)
	{
		return true;
	}
	return false;

}

int CSItemHelper::GetPalleteIndex( int nNum, int nSlot )
{
	return (nNum*PALETTESLOT_MAX)+nSlot;
}

bool CSItemHelper::IsEquipableSlot(SH_ITEM_SLOT nSlot, CSItemData* pItemData)
{
	if (pItemData->m_nItemSlot < 0 ||
		pItemData->m_nItemSlot >= ITEMSLOT_MAX) return false;

	if (nSlot < 0 || 
		nSlot >= ITEMSLOT_MAX)	return false;

	// 반지는 왼손 오른손 상관없이 장비할수 있다.
	if (nSlot == ITEMSLOT_LFINGER || nSlot == ITEMSLOT_RFINGER)
	{
		if (pItemData->m_nItemSlot == ITEMSLOT_LFINGER ||
			pItemData->m_nItemSlot == ITEMSLOT_RFINGER)
		{
			return true;
		}
	}

	if (nSlot == ITEMSLOT_LWEAPON || nSlot == ITEMSLOT_LWEAPON2)
	{
		// 오른손무기지만 왼손에도 장착가능한 아이템은 왼손무기슬롯에 장비할수 있다.
		if (true == pItemData->m_bLeftWeaponEquipable)
		{
			if (pItemData->m_nItemSlot == ITEMSLOT_RWEAPON ||
				pItemData->m_nItemSlot == ITEMSLOT_LWEAPON)
			{
				return true;
			}
		}
	}

	if (nSlot == ITEMSLOT_LWEAPON2 && pItemData->m_nItemSlot == ITEMSLOT_LWEAPON)
	{
		return true;
	}

	if (nSlot == ITEMSLOT_RWEAPON2 && pItemData->m_nItemSlot == ITEMSLOT_RWEAPON)
	{
		return true;
	}

	if (nSlot != pItemData->m_nItemSlot)
	{
		return false;
	}

	return true;
}

bool CSItemHelper::IsMustCheckPassiveType(TALENT_EXTRA_PASSIVE_TYPE nPassiveType)
{
	switch (nPassiveType)
	{
	case TEPT_BASE_WEAPON_ONE_HANDED:
	case TEPT_BASE_WEAPON_1H_SLASH:
	case TEPT_BASE_WEAPON_1H_BLUNT:
	case TEPT_BASE_WEAPON_1H_PIERCE:
	case TEPT_BASE_WEAPON_DUAL_PIERCE:
	case TEPT_BASE_WEAPON_TWO_HANDED:
	case TEPT_BASE_WEAPON_2H_BLUNT:
	case TEPT_BASE_WEAPON_ARCHERY:
	case TEPT_BASE_WEAPON_GUN:
	case TEPT_BASE_WEAPON_STAFF:
	case TEPT_BASE_WEAPON_BOOK:
	case TEPT_BASE_SNEAK:
	case TEPT_BASE_ELEMENTAL:
	case TEPT_BASE_HOLY:
	case TEPT_SPECIALIZATION_SLASH:
	case TEPT_SPECIALIZATION_BLUNT:
	case TEPT_SPECIALIZATION_PIERCE:
	case TEPT_SPECIALIZATION_ELEMENTAL:
	case TEPT_SPECIALIZATION_HOLY:
	case TEPT_UTILITY_SAFEFALL:
	case TEPT_UTILITY_SAFEFALL2:
	case TEPT_BASE_ARMOR_LIGHT:
	case TEPT_BASE_ARMOR_MEDUIM:
	case TEPT_BASE_ARMOR_HEAVY:
	case TEPT_TALENT_SHOUT_RADIUS:
	case TEPT_TALENT_SAZAHU_SPECIALIZATION:
	case TEPT_FOCUS_USE_BERSERK:
	case TEPT_FOCUS_USE_COUNTER:
		{
			return true;
		}
		break;
	}

	return false;
}

TALENT_EXTRA_PASSIVE_TYPE CSItemHelper::GetTEPTForEquip(CSItemData* pItemData)
{
	if (NULL == pItemData) return TEPT_NONE;

	if (pItemData->m_nWeaponType != WEAPON_NONE)
	{
		switch (pItemData->m_nWeaponType)
		{
		case WEAPON_1H_SLASH	: return TEPT_BASE_WEAPON_1H_SLASH; 
		case WEAPON_1H_BLUNT	: return TEPT_BASE_WEAPON_1H_BLUNT;
		case WEAPON_1H_PIERCE	: return TEPT_BASE_WEAPON_1H_PIERCE;
		case WEAPON_DUAL_PIERCE	: return TEPT_BASE_WEAPON_DUAL_PIERCE;
		case WEAPON_TWO_HANDED	: return TEPT_BASE_WEAPON_TWO_HANDED;
		case WEAPON_2H_BLUNT	: return TEPT_BASE_WEAPON_2H_BLUNT;
		case WEAPON_ARCHERY		: return TEPT_BASE_WEAPON_ARCHERY;
		case WEAPON_STAFF		: return TEPT_BASE_WEAPON_STAFF;
		}

		return TEPT_NONE;
	}
	else if (pItemData->m_nArmorType != ARMOR_NONE) 
	{
		switch (pItemData->m_nArmorType)
		{
		case ARMOR_LIGHT	: return TEPT_BASE_ARMOR_LIGHT; 
		case ARMOR_MEDIUM	: return TEPT_BASE_ARMOR_MEDUIM;
		case ARMOR_HEAVY	: return TEPT_BASE_ARMOR_HEAVY;
		}

		return TEPT_NONE;
	}	

	return TEPT_NONE;
}