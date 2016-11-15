#include "StdAfx.h"
#include "GConditionsInfoValidator.h"
#include "GStrings.h"
#include "GServerSystems.h"
#include "GConditionsInfoMgr.h"
#include "GItemConditionsInfoMgr.h"
#include "GConditionsInfo.h"
#include "GConditionInfo.h"
#include "GValidateLogger.h"

bool GConditionsInfoValidator::Check()
{
	if (gmgr.pCondtionsInfoMgr == NULL)	return true;
	if (gmgr.pItemConditionsInfoMgr == NULL)	return true;

	_VLI(FILENAME_CONDITIONS);
	MAP_CONDITIONSINFO& mapConditionsInfo = gmgr.pCondtionsInfoMgr->GetContainer();
	for each (pair<int, CSConditionsInfo*> pairConditionsInfo in mapConditionsInfo)
	{
		GConditionsInfo* pConditionsInfo = dynamic_cast<GConditionsInfo*>(pairConditionsInfo.second);
		if (NULL == pConditionsInfo) continue;

		CheckConditions(pConditionsInfo);
	}	

	_VLI(FILENAME_CONDITIONS_ITEM);
	MAP_CONDITIONSINFO& mapItemConditionsInfo = gmgr.pItemConditionsInfoMgr->GetContainer();
	for each (pair<int, CSConditionsInfo*> pairConditionsInfo in mapItemConditionsInfo)
	{
		GConditionsInfo* pConditionsInfo = dynamic_cast<GConditionsInfo*>(pairConditionsInfo.second);
		if (NULL == pConditionsInfo) continue;

		CheckConditions(pConditionsInfo);
	}	

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GConditionsInfoValidator::CheckConditions(GConditionsInfo* pConditionsInfo)
{
	if (false == pConditionsInfo->m_bValidation) return;

	_VLE(_T(CONDITIONS_XML_TAG_CONDITIONS));
	_VLA(_T(CONDITIONS_XML_ATTR_ID), pConditionsInfo->m_nID);
	_VLP;

	_VLOGGER->ValidateID(pConditionsInfo->m_nID);

	for each (CSCondition_Component* pConditionComponent in pConditionsInfo->m_pConditionComposite->m_vecConditionComponent)
	{
		switch (pConditionComponent->GetType())
		{
		case CCT_LEAF:
			{
				CheckLeaf(dynamic_cast<GCondition_Leaf*>(pConditionComponent));
			}
			break;
		case CCT_COMPOSITE:
			{
				CheckComposite(dynamic_cast<GCondition_Composite*>(pConditionComponent));
			}
			break;
		}
	}
}

void GConditionsInfoValidator::CheckLeaf(GCondition_Leaf* pConditionLeaf)
{
	if (NULL == pConditionLeaf)	return;

	wstring strValue;
	_VLE(_T(CONDITIONS_XML_TAG_CONDITION));
	_VLA(_T(CONDITIONS_XML_ATTR_TYPE), GStrings::ConditionTypeToString(pConditionLeaf->m_nCT));
	GStrings::IntVectorToStringNumbers(pConditionLeaf->m_vecParam1, strValue);
	if (false == strValue.empty())
	{
		_VLA(_T(CONDITIONS_XML_ATTR_PARAM1), strValue);
	}		
	GStrings::IntVectorToStringNumbers(pConditionLeaf->m_vecParam2, strValue);
	if (false == strValue.empty())
	{
		_VLA(_T(CONDITIONS_XML_ATTR_PARAM2), strValue);
	}
	_VLP;

	if (pConditionLeaf->m_vecParam1.empty())
	{
		_VLOGGER->Log(_T(CONDITIONS_XML_ATTR_PARAM1) IS_EMPTY);
		return;
	}

	_VLOGGER->ValidateConditionType(pConditionLeaf->m_nCT, _T(CONDITIONS_XML_ATTR_TYPE));

	switch (pConditionLeaf->m_nCT)
	{
	case CT_LEVEL:
		{
			// 생략 시, 기본값
			int min = 0;
			int max = 100;

			if (!pConditionLeaf->m_vecParam1.empty())
			{
				min = pConditionLeaf->m_vecParam1[0];
			}

			if (!pConditionLeaf->m_vecParam2.empty())
			{
				max = pConditionLeaf->m_vecParam2[0];
			}

			_VLOGGER->ValidateBiggerThan(min, max, _T(CONDITIONS_XML_ATTR_PARAM1), _T(CONDITIONS_XML_ATTR_PARAM2));
		}
		break;
	case CT_QUEST:
		{
			int nQuestID = 0;

			if (!pConditionLeaf->m_vecParam1.empty())
			{
				nQuestID = pConditionLeaf->m_vecParam1[0];
			}
			
			_VLOGGER->ValidateQuestID(nQuestID, _T(CONDITIONS_XML_ATTR_PARAM1));
			

			int nState = -1;

			if (!pConditionLeaf->m_vecParam2.empty())
			{
				nState = pConditionLeaf->m_vecParam2[0];
			}
			
			_VLOGGER->ValidateConditionQuestState(nState, _T(CONDITIONS_XML_ATTR_PARAM2));			
		}
		break;
	case CT_QUEST_VAR:
		{
			int nQuestID = 0;

			if (!pConditionLeaf->m_vecParam1.empty())
			{
				nQuestID = pConditionLeaf->m_vecParam1[0];
			}

			_VLOGGER->ValidateQuestID(nQuestID, _T(CONDITIONS_XML_ATTR_PARAM1));
		}
		break;
	case CT_ITEM:
	case CT_NOITEM:
	case CT_FEWITEM:
	case CT_MANYITEM:
	case CT_EQUIP:
		{
			for each (int nItemID in pConditionLeaf->m_vecParam1)
			{
				_VLOGGER->ValidateItemID(nItemID, _T(CONDITIONS_XML_ATTR_PARAM1));
			}
		}
		break;
	case CT_RACE:
		{
			_VLOGGER->ValidateRace(pConditionLeaf->m_vecParam1[0], _T(CONDITIONS_XML_ATTR_PARAM1));	
		}
		break;
	case CT_SEX:
		{
			_VLOGGER->ValidateSex(pConditionLeaf->m_vecParam1[0], _T(CONDITIONS_XML_ATTR_PARAM1));				
		}
		break;
	case CT_FACTION:
		{				
		}
		break;
	case CT_TITLE:
		{

		}
		break;
	case CT_DATE:
		{
			if (false == pConditionLeaf->m_vecParam1.empty() &&
				false == pConditionLeaf->m_vecParam2.empty())
			{
				if ((0 <= pConditionLeaf->m_vecParam1[0] - 10000 && 0 > pConditionLeaf->m_vecParam2[0] - 10000) ||
					(0 <= pConditionLeaf->m_vecParam2[0] - 10000 && 0 > pConditionLeaf->m_vecParam1[0] - 10000))
				{
					_VLOGGER->LogNotEqualFormat(CONDITION_BEGIN_DATE, CONDITION_END_DATE);
				}
				else if (pConditionLeaf->m_vecParam1[0] > pConditionLeaf->m_vecParam2[0])
				{
					_VLOGGER->ValidateBiggerThan(pConditionLeaf->m_vecParam1[0], pConditionLeaf->m_vecParam2[0], CONDITION_BEGIN_DATE, CONDITION_END_DATE);
				}
			}
		}
		break;
	case CT_TALENT:
		{
			for each (int nTalentID in pConditionLeaf->m_vecParam1)
			{
				_VLOGGER->ValidateTalentID(nTalentID, _T(CONDITIONS_XML_ATTR_PARAM1));
			}
		}
		break;
	default:
		{
			//				_ASSERT(0);
		}
	}
}

void GConditionsInfoValidator::CheckComposite( GCondition_Composite* pConditionComposite )
{
	if (NULL == pConditionComposite) return;

	if (CAO_AND == pConditionComposite->m_nCAO)
	{
		_VLE(_T(CONDITIONS_XML_TAG_AND));
	}
	else if (CAO_OR == pConditionComposite->m_nCAO)
	{
		_VLE(_T(CONDITIONS_XML_TAG_OR));
	}
	else
	{
		return;
	}	

	for each (CSCondition_Component* pConditionComponent in pConditionComposite->m_vecConditionComponent)
	{
		switch (pConditionComponent->GetType())
		{
		case CCT_LEAF:
			{
				CheckLeaf(dynamic_cast<GCondition_Leaf*>(pConditionComponent));
			}
			break;
		case CCT_COMPOSITE:
			{
				CheckComposite(dynamic_cast<GCondition_Composite*>(pConditionComponent));
			}
			break;
		}
	}
}