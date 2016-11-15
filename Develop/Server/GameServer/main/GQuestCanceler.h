#pragma once

class GEntityPlayer;
class GPlayerQuest;
class GDBT_QUEST_GIVEUP;

class GQuestCanceler : public MTestMemPool<GQuestCanceler>
{
public:
	bool Cancel(GEntityPlayer* pPlayer, int nQuestID);		
	void CancelForDBTask(const GDBT_QUEST_GIVEUP& data);

private:
	bool Check(GEntityPlayer* pPlayer, int nQuestID);
	void MakeDBTaskData(GEntityPlayer* pPlayer, GPlayerQuest* pQuest, int nQuestID, GDBT_QUEST_GIVEUP& data);
	void Route(GEntityPlayer* pPlayer, int nQuestID);	
};
