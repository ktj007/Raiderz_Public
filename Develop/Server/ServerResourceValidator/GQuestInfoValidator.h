#ifndef _G_QUEST_INFO_VALIDATOR_H_
#define _G_QUEST_INFO_VALIDATOR_H_

#include "GValidatorLib.h"
#include "CSBaseValidator.h"
#include "CSQuestInfo.h"

class GQuestInfo;
class GQObjectiveInfo;
class GItemAddRemoveInfo;

class GVALIDATOR_API GQuestInfoValidator : public CSBaseValidator
{
public:
	void CheckQuest(GQuestInfo* pQuestInfo);
	void CheckItem(GItemAddRemoveInfo* pItemInfo);
	void CheckQObjectives(GQuestInfo* pQuestInfo);
	void CheckQObjective(GQObjectiveInfo* pQObjectiveInfo);
	void CheckRewards(GQuestInfo* pQuestInfo);
	void CheckReward(const QUEST_REWARD& questReward, bool bRepeatableQuest);
	void CheckSelectableReward(const vector<QUEST_REWARD>& vecSelectableReward);
	void CheckChallengerQuest(GQuestInfo* pQuestInfo);

public:
	virtual bool Check();
	
};

#endif//_G_QUEST_INFO_VALIDATOR_H_
