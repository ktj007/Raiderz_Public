#pragma once

class GEntityPlayer;

class GQuestReseter : public MTestMemPool<GQuestReseter>
{
public:
	bool ResetDoneQuestForGM(GEntityPlayer* pPlayer);	
	bool ResetAllQuestForGM(GEntityPlayer* pPlayer);
};
