#include "StdAfx.h"
#include "XConditionsInfo.h"
#include "XConditionInfo.h"

XConditionsInfo::XConditionsInfo()
: CSConditionsInfo()
{

}

XConditionsInfo::~XConditionsInfo()
{

}

CSCondition_Composite* XConditionsInfo::New_Condition_Composite( CONDITION_AND_OR nAndOr )
{
	return new XCondition_Composite(nAndOr);
}