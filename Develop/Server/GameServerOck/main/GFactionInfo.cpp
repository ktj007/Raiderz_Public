#include "StdAfx.h"
#include "GFactionInfo.h"
#include "GFactionRelationInfoMgr.h"
#include "GGlobal.h"
#include "CSFactionCalculator.h"

GFactionInfo::GFactionInfo(uint8 nID, uint16 nDefaultQuantity)
: m_nID(nID)
, m_nDefaultQuantity(nDefaultQuantity)
, m_nDefaultFRT(CSFactionCalculator::CalculRelation(nDefaultQuantity))
{
	
}