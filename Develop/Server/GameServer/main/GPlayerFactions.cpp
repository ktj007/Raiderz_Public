#include "StdAfx.h"
#include "GPlayerFactions.h"
#include "GPlayerFaction.h"
#include "GFactionInfoMgr.h"
#include "GFactionInfo.h"
#include "GSTLUtil.h"
#include "GDef.h"
#include "GGlobal.h"
#include "CSFactionCalculator.h"



GPlayerFactions::GPlayerFactions(void)
{
}

GPlayerFactions::~GPlayerFactions(void)
{
	Clear();
}

void GPlayerFactions::Clear()
{
	SAFE_DELETE_MAP(m_mapPlayerFaction);
}

GPlayerFaction* GPlayerFactions::Get(uint8 nID)
{	
	MAP_PLAYERFACTION::iterator itor = m_mapPlayerFaction.find(nID);
	if (m_mapPlayerFaction.end() == itor) return NULL;

	return (*itor).second;
}

void GPlayerFactions::Increase(uint8 nID, uint16 nQuantity)
{
	VALID(0 < nQuantity);
	GFactionInfo* pFactionInfo = gmgr.pFactionInfoMgr->Get(nID);
	VALID(pFactionInfo);

	GPlayerFaction* pPlayerFaction = Get(nID);
	if (NULL == pPlayerFaction)
	{
		pPlayerFaction = Insert(pFactionInfo, pFactionInfo->m_nDefaultQuantity);
	}
	VALID(pPlayerFaction);

	pPlayerFaction->Increase(nQuantity);
}

void GPlayerFactions::Decrease(uint8 nID, uint16 nQuantity)
{
	VALID(0 < nQuantity);
	GFactionInfo* pFactionInfo = gmgr.pFactionInfoMgr->Get(nID);
	VALID(pFactionInfo);

	GPlayerFaction* pPlayerFaction = Get(nID);
	if (NULL == pPlayerFaction)
	{
		pPlayerFaction = Insert(pFactionInfo, pFactionInfo->m_nDefaultQuantity);
	}
	VALID(pPlayerFaction);

	pPlayerFaction->Decrease(nQuantity);
}

GPlayerFaction* GPlayerFactions::Insert(GFactionInfo* pFactionInfo, uint16 nQuantity)
{
	VALID_RET(false == IsExist(pFactionInfo->m_nID), NULL);	

	GPlayerFaction* pNewPlayerFaction = new GPlayerFaction(pFactionInfo, nQuantity);

	m_mapPlayerFaction.insert(MAP_PLAYERFACTION::value_type(pFactionInfo->m_nID, pNewPlayerFaction));

	return pNewPlayerFaction;
}

void GPlayerFactions::GetAll(vector<GPlayerFaction*>& outvecPlayerFaction)
{
	for each (const MAP_PLAYERFACTION::value_type& val in m_mapPlayerFaction)
	{
		GPlayerFaction* pPlayerFaction =  val.second;
		if (NULL == pPlayerFaction) continue;

		outvecPlayerFaction.push_back(pPlayerFaction);
	}
}

bool GPlayerFactions::IsExist( uint8 nID )
{
	GPlayerFaction* pPlayerFaction = Get(nID);
	if (NULL == pPlayerFaction) return false;

	return true;
}

uint16 GPlayerFactions::GetQuantity(uint8 nID)
{
	GPlayerFaction* pPlayerFaction = Get(nID);
	if (NULL != pPlayerFaction)	return pPlayerFaction->GetQuantity();

	GFactionInfo* pFactionInfo = gmgr.pFactionInfoMgr->Get(nID);
	if (NULL != pFactionInfo) return pFactionInfo->m_nDefaultQuantity;

	return 0;
}

uint16 GPlayerFactions::GetQuantityPercent(uint8 nID)
{
	return GetQuantity(nID) * 100 / CSFactionCalculator::GetMaxQuantity();
}

FACTION_RELATION_TYPE GPlayerFactions::GetRelation(uint8 nID)
{
	if (0 == nID) return FRT_NONE;

	GPlayerFaction* pPlayerFaction = Get(nID);
	if (NULL != pPlayerFaction)	return pPlayerFaction->GetRelation();

	GFactionInfo* pFactionInfo = gmgr.pFactionInfoMgr->Get(nID);
	if (NULL != pFactionInfo) return pFactionInfo->m_nDefaultFRT;

	return FRT_NONE;
}