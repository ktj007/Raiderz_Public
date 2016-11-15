#include "StdAfx.h"
#include "GConditionInfo.h"


GCondition_Leaf::GCondition_Leaf(void)
: CSCondition_Leaf()
{
}

GCondition_Leaf::~GCondition_Leaf(void)
{

}

//////////////////////////////////////////////////////////////////////////
GCondition_Composite::GCondition_Composite(CONDITION_AND_OR nCAO)
: CSCondition_Composite(nCAO)
{

}

GCondition_Composite::~GCondition_Composite()
{

}

