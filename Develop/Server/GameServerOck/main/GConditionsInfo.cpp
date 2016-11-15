#include "StdAfx.h"
#include "GConditionsInfo.h"
#include "GConditionInfo.h"

GConditionsInfo::GConditionsInfo()
: CSConditionsInfo()
{

}

GConditionsInfo::~GConditionsInfo()
{
	
}

CSCondition_Composite* GConditionsInfo::New_Condition_Composite( CONDITION_AND_OR nAndOr )
{
	return new GCondition_Composite(nAndOr);
}