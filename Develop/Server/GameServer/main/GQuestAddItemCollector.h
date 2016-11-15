#pragma once

class GDBT_QUEST_ADDITEM;
class GQuestInfo;
struct QUEST_REWARD;
struct GITEM_STACK_AMT;

class GQuestAddItemCollector : public MTestMemPool<GQuestAddItemCollector>
{
public:
	void CollectForGive(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, vector<GDBT_QUEST_ADDITEM>& vecAddItem);
	void CollectForReward(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, QUEST_REWARD selectedReward, vector<GDBT_QUEST_ADDITEM>& vecAddItem);

	void RemoveDuplicateCollection(vector<GDBT_QUEST_ADDITEM>& vecAddItem, vector<GITEM_STACK_AMT>& vecRemoveItem);

private:
	void Collect( GEntityPlayer* pPlayer, int nID, int nAddAmount, vector<GDBT_QUEST_ADDITEM> &vecAddItem );	
};
