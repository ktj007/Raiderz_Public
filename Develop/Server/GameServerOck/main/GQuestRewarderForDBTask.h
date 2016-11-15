#ifndef _GQUEST_REWARD_H
#define _GQUEST_REWARD_H

class GEntityPlayer;
class GPlayerQuest;
class GDBT_QEUST_DONE;

struct GITEM_STACK_AMT;
struct QUEST_REWARD;

class GQuestRewarderForDBTask : public MTestMemPool<GQuestRewarderForDBTask>
{
public:
	bool Reward(GDBT_QEUST_DONE& data);	

private:
	void UpdatePlayerUsableSensor(GEntityPlayer* pPlayer);	
	void SynchParty(GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest);
	void DoneQuest(GEntityPlayer* pPlayer, int nQuestID);

	void AddItem(GEntityPlayer* pPlayer, GDBT_QEUST_DONE& data);
	void RemoveItem( GEntityPlayer* pPlayer, GDBT_QEUST_DONE &data );
	void RewardFaction(GEntityPlayer* pPlayer, const vector<QUEST_REWARD>& questReward, QUEST_REWARD selectedReward);
	void RewardMoney(GEntityPlayer* pPlayer, int nMoney);
	void RewardXP(GEntityPlayer* pPlayer, int nXP, int nDeltaXP, int nLevel, int nDeltaLevel);


	void Route(GEntityPlayer* pPlayer, MUID uidNPC, int nQuestID);
	void RefreshNPCIcon(GEntityPlayer* pPlayer);
	void EndInteraction(GEntityPlayer* pPlayer);

	void Script(GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest, MUID uidNPC);
	void DeleteQuest(GPlayerQuest* pPlayerQuest);
};	

#endif