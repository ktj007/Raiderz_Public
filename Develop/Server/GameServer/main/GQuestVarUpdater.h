#pragma once

class GEntityPlayer;

class GQuestVarUpdater : public MTestMemPool<GQuestVarUpdater>
{
public:
	bool Update(GEntityPlayer* pPlayer, int nQuestID, int nVar);

private:
	void UpdateSingle(GEntityPlayer* pPlayer, int nQuestID, int nVar);
	vector<GEntityPlayer*> GetQuestVarSharablePlayer( GEntityPlayer* pPlayer, bool bPublic );
};
