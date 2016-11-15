#pragma once

#include "CSConditionInfo.h"

class XCondition_Leaf : public CSCondition_Leaf
{
public:
	explicit XCondition_Leaf();
	virtual ~XCondition_Leaf();
};

class XCondition_Composite : public CSCondition_Composite
{
public:
	explicit XCondition_Composite(CONDITION_AND_OR nCAO);
	virtual ~XCondition_Composite();
};


