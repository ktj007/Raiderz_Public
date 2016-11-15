#include "StdAfx.h"
#include "GPaletteItem.h"

GPaletteItem::GPaletteItem()
: m_nType(PIT_NONE)
, m_nItemIDorTalentID(0)
{
}

GPaletteItem::~GPaletteItem()
{
}

bool GPaletteItem::IsEmpty()
{
	if (PIT_NONE == m_nType && 0 == m_nItemIDorTalentID) return true;

	return false;
}