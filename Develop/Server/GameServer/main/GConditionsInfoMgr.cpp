#include "StdAfx.h"
#include "GConditionsInfoMgr.h"
#include "GConditionsInfo.h"
#include "GConditionInfo.h"

GConditionsInfoMgr::GConditionsInfoMgr()
{
}

GConditionsInfoMgr::~GConditionsInfoMgr()
{

}

CSCondition_Leaf* GConditionsInfoMgr::New_Condition_Leaf()
{
	return new GCondition_Leaf();
}

CSConditionsInfo* GConditionsInfoMgr::New_ConditionsInfo()
{
	return new GConditionsInfo();
}

CSCondition_Composite* GConditionsInfoMgr::New_Condition_Composite( CONDITION_AND_OR nCAO )
{
	return new GCondition_Composite(nCAO);
}

GConditionsInfo* GConditionsInfoMgr::Get( int nID )
{
	return static_cast<GConditionsInfo*>(CSConditionsInfoMgr::Get(nID));
}

bool GConditionsInfoMgr::IsExist( int nID )
{
	if (NULL == Get(nID)) return false;

	return true;
}