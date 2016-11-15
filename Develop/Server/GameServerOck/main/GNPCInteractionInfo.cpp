#include "StdAfx.h"
#include "GNPCInteractionInfo.h"
#include "GConditionsInfo.h"
#include "GConditionInfo.h"
#include "GSTLUtil.h"
#include "CSDef_Interaction.h"

bool IsIEGatherType(INTERACTION_TYPE eType)
{
	if (IT_GATHER_1 <= eType && eType <= IT_GATHER_10) return true;
	return false;
}

GNPCInteractionElementInfo::GNPCInteractionElementInfo()
: m_nID(0)
, m_pConditionsInfo(NULL)
, m_nType(IT_NONE)
, m_nTargetCondition(ITCT_LIVE)
, m_nUseItemID(0)
{
}

GNPCInteractionElementInfo::~GNPCInteractionElementInfo()
{
}

int GNPCInteractionElementInfo::GetSinlgeAct()
{
	if (m_vecAct.empty()) return 0;

	return m_vecAct.front();
}

ITRIGGER_TYPE GNPCInteractionElementInfo::GetTriggerType()
{
	if (0 != m_nUseItemID) return ITRT_ITEM;

	return ITRT_CLICK;
}

GNPCInteractionInfo::GNPCInteractionInfo()
{

}

GNPCInteractionInfo::~GNPCInteractionInfo()
{
	Clear();
}

void GNPCInteractionInfo::Clear()
{
	SAFE_DELETE_VECTOR(m_vecNPCIEInfo);
}

bool GNPCInteractionInfo::HasCondition(CONDITION_TYPE nConditionType)
{
	for each (GNPCInteractionElementInfo* pIElementInfo in m_vecNPCIEInfo)
	{
		if (NULL == pIElementInfo) continue;
		GConditionsInfo* pConditionsInfo = pIElementInfo->m_pConditionsInfo;
		if (NULL == pConditionsInfo) continue;

		if (true == pConditionsInfo->HasCondition(nConditionType))
		{
			return true;
		}
	}

	return false;
}

bool GNPCInteractionInfo::HasQuestIE()
{
	if (HasIE(IT_QUEST_BEGIN)) return true;
	if (HasIE(IT_QUEST_END)) return true;

	return false;
}


bool GNPCInteractionInfo::HasIE(INTERACTION_TYPE nIEType)
{
	for each (GNPCInteractionElementInfo* pIElementInfo in m_vecNPCIEInfo)
	{
		if (NULL == pIElementInfo) continue;
		if (nIEType != pIElementInfo->m_nType) continue;

		return true;		
	}

	return false;
}

bool ByElementType(GNPCInteractionElementInfo* pA, GNPCInteractionElementInfo* pB)
{
	return (pA->m_nType > pB->m_nType);
}

void GNPCInteractionInfo::SortByInteractionType()
{
	std::sort(m_vecNPCIEInfo.begin(), m_vecNPCIEInfo.end(), ByElementType);
}