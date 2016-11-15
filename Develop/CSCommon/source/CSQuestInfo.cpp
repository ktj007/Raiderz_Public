#include "stdafx.h"
#include "CSQuestInfo.h"
#include "CSDef_Quest.h"
#include "CSDef.h"
#include "AStlUtil.h"
#include "CSQObjectiveInfo.h"

CSQuestInfo::CSQuestInfo() : nID(0)
, nLevel(0)	
, nLimitPersonnel(INT_MAX)
, bRepeatable(false)
, bCancelable(true)
, nShareType(QSHAT_NONE)	
, nRewardNPCID(0)
, nRewardDialogID(0)
, nRewardFieldID(0)
, vRewardPos(vec3::ZERO)
, nTimeLimit(NO_LIMIT_TIME)
, bPublicVar(false)
, bValidation(true)
, nTrialType(QTT_PARTY)
, nTrialFieldGroupID(0)
, nGiveDialogID(0)
, nIncompleteDialogID(0)
, nShareConditionsID(0)
, nPartyFieldID(0)
, nPartySensorID(0)
, nAutoPartyMemberLimit(MAX_PARTY_MEMBER_COUNT)
, bAutoParty(false)
, nRequiredMinLevelForChallengerQuest(0)
, nRequiredMaxLevelForChallengerQuest(0)
, nRequiredConditionIDForChallengerQuest(0)
{

}

CSQuestInfo::~CSQuestInfo()
{
	SAFE_DELETE_MAP(mapQObjectiveInfo);
}

bool CSQuestInfo::Insert(CSQObjectiveInfo* pQObjectiveInfo)
{
	_ASSERT(NULL != pQObjectiveInfo);
	if (NULL == pQObjectiveInfo) return false;

	CSQObjectiveInfo* pExistnfo = Get(pQObjectiveInfo->nID);
	_ASSERT(NULL == pExistnfo);
	if (NULL != pExistnfo) return false;

	mapQObjectiveInfo.insert(MAP_QOBJECTIVEINFO::value_type(pQObjectiveInfo->nID, pQObjectiveInfo));
	return true;
}

CSQObjectiveInfo* CSQuestInfo::Get(int nQObjectiveID)
{
	MAP_QOBJECTIVEINFO::iterator itor = mapQObjectiveInfo.find(nQObjectiveID);
	if (mapQObjectiveInfo.end() == itor) return NULL;

	return (*itor).second;
}

bool CSQuestInfo::IsTrialQuest()
{
	return (nTrialFieldGroupID != 0);
}

bool CSQuestInfo::IsEscortQuest()
{
	for each (MAP_QOBJECTIVEINFO::value_type pair in mapQObjectiveInfo)
	{
		CSQObjectiveInfo* pQObjectiveInfo = pair.second;
		if (NULL == pQObjectiveInfo) continue;
		if (QOT_ESCORT != pQObjectiveInfo->nType) continue;

		return true;
	}

	return false;
}

bool CSQuestInfo::IsEmptyQObjective()
{
	return mapQObjectiveInfo.empty();
}

bool CSQuestInfo::CheckRequireLevel_ForChallengerQuest(int nLevel) const
{
	if (nRequiredMinLevelForChallengerQuest != 0)
	{
		if (nLevel < nRequiredMinLevelForChallengerQuest)
			return false;
	}

	if (nRequiredMaxLevelForChallengerQuest != 0)
	{
		if (nLevel > nRequiredMaxLevelForChallengerQuest)
			return false;
	}

	return true;
}

vector<CSQObjectiveInfo*> CSQuestInfo::GetAllQObjectiveInfo()
{
	vector<CSQObjectiveInfo*> vecQObjectiveInfo;

	for each (const MAP_QOBJECTIVEINFO::value_type& data in mapQObjectiveInfo)
	{
		vecQObjectiveInfo.push_back(data.second);
	}

	return vecQObjectiveInfo;
}

vector<CSQObjectiveInfo*> CSQuestInfo::GetActQObjectiveInfo()
{
	return GetQObjectiveInfo(QOT_ACT);
}

vector<CSQObjectiveInfo*> CSQuestInfo::GetEscortQObjectiveInfo()
{
	return GetQObjectiveInfo(QOT_ESCORT);
}

vector<CSQObjectiveInfo*> CSQuestInfo::GetCollectQObjectiveInfo()
{
	return GetQObjectiveInfo(QOT_COLLECT);
}

vector<CSQObjectiveInfo*> CSQuestInfo::GetQObjectiveInfo(QOBJECTIVE_TYPE nType)
{
	vector<CSQObjectiveInfo*> vecQObjectiveInfo;

	for each (MAP_QOBJECTIVEINFO::value_type pair in mapQObjectiveInfo)
	{
		CSQObjectiveInfo* pQObjectiveInfo = pair.second;
		if (NULL == pQObjectiveInfo) continue;
		if (nType != pQObjectiveInfo->nType) continue;

		vecQObjectiveInfo.push_back(pQObjectiveInfo);
	}

	return vecQObjectiveInfo;
}

int CSQuestInfo::GetQObejctiveInfoCount()
{
	return static_cast<int>(mapQObjectiveInfo.size());
}

QUEST_REWARD::QUEST_REWARD()
: nType(QRT_NONE)
, nParam1(0)
, nParam2(0)
{

}

QUEST_REWARD::QUEST_REWARD( QUEST_REWARD_TYPE nType, int nParam1, int nParam2 )
: nType(nType)
, nParam1(nParam1)
, nParam2(nParam2)
{

}

QUEST_REWARD::QUEST_REWARD( QUEST_REWARD_TYPE nType, int nParam1)
: nType(nType)
, nParam1(nParam1)
, nParam2(0)
{

}