#ifndef _G_CONDITION_INFO_H_
#define _G_CONDITION_INFO_H_

#include "CSConditionInfo.h"

class GCondition_Leaf : public CSCondition_Leaf, public MTestMemPool<GCondition_Leaf>
{
public:
	explicit GCondition_Leaf();
	virtual ~GCondition_Leaf();
};

class GCondition_Composite : public CSCondition_Composite, public MTestMemPool<GCondition_Composite>
{
public:
	explicit GCondition_Composite(CONDITION_AND_OR nCAO);
	virtual ~GCondition_Composite();
};




#endif//_G_CONDITION_INFO_H_

