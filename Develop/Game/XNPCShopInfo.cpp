#include "StdAfx.h"
#include "XNPCShopInfo.h"

void XNPCShopInfo::Clear()
{
	m_vecBuyableUsableItemID.clear();
	m_vecBuyableUnusableItemID.clear();
	m_vecUnbuyableItemID.clear();

	m_vecNPCShopItem.clear();

	m_fBuyMod	= 0.f;
	m_fSellMod	= 0.f;
	m_bRepairer = false;
}

bool XNPCShopInfo::IsBuyable( int nID)
{
	size_t _size = m_vecUnbuyableItemID.size();
	for ( size_t i = 0;  i < _size;  i++)
	{
		if (nID == m_vecUnbuyableItemID[i])
			return false;
	}

	return true;
}

bool XNPCShopInfo::IsUsable( int nID)
{
	size_t _size = m_vecBuyableUsableItemID.size();
	for ( size_t i = 0;  i < _size;  i++)
	{
		if (nID == m_vecBuyableUsableItemID[i])
			return true;
	}

	return false;
}
