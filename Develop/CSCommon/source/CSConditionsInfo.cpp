#include "StdAfx.h"
#include "CSConditionsInfo.h"
#include "CSConditionInfo.h"

CSConditionsInfo::CSConditionsInfo()
: m_nID(0)
, m_pConditionComposite(NULL)
, m_bValidation(true)
{
	
}

CSConditionsInfo::~CSConditionsInfo()
{
	SAFE_DELETE(m_pConditionComposite);
}

bool CSConditionsInfo::HasCondition(CONDITION_TYPE nConditionType)
{
	set<CONDITION_TYPE>::iterator itor = m_setUsedConditionType.find(nConditionType);
	if (m_setUsedConditionType.end() != itor) return true;

	if (CT_QUEST == nConditionType)
	{
		itor = m_setUsedConditionType.find(CT_QUEST_VAR);
		if (m_setUsedConditionType.end() != itor) return true;

		itor = m_setUsedConditionType.find(CT_QUEST_OBJECTIVE_COMPLETE);
		if (m_setUsedConditionType.end() != itor) return true;

		itor = m_setUsedConditionType.find(CT_QUEST_OBJECTIVE_INCOMPLETE);
		if (m_setUsedConditionType.end() != itor) return true;
	}

	if (CT_ITEM == nConditionType)
	{
		itor = m_setUsedConditionType.find(CT_NOITEM);
		if (m_setUsedConditionType.end() != itor) return true;

		itor = m_setUsedConditionType.find(CT_FEWITEM);
		if (m_setUsedConditionType.end() != itor) return true;

		itor = m_setUsedConditionType.find(CT_MANYITEM);
		if (m_setUsedConditionType.end() != itor) return true;
	}

	return false;
}

void CSConditionsInfo::Create()
{
	_ASSERT(m_pConditionComposite == NULL);

	m_pConditionComposite = New_Condition_Composite(CAO_AND);	
}