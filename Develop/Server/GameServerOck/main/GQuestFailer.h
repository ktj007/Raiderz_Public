#pragma once

class GEntityPlayer;
class GDBT_QUEST_COMMON;

class GQuestFailer : public MTestMemPool<GQuestFailer>
{
public:
	void Fail(GEntityPlayer* pPlayer, int nQuestID);
	void FailForDBTask(const GDBT_QUEST_COMMON& data);

private:
	void Route(GEntityPlayer* pPlayer, int nQuestID);	
};
