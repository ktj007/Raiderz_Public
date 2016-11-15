#pragma once

class GField;
class GGluePlayer;
class GGlueNPC;
class GGlueQuest;

class GScriptGroup_Quest : public MTestMemPool<GScriptGroup_Quest>
{
public:
	GScriptGroup_Quest(void);
	~GScriptGroup_Quest(void);

	void RegisterGlues();
	
	void Load(int nQuestID);

	bool OnBegin(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer, GGlueNPC* pNPC);
	bool OnEnd(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer, GGlueNPC* pNPC);
	bool OnComplete(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer);
	bool OnFail(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer);
	bool OnObjectiveComplete(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer, int nObjectiveID, bool bTrigger);
	bool OnObjectiveUpdate(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer, int nObjectiveID, bool bTrigger, int nProgress);
	bool OnObjectiveProgress(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer, int nObjectiveID, bool bTrigger);
	bool OnObjectiveInteract(GGlueQuest* pPlayerQuest, GGluePlayer* pPlayer, int nObjectiveID, bool bTrigger, int nProgress, GGlueNPC* pNPC);

private:
	bool PreCheck();
};
