#include "StdAfx.h"
#include "CSConditionsInfoMgr.h"
#include "CSConditionsInfo.h"
#include "CSConditionInfo.h"
#include "CSValidateLogger.h"
#include "CSChrInfo.h"
#include "CSStrings.h"
#include "MLocale.h"
#include "CSFactionInfo.h"
#include "CSFactionCalculator.h"


CSConditionsInfoMgr::CSConditionsInfoMgr()
{
}

CSConditionsInfoMgr::~CSConditionsInfoMgr()
{
	Clear();
}

void CSConditionsInfoMgr::Clear()
{
	for each(pair<int, CSConditionsInfo*> pair in m_mapConditionsInfo)
	{
		delete pair.second;
	}

	m_mapConditionsInfo.clear();
}

bool CSConditionsInfoMgr::Load(const TCHAR* szFileName)
{
	_ASSERT(NULL != szFileName);
	if (NULL == szFileName) return false;


	MXml xml;
	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(CONDITIONS_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), CONDITIONS_XML_TAG_CONDITIONS))
		{
			ParseConditions(pElement, &xml);
		}
	}

	return true;
}

void CSConditionsInfoMgr::ParseConditions( MXmlElement* pElement, MXml* pXml )
{
	int nCondtionsID;
	_Attribute(&nCondtionsID, pElement, CONDITIONS_XML_ATTR_ID);

	const CSConditionsInfo* pExistInfo = Get(nCondtionsID);
	if (NULL != pExistInfo)
	{
		return;	
	}

	CSConditionsInfo* pNewCondtionsInfo = New_ConditionsInfo();
	pNewCondtionsInfo->Create();
	pNewCondtionsInfo->m_nID = nCondtionsID;

	_Attribute(&pNewCondtionsInfo->m_bValidation, pElement, CONDITIONS_XML_ATTR_VALIDATION);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement())
	{	
		if (!_stricmp(pChild->Value(), CONDITIONS_XML_TAG_AND))
		{
			ParseAnd(pChild, pXml, pNewCondtionsInfo, pNewCondtionsInfo->m_pConditionComposite->m_vecConditionComponent);
		}
		else if (!_stricmp(pChild->Value(), CONDITIONS_XML_TAG_OR))
		{
			ParseOr(pChild, pXml, pNewCondtionsInfo, pNewCondtionsInfo->m_pConditionComposite->m_vecConditionComponent);
		}
		else if (!_stricmp(pChild->Value(), CONDITIONS_XML_TAG_CONDITION))
		{
			ParseCondition(pChild, pXml, pNewCondtionsInfo, pNewCondtionsInfo->m_pConditionComposite->m_vecConditionComponent);
		}
	}

	Insert(pNewCondtionsInfo);
}

void CSConditionsInfoMgr::ParseAnd(MXmlElement* pElement, MXml* pXml, CSConditionsInfo* pConditionsInfo, vector<CSCondition_Component*>& vecConditionComponent)
{
	CSCondition_Composite* pConditionComponent = New_Condition_Composite(CAO_AND);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), CONDITIONS_XML_TAG_AND))
		{
			ParseAnd(pChild, pXml, pConditionsInfo, pConditionComponent->m_vecConditionComponent);
		}
		else if (!_stricmp(pChild->Value(), CONDITIONS_XML_TAG_OR))
		{
			ParseOr(pChild, pXml, pConditionsInfo, pConditionComponent->m_vecConditionComponent);
		}
		else if (!_stricmp(pChild->Value(), CONDITIONS_XML_TAG_CONDITION))
		{
			ParseCondition(pChild, pXml, pConditionsInfo, pConditionComponent->m_vecConditionComponent);
		}
	}

	vecConditionComponent.push_back(pConditionComponent);
}

void CSConditionsInfoMgr::ParseOr(MXmlElement* pElement, MXml* pXml, CSConditionsInfo* pConditionsInfo, vector<CSCondition_Component*>& vecConditionComponent)
{
	CSCondition_Composite* pConditionComponent = New_Condition_Composite(CAO_OR);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), CONDITIONS_XML_TAG_AND))
		{
			ParseAnd(pChild, pXml, pConditionsInfo, pConditionComponent->m_vecConditionComponent);
		}
		else if (!_stricmp(pChild->Value(), CONDITIONS_XML_TAG_OR))
		{
			ParseOr(pChild, pXml, pConditionsInfo, pConditionComponent->m_vecConditionComponent);
		}
		else if (!_stricmp(pChild->Value(), CONDITIONS_XML_TAG_CONDITION))
		{
			ParseCondition(pChild, pXml, pConditionsInfo, pConditionComponent->m_vecConditionComponent);
		}
	}

	vecConditionComponent.push_back(pConditionComponent);
}

void CSConditionsInfoMgr::ParseCondition( MXmlElement* pElement, MXml* pXml, CSConditionsInfo* pConditionsInfo, vector<CSCondition_Component*>& vecConditionComponent)
{
	tstring strValue;
	if (false == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_TYPE)) return;
	
	CSCondition_Leaf* pConditionLeaf = New_Condition_Leaf();
	int nConditionType = CSStrings::StringToConditionType(strValue.c_str());
	pConditionLeaf->m_nCT = static_cast<CONDITION_TYPE>(nConditionType);

	switch (pConditionLeaf->m_nCT)
	{
	case CT_LEVEL:		ParseCondition_Level(pElement, pConditionLeaf);		break;
	case CT_QUEST:		ParseCondition_Quest(pElement, pConditionLeaf);		break;
	case CT_QUEST_VAR:	ParseCondition_QuestVar(pElement, pConditionLeaf);	break;
	case CT_QUEST_OBJECTIVE_COMPLETE:	ParseCondition_QuestObjective_Complete(pElement, pConditionLeaf);	break;
	case CT_QUEST_OBJECTIVE_INCOMPLETE:	ParseCondition_QuestObjective_Incomplete(pElement, pConditionLeaf);	break;
	case CT_ITEM:		ParseCondition_Item(pElement, pConditionLeaf);		break;
	case CT_NOITEM:		ParseCondition_NoItem(pElement, pConditionLeaf);		break;
	case CT_FEWITEM:	ParseCondition_FewItem(pElement, pConditionLeaf);		break;
	case CT_MANYITEM:	ParseCondition_ManyItem(pElement, pConditionLeaf);		break;
	case CT_EQUIP:		ParseCondition_Equip(pElement, pConditionLeaf);		break;		
	case CT_RACE:		ParseCondition_Race(pElement, pConditionLeaf);		break;
	case CT_SEX:		ParseCondition_Sex(pElement, pConditionLeaf);		break;
	case CT_FACTION:	ParseCondition_Faction(pElement, pConditionLeaf);	break;
	case CT_TITLE:		ParseCondition_Title(pElement, pConditionLeaf);		break;
	case CT_DATE:		ParseCondition_Date(pElement, pConditionLeaf);		break;
	case CT_TALENT:		ParseCondition_Talent(pElement, pConditionLeaf);		break;
	case CT_TALENT_STYLE_TP:	ParseCondition_Talent_Style_TP(pElement, pConditionLeaf);	break;
	case CT_KILL:		ParseCondition_Kill(pElement, pConditionLeaf);		break;
	case CT_QPVP_TEAM:	ParseCondition_QPVP_Team(pElement, pConditionLeaf);	break;
	default:			break;
	}

	pConditionsInfo->m_setUsedConditionType.insert(pConditionLeaf->m_nCT);
	vecConditionComponent.push_back(pConditionLeaf);
}

void CSConditionsInfoMgr::ParseCondition_Level(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	int nMinLevel;	
	if (true == _Attribute(&nMinLevel, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		pConditionLeaf->m_vecParam1.push_back(nMinLevel);
	}

	int nMaxLevel;
	if (true == _Attribute(&nMaxLevel, pElement, CONDITIONS_XML_ATTR_PARAM2))
	{
		pConditionLeaf->m_vecParam2.push_back(nMaxLevel);
	}
}

void CSConditionsInfoMgr::ParseCondition_Quest(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	int nQuestID;
	if (true == _Attribute(&nQuestID, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		pConditionLeaf->m_vecParam1.push_back(nQuestID);
	}

	tstring strValue;	
	if(true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM2))
	{
		CONDITION_QUEST_STATE nQuestState = static_cast<CONDITION_QUEST_STATE>(CSStrings::StringToConditionQuestState(strValue.c_str()));
		pConditionLeaf->m_vecParam2.push_back(nQuestState);
	}
}

void CSConditionsInfoMgr::ParseCondition_QuestVar(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;	

	int nQuestID;
	if (true == _Attribute(&nQuestID, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		pConditionLeaf->m_vecParam1.push_back(nQuestID);
	}

	int nQuestVar;
	if (true == _Attribute(&nQuestVar, pElement, CONDITIONS_XML_ATTR_PARAM2))
	{
		pConditionLeaf->m_vecParam2.push_back(nQuestVar);
	}
}

void CSConditionsInfoMgr::ParseCondition_QuestObjective_Complete(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	int nQuestID;
	if (true == _Attribute(&nQuestID, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		pConditionLeaf->m_vecParam1.push_back(nQuestID);
	}

	int nQuestObjectiveID;
	if(true == _Attribute(&nQuestObjectiveID, pElement, CONDITIONS_XML_ATTR_PARAM2))
	{
		pConditionLeaf->m_vecParam2.push_back(nQuestObjectiveID);
	}
}

void CSConditionsInfoMgr::ParseCondition_QuestObjective_Incomplete(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	int nQuestID;
	if (true == _Attribute(&nQuestID, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		pConditionLeaf->m_vecParam1.push_back(nQuestID);
	}

	int nQuestObjectiveID;
	if(true == _Attribute(&nQuestObjectiveID, pElement, CONDITIONS_XML_ATTR_PARAM2))
	{
		pConditionLeaf->m_vecParam2.push_back(nQuestObjectiveID);
	}
}

void CSConditionsInfoMgr::ParseCondition_Item(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	tstring strValue;
	if (true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		CSStrings::StringNumbersToIntVector(strValue, pConditionLeaf->m_vecParam1);
	}
}

void CSConditionsInfoMgr::ParseCondition_NoItem(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	tstring strValue;
	if (true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		CSStrings::StringNumbersToIntVector(strValue, pConditionLeaf->m_vecParam1);
	}
}

void CSConditionsInfoMgr::ParseCondition_FewItem( MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	tstring strValue;
	if (true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		CSStrings::StringNumbersToIntVector(strValue, pConditionLeaf->m_vecParam1);
	}

	if (true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM2))
	{
		CSStrings::StringNumbersToIntVector(strValue, pConditionLeaf->m_vecParam2);
	}
}

void CSConditionsInfoMgr::ParseCondition_ManyItem( MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	tstring strValue;
	if (true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		CSStrings::StringNumbersToIntVector(strValue, pConditionLeaf->m_vecParam1);
	}

	if (true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM2))
	{
		CSStrings::StringNumbersToIntVector(strValue, pConditionLeaf->m_vecParam2);
	}
}

void CSConditionsInfoMgr::ParseCondition_Equip(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	tstring strValue;
	if (true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		CSStrings::StringNumbersToIntVector(strValue, pConditionLeaf->m_vecParam1);
	}
}

void CSConditionsInfoMgr::ParseCondition_Race(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	tstring strValue;
	if(true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		RACE nRace = static_cast<RACE>(CSStrings::StringToRace(strValue.c_str()));
		pConditionLeaf->m_vecParam1.push_back(nRace);
	}
}

void CSConditionsInfoMgr::ParseCondition_Sex(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	tstring strValue;
	if(true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		SEX nSex = static_cast<SEX>(CSStrings::StringToSex(strValue.c_str()));
		pConditionLeaf->m_vecParam1.push_back(nSex);
	}
}

void CSConditionsInfoMgr::ParseCondition_Faction(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	int nFactionID;
	if (true == _Attribute(&nFactionID, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		pConditionLeaf->m_vecParam1.push_back(nFactionID);
	}

	tstring strValue;
	if (true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM2))
	{
		int nQuantity = _tstoi(strValue.c_str());
		if (0 == nQuantity)
		{
			FACTION_RELATION_TYPE nFRT = static_cast<FACTION_RELATION_TYPE>(CSStrings::StringToFactionRelationType(strValue.c_str()));
			pConditionLeaf->m_vecParam2.push_back(CSFactionCalculator::GetQuantity(nFRT));
		}
		else
		{
			pConditionLeaf->m_vecParam2.push_back(nQuantity);
		}		
	}
}

void CSConditionsInfoMgr::ParseCondition_Title(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	tstring strValue;
	if (true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		CSStrings::StringNumbersToIntVector(strValue, pConditionLeaf->m_vecParam1);
	}
}

void CSConditionsInfoMgr::ParseCondition_Date(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	int nStartTime;
	if (true == _Attribute(&nStartTime, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		pConditionLeaf->m_vecParam1.push_back(nStartTime);
	}

	int nEndTime;
	if (true == _Attribute(&nEndTime, pElement, CONDITIONS_XML_ATTR_PARAM2))
	{
		pConditionLeaf->m_vecParam2.push_back(nEndTime);
	}
}

void CSConditionsInfoMgr::ParseCondition_Talent(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	tstring strValue;
	if (true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		CSStrings::StringNumbersToIntVector(strValue, pConditionLeaf->m_vecParam1);
	}
}

void CSConditionsInfoMgr::ParseCondition_Talent_Style_TP(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	tstring strValue;	
	if (true == _Attribute(strValue, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		TALENT_STYLE nTalentStyle = static_cast<TALENT_STYLE>(CSStrings::StringToTalentStyle(strValue.c_str()));
		pConditionLeaf->m_vecParam1.push_back(nTalentStyle);
	}

	int nTP;
	if (true == _Attribute(&nTP, pElement, CONDITIONS_XML_ATTR_PARAM2))
	{
		pConditionLeaf->m_vecParam2.push_back(nTP);
	}
}

void CSConditionsInfoMgr::ParseCondition_Kill(MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf)
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	int nNPCID;
	if (true == _Attribute(&nNPCID, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		pConditionLeaf->m_vecParam1.push_back(nNPCID);
	}
}

void CSConditionsInfoMgr::ParseCondition_QPVP_Team( MXmlElement* pElement, CSCondition_Leaf* pConditionLeaf )
{
	if (NULL == pElement) return;
	if (NULL == pConditionLeaf) return;

	int nTeam;
	if (true == _Attribute(&nTeam, pElement, CONDITIONS_XML_ATTR_PARAM1))
	{
		pConditionLeaf->m_vecParam1.push_back(nTeam);
	}
}

void CSConditionsInfoMgr::Insert(CSConditionsInfo* pConditionInfo)
{
	_ASSERT(NULL != pConditionInfo);
	if (NULL == pConditionInfo) return;

	m_mapConditionsInfo.insert(MAP_CONDITIONSINFO::value_type(pConditionInfo->m_nID, pConditionInfo));
}

CSConditionsInfo* CSConditionsInfoMgr::Get(int nID)
{
	if (true == m_mapConditionsInfo.empty()) return NULL;

	MAP_CONDITIONSINFO::iterator itor = m_mapConditionsInfo.find(nID);
	if (m_mapConditionsInfo.end() == itor) return NULL;

	return (*itor).second;
}

bool CSConditionsInfoMgr::IsConditionsID( int nID )
{
	if (NULL == Get(nID)) return false;

	return true;
}

MAP_CONDITIONSINFO& CSConditionsInfoMgr::GetContainer()
{
	return m_mapConditionsInfo;
}