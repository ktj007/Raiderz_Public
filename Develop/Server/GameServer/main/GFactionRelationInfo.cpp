#include "StdAfx.h"
#include "GFactionRelationInfo.h"
#include "CSFactionInfo.h"

GFactionRelationInfo::GFactionRelationInfo(uint8 nID1, uint8 nID2, FACTION_RELATION_TYPE nFRT)
: m_nID1(nID1)
, m_nID2(nID2)
, m_nFRT(nFRT)
{
}
