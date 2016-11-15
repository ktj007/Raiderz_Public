#pragma once

class GEntityPlayer;
class GEntityNPC;

class GQObjInteractor : public MTestMemPool<GQObjInteractor>
{
public:
	bool Prepare(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nQuestID);
	bool Cancel(GEntityPlayer* pPlayer, int nQuestID);
	bool Act(GEntityPlayer* pPlayer, GEntityNPC* pNPC, int nQuestID, const vector<int>& vecQObjeciveID);
};
