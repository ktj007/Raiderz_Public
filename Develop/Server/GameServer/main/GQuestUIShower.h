#pragma once

class GEntityPlayer;

class GQuestUIShower : public MTestMemPool<GQuestUIShower>
{
public:
	bool ShowAcceptUI(GEntityPlayer* pPlayer, int nQuestID);
	bool ShowRewardUI(GEntityPlayer* pPlayer, int nQuestID);
};
