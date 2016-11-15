#pragma once

#include "CSConditionsInfoMgr.h"

TEST_FORWARD_F(Emblem, FEmblem);

class GConditionsInfo;

class GConditionsInfoMgr : public CSConditionsInfoMgr
{
	TEST_FRIEND_F(Emblem, FEmblem);
protected:
	virtual CSCondition_Leaf* New_Condition_Leaf() override;
	virtual CSConditionsInfo* New_ConditionsInfo() override;
	virtual CSCondition_Composite* New_Condition_Composite(CONDITION_AND_OR nCAO) override;

public:
	GConditionsInfoMgr();
	virtual ~GConditionsInfoMgr();

	GConditionsInfo* Get(int nID);
	bool IsExist(int nID);
};


