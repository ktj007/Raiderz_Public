#include "StdAfx.h"
#include "GCraftInfo.h"

GCraftInfo::GCraftInfo()
: m_nID(0)
, m_nDialogID(0)
, m_fBaseMakeMod(1.0f)
, m_nFactionID(0)
{
}

GCraftInfo::~GCraftInfo()
{
}

bool GCraftInfo::IsHaveRecipeID( int nRecipeID )
{
	if (m_setRecipeID.end() == m_setRecipeID.find(nRecipeID)) return false;

	return true;
}