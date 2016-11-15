#include "StdAfx.h"
#include "GItemActableChecker.h"
#include "GEntityPlayer.h"
#include "GPlayerDoing.h"
#include "GItemStateSelector.h"

bool GItemActableChecker::IsDropable(GEntityPlayer* pPlayer, GItem* pItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return IsItemActable(pPlayer, pItem, IAT_DROP);
}

bool GItemActableChecker::IsSellable(GEntityPlayer* pPlayer, GItem* pItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return IsItemActable(pPlayer, pItem, IAT_SELL);
}

bool GItemActableChecker::IsTradable( GEntityPlayer* pPlayer, GItem* pItem )
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return IsItemActable(pPlayer, pItem, IAT_TRADE);
}

bool GItemActableChecker::IsMailAppendable(GEntityPlayer* pPlayer, const GItem* pItem) const
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return IsItemActable(pPlayer, pItem, IAT_MAIL_APPEND);
}

bool GItemActableChecker::IsEquipable(GEntityPlayer* pPlayer, GItem* pItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return IsItemActable(pPlayer, pItem, IAT_EQUIP);
}

bool GItemActableChecker::IsUnequipable( GEntityPlayer* pPlayer, GItem* pItem )
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return IsItemActable(pPlayer, pItem, IAT_UNEQUIP);
}


bool GItemActableChecker::IsUsable(GEntityPlayer* pPlayer, GItem* pItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return IsItemActable(pPlayer, pItem, IAT_USE);
}

bool GItemActableChecker::IsCraftable( GEntityPlayer* pPlayer, GItem* pItem )
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return IsItemActable(pPlayer, pItem, IAT_CRAFT);
}

bool GItemActableChecker::IsMovable(GEntityPlayer* pPlayer, GItem* pItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return IsItemActable(pPlayer, pItem, IAT_MOVE);
}

bool GItemActableChecker::IsDyeable(GEntityPlayer* pPlayer, GItem* pItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return IsItemActable(pPlayer, pItem, IAT_DYE);
}

bool GItemActableChecker::IsEnchantable(GEntityPlayer* pPlayer, GItem* pItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return IsItemActable(pPlayer, pItem, IAT_ENCHANT);
}

bool GItemActableChecker::IsRepairable(GEntityPlayer* pPlayer, GItem* pItem)
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	return IsItemActable(pPlayer, pItem, IAT_REPAIR);
}

bool GItemActableChecker::IsItemActable(GEntityPlayer* pPlayer, const GItem* pItem, ITEM_ACT_TYPE nAct) const
{
	if (NULL == pPlayer) return false;
	if (NULL == pItem) return false;

	GItemStateSelector itemStateSelector;
	ITEM_STATE_TYPE nState = itemStateSelector.Select(pPlayer, pItem);

	if (IAT_ENCHANT == nAct && IST_ENCNAHTING != nState) return false;
	if (IAT_ENCHANT != nAct && IST_ENCNAHTING == nState) return false;

	if (IAT_DYE == nAct && IST_DYEING != nState) return false;
	if (IAT_DYE != nAct && IST_DYEING == nState) return false;


	switch (nAct)
	{
	case IAT_CRAFT :
	case IAT_EQUIP :
		{
			if (IST_STORAGE == nState) return false;			
			if (IST_ARRANGED == nState) return false;
			if (IST_TRADING == nState) return false;
			if (IST_POST == nState) return false;
		}
		break;
	case IAT_UNEQUIP :
		{
			if (IST_STORAGE == nState) return false;			
			if (IST_TRADING == nState) return false;
			if (IST_POST == nState) return false;
		}
		break;	
	case IAT_TRADE :
		{
			if (IST_STORAGE == nState) return false;			
			if (IST_ARRANGED == nState) return false;
			if (IST_EQUIPED == nState) return false;			
			if (IST_POST == nState) return false;
		}
		break;
	case IAT_MAIL_APPEND :
		{
			if (IST_STORAGE == nState) return false;			
			if (IST_ARRANGED == nState) return false;
			if (IST_EQUIPED == nState) return false;
			if (IST_TRADING == nState) return false;
		}
		break;
	case IAT_DROP :
		{
			if (IST_ARRANGED == nState) return false;
			if (IST_EQUIPED == nState) return false;
			if (IST_TRADING == nState) return false;
			if (IST_POST == nState) return false;
		}
		break;
	case IAT_SELL :		
	case IAT_USE :
		{
			if (IST_STORAGE == nState) return false;			
			if (IST_ARRANGED == nState) return false;
			if (IST_EQUIPED == nState) return false;
			if (IST_TRADING == nState) return false;
			if (IST_POST == nState) return false;
		}
		break;
	case IAT_MOVE :
		{
			if (IST_POST == nState) return false;
			if (IST_EQUIPED == nState) return false;
			if (IST_ARRANGED == nState) return false;
			if (IST_TRADING == nState) return false;
		}
		break;
	case IAT_REPAIR	:
		{
			if (IST_STORAGE == nState) return false;
			if (IST_POST == nState) return false;		
			if (IST_TRADING == nState) return false;
		}
		break;
	}	

	return true;
}

