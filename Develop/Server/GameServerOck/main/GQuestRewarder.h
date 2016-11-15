#pragma once

class GEntityPlayer;
class GQuestInfo;
class GDBT_QEUST_DONE;
struct QUEST_REWARD;

TEST_FORWARD_N(QuestSystem, TimeExpired);

class GQuestRewarder : public MTestMemPool<GQuestRewarder>
{
public:
	TEST_FRIEND_N(QuestSystem, TimeExpired);	

	bool Reward(GEntityPlayer* pPlayer, int nQuestID, int nSelectedReward=-1);
	bool RewardForGM(GEntityPlayer* pPlayer, int nQuestID);		

private:
	bool Check(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, int nSelectedReward, QUEST_REWARD& selectedReward);
	bool CheckItemAddable(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, const QUEST_REWARD& selectedReward);

	bool MakeDBTaskData(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, const QUEST_REWARD& selectedReward, GDBT_QEUST_DONE& data);
	void CalcRewardMoneyAndXP(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, const QUEST_REWARD& selectedReward, int& nMoney, int& nXP);	
};
