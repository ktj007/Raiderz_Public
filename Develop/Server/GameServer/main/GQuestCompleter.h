#pragma once

class GEntityPlayer;

class GQuestCompleter : public MTestMemPool<GQuestCompleter>
{
public:
	bool Complete(GEntityPlayer* pPlayer, int nQuestID);
	void CompleteForGM(GEntityPlayer* pPlayer, int nQuestID);

	bool Incomplete(GEntityPlayer* pPlayer, int nQuestID);
};
