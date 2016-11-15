#include "StdAfx.h"
#include "GPlayerFaction.h"
#include "GGlobal.h"
#include "GFactionRelationInfoMgr.h"
#include "GFactionRelationInfo.h"
#include "CSFactionCalculator.h"
#include "GFactionInfo.h"

GPlayerFaction::GPlayerFaction(GFactionInfo* pInfo, uint16 nQuantity)
: m_pInfo(pInfo)
, m_nQuantity(nQuantity)
{

}

void GPlayerFaction::Increase(uint16 nQuantity)
{
	if (CSFactionCalculator::GetMaxQuantity() - nQuantity <= m_nQuantity)
	{
		m_nQuantity = CSFactionCalculator::GetMaxQuantity();
	}
	else
	{
		m_nQuantity += nQuantity;
	}
}

void GPlayerFaction::Decrease(uint16 nQuantity)
{
	if (nQuantity >= m_nQuantity)
	{
		m_nQuantity = 0;
	}
	else
	{
		m_nQuantity -= nQuantity;
	}
}

uint16 GPlayerFaction::GetQuantity()
{
	return m_nQuantity;
}

FACTION_RELATION_TYPE GPlayerFaction::GetRelation()
{	
	return CSFactionCalculator::CalculRelation(m_nQuantity);
}

GFactionInfo* GPlayerFaction::GetInfo()
{
	return m_pInfo;
}

uint8 GPlayerFaction::GetID()
{
	return m_pInfo->m_nID;
}