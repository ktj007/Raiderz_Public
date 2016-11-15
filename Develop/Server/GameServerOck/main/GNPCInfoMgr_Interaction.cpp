#include "stdafx.h"
#include "GNPCInfoMgr.h"
#include "CSDef_Interaction.h"
#include "GStrings.h"
#include "GNPCInteractionInfo.h"
#include "GGlobal.h"
#include "GConditionsInfoMgr.h"
#include "GNPCInfo.h"

void GNPCInfoMgr::ParseInteraction(MXmlElement* pElement, GNPCInfo* pNewNPCInfo)
{
	ParseInteractionForClick(pElement, pNewNPCInfo);
	ParseInteractionForItem(pElement, pNewNPCInfo);	
}

void GNPCInfoMgr::ParseInteractionForClick( MXmlElement* pElement, GNPCInfo* pNewNPCInfo )
{
	for (int i = CLICK_INTERACTION_BEGIN_ID; MAX_CLICK_INTERACTION_SIZE >= i; i++)
	{
		GNPCInteractionElementInfo* pNewIElementInfo = ParseIElementCommon(pElement, i, ITRT_CLICK);
		if (NULL == pNewIElementInfo) continue;

		pNewNPCInfo->m_InteractionInfo.m_vecNPCIEInfo.push_back(pNewIElementInfo);
	}
	if(1 < pNewNPCInfo->m_InteractionInfo.m_vecNPCIEInfo.size())
	{
		pNewNPCInfo->m_InteractionInfo.SortByInteractionType();
	}
}

void GNPCInfoMgr::ParseInteractionForItem(MXmlElement* pElement, GNPCInfo* pNewNPCInfo)
{	
	/*for (int i = ITEM_INTERACTION_BEGIN_ID; MAX_CLICK_INTERACTION_SIZE >= i; i++)
	{		
		GNPCInteractionElementInfo*  pNewIElementInfo = ParseIElementCommon(pElement, i, ITRT_ITEM);
		if (NULL == pNewIElementInfo) continue;

		char szIItemID[512];
		char szITarCond[512];
		sprintf_s(szIItemID, 512, "%s%d", NPC_XML_ATTR_IITEMID, i);
		sprintf_s(szITarCond, 512, "%s%d", NPC_XML_ATTR_IITEMTARGETCOND, i);

		_Attribute(&pNewIElementInfo->m_nUseItemID,	pElement, szIItemID);

		wstring strValue;
		if (true == _Attribute(strValue, pElement, szITarCond))
		{
			pNewIElementInfo->m_nTargetCondition = static_cast<ITARGET_CONDITION_TYPE>(GStrings::StringToITargetConditionType(strValue.c_str()));
		}

		pNewNPCInfo->m_InteractionInfo.m_vecNPCIEInfo.push_back(pNewIElementInfo);
	}*/


	int nICond = 0;
	_Attribute(&nICond,	pElement, NPC_XML_ATTR_IITEMCOND);
	wstring strValue;
	if (false == _Attribute(strValue, pElement, NPC_XML_ATTR_IITEMTYPE)) return;	
	INTERACTION_TYPE nIET = static_cast<INTERACTION_TYPE>(GStrings::StringToIElementType(strValue.c_str()));
	if (IT_NONE == nIET) return;

	GNPCInteractionElementInfo* pNewIElementInfo = new GNPCInteractionElementInfo();
	pNewIElementInfo->m_nID = ITEM_INTERACTION_BEGIN_ID;
	pNewIElementInfo->m_pConditionsInfo = gmgr.pCondtionsInfoMgr->Get(nICond);
	pNewIElementInfo->m_nType = nIET;
	if (true == _Attribute(strValue, pElement, NPC_XML_ATTR_IITEMACT))
	{
		CSStrings::StringNumbersToIntVector(strValue, pNewIElementInfo->m_vecAct);
	}
	
	_Attribute(&pNewIElementInfo->m_nUseItemID,	pElement, NPC_XML_ATTR_IITEMID);

	if (true == _Attribute(strValue, pElement, NPC_XML_ATTR_IITEMTARGETCOND))
	{
		pNewIElementInfo->m_nTargetCondition = static_cast<ITARGET_CONDITION_TYPE>(GStrings::StringToITargetConditionType(strValue.c_str()));
	}

	pNewNPCInfo->m_InteractionInfo.m_vecNPCIEInfo.push_back(pNewIElementInfo);
}

GNPCInteractionElementInfo* GNPCInfoMgr::ParseIElementCommon( MXmlElement* pElement, int i, ITRIGGER_TYPE nTriggerTyp )
{
	char szICond[512];
	char szIType[512];
	char szIAct[512];
	if (ITRT_CLICK == nTriggerTyp)
	{
		sprintf_s(szICond, 512, "%s%d", NPC_XML_ATTR_ICOND, i);
		sprintf_s(szIType, 512, "%s%d", NPC_XML_ATTR_ITYPE, i);
		sprintf_s(szIAct, 512, "%s%d", NPC_XML_ATTR_IACT, i);
	}
	else if (ITRT_ITEM == nTriggerTyp)
	{
		sprintf_s(szICond, 512, "%s%d", NPC_XML_ATTR_IITEMCOND, i);
		sprintf_s(szIType, 512, "%s%d", NPC_XML_ATTR_IITEMTYPE, i);
		sprintf_s(szIAct, 512, "%s%d", NPC_XML_ATTR_IITEMACT, i);
	}
	else
	{
		return NULL;
	}

	int nICond = 0;
	_Attribute(&nICond,	pElement, szICond);
	wstring strValue;
	if (false == _Attribute(strValue, pElement, szIType)) return NULL;	
	INTERACTION_TYPE nIET = static_cast<INTERACTION_TYPE>(GStrings::StringToIElementType(strValue.c_str()));
	if (IT_NONE == nIET) return NULL;	
	
	GNPCInteractionElementInfo* pNewIElementInfo = new GNPCInteractionElementInfo();
	pNewIElementInfo->m_nID = i;
	pNewIElementInfo->m_pConditionsInfo = gmgr.pCondtionsInfoMgr->Get(nICond);
	pNewIElementInfo->m_nType = nIET;
	if (true == _Attribute(strValue, pElement, szIAct))
	{
		CSStrings::StringNumbersToIntVector(strValue, pNewIElementInfo->m_vecAct);
	}

	return pNewIElementInfo;
}
