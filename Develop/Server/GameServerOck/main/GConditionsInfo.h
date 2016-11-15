#ifndef _G_CONDITIONS_INFO_H_
#define _G_CONDITIONS_INFO_H_

#include "CSConditionsInfo.h"

class GConditionsInfo : public CSConditionsInfo, public MTestMemPool<GConditionsInfo>
{
protected:
	virtual CSCondition_Composite*	New_Condition_Composite(CONDITION_AND_OR nAndOr);
public:
	GConditionsInfo();
	virtual ~GConditionsInfo();
};

#endif//_G_CONDITIONS_INFO_H_
