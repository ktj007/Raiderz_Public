#pragma once

#include "CSConditionsInfoMgr.h"
#include "CSConditionInfo.h"

class XConditionsInfo;

class XConditionsInfoMgr : public CSConditionsInfoMgr
{
protected:
	virtual CSCondition_Leaf* New_Condition_Leaf() override;
	virtual CSConditionsInfo* New_ConditionsInfo() override;
	virtual CSCondition_Composite* New_Condition_Composite(CONDITION_AND_OR nCAO) override;

	CSCondition_Leaf* GetFirstLeaf(CSCondition_Composite* pCondition_Composite, CONDITION_TYPE nCondType);
public:
	XConditionsInfoMgr();
	virtual ~XConditionsInfoMgr();

	XConditionsInfo* Get(int nID);

	int GetMaxLevel_For_CT_LEVEL(int nID);
	int GetMinLevel_For_CT_LEVEL(int nID);
};



class XItemConditionsInfoMgr : public XConditionsInfoMgr
{
public:
	XItemConditionsInfoMgr();
	virtual ~XItemConditionsInfoMgr();
};