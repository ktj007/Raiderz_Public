#include "stdafx.h"
#include "XConditionInfo.h"

XCondition_Leaf::XCondition_Leaf(void)
: CSCondition_Leaf()
{
}

XCondition_Leaf::~XCondition_Leaf(void)
{

}

//////////////////////////////////////////////////////////////////////////
XCondition_Composite::XCondition_Composite(CONDITION_AND_OR nCAO)
: CSCondition_Composite(nCAO)
{

}

XCondition_Composite::~XCondition_Composite()
{

}

