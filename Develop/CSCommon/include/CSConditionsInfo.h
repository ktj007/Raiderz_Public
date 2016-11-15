#pragma once

#include "CSConditionInfo.h"

#include <vector>
#include <set>
using std::vector;
using std::set;

enum CONDITION_TYPE;

class CSConditionsInfo
{
protected:
	virtual CSCondition_Composite*	New_Condition_Composite(CONDITION_AND_OR nAndOr) = 0;
public:
	int						m_nID;
	bool					m_bValidation;
	CSCondition_Composite*	m_pConditionComposite;

	set<CONDITION_TYPE>		m_setUsedConditionType;

public:
	CSConditionsInfo();
	virtual ~CSConditionsInfo();
	void Create();

	bool HasCondition(CONDITION_TYPE nConditionType);
};


