#include "stdafx.h"
#include "GQuestInfo.h"
#include "GSTLUtil.h"
#include "GValidateLogger.h"
#include "GGlobal.h"
#include "AStlUtil.h"
#include "GItemAddRemoveInfo.h"
#include "GQObjectiveInfo.h"
#include "CSDef_Quest.h"


GQuestInfo::GQuestInfo()
: pItemAddRemoveInfo(NULL)
, bChallengerQuest(false)
{

}

GQuestInfo::~GQuestInfo()
{
	SAFE_DELETE(pItemAddRemoveInfo);
}

GQObjectiveInfo* GQuestInfo::Get( int nQuestID )
{
	return static_cast<GQObjectiveInfo*>(__super::Get(nQuestID));
}

vector<GQObjectiveInfo*> GQuestInfo::GetAllQObjectiveInfo()
{
	return ToGQObjectiveInfo(__super::GetAllQObjectiveInfo());
}

vector<GQObjectiveInfo*> GQuestInfo::GetActQObjectiveInfo()
{
	return ToGQObjectiveInfo(__super::GetActQObjectiveInfo());
}

vector<GQObjectiveInfo*> GQuestInfo::GetEscortQObjectiveInfo()
{
	return ToGQObjectiveInfo(__super::GetEscortQObjectiveInfo());
}

vector<GQObjectiveInfo*> GQuestInfo::GetQObjectiveInfo( QOBJECTIVE_TYPE nType )
{
	return ToGQObjectiveInfo(__super::GetQObjectiveInfo(nType));
}

vector<GQObjectiveInfo*> GQuestInfo::ToGQObjectiveInfo(vector<CSQObjectiveInfo*> vecInfo)
{
	vector<GQObjectiveInfo*> vecGQObjectiveInfo;

	for each (CSQObjectiveInfo* pInfo in vecInfo)
	{
		vecGQObjectiveInfo.push_back(static_cast<GQObjectiveInfo*>(pInfo));
	}

	return vecGQObjectiveInfo;
}