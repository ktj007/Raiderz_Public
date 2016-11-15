#pragma once

#include "CSConditionsInfo.h"

class XConditionsInfo : public CSConditionsInfo
{
protected:
	virtual CSCondition_Composite*	New_Condition_Composite(CONDITION_AND_OR nAndOr);
public:
	XConditionsInfo();
	virtual ~XConditionsInfo();
};

