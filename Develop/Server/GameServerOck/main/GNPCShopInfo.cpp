#include "StdAfx.h"
#include "GNPCShopInfo.h"

GNPCShopInfo::GNPCShopInfo()
: m_nID(0)
, m_fBaseBuyMod(1.0f)
, m_fBaseSellMod(1.0f)
, m_nFactionID(0)
, m_bRepairable(false)
, m_pDialogInfo(NULL)
{

}

GNPCShopInfo::~GNPCShopInfo()
{
}

GConditionsInfo* GNPCShopInfo::GetConditionsInfo(GItemData* pItemData)
{
	map<GItemData*, GConditionsInfo*>::iterator itor = m_mapConditionsItem.find(pItemData);

	if (m_mapConditionsItem.end() == itor) return NULL;

	return (*itor).second;
}

bool GNPCShopInfo::HasFaction()
{
	if (0 == m_nFactionID) return false;

	return true;
}