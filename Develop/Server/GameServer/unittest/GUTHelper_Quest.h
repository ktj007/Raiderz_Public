#pragma once

class GQuestInfo;
class GQObjectiveInfo;
class GPlayerQuest;
class GEntityPlayer;
struct QUEST_REWARD;

class GUTHelper_Quest
{
public:
	static GQuestInfo* NewQuestInfo(int nID = 0);
	static GQObjectiveInfo* NewQObjectiveInfo(GQuestInfo* pQuestInfo, int nID = 0);
	static GQObjectiveInfo* NewCollectQObjectiveInfo(GQuestInfo* pQuestInfo, int nItemQuantity=1, int nItemID = INVALID_ID, int nID = INVALID_ID);
	static GQObjectiveInfo* NewDestroyQObjectiveInfo(GQuestInfo* pQuestInfo, int nDestroyQuantity=1, int nNPCID = INVALID_ID, int nID = INVALID_ID);
	static GQObjectiveInfo* NewScoutQObjectiveInfo(GQuestInfo* pQuestInfo, int nSensorID, int nFieldID=INVALID_ID, int nID=INVALID_ID);

	static GQObjectiveInfo* NewInteractQObjectiveInfo(GQuestInfo* pQuestInfo, int nNPCID, int nInteractCount);
	static GPlayerQuest* GiveNewPlayerQuest(GEntityPlayer* pPlayer, int nID = 0, int nTimeLimit = 0, bool bHasObjective=false);
	static void GiveNewPlayerDoneQuest(GEntityPlayer* pPlayer, int nID = 0, int nTimeLimit = 0, bool bHasObjective=false);
	static void AddSelectableReward(GQuestInfo* pQuestInfo, const QUEST_REWARD& reward);
	static void AddReward(GQuestInfo* pQuestInfo, const QUEST_REWARD& reward);


	static void DropQuest(GEntityPlayer* pPlayer, int nID);
	static void FailQuest(GEntityPlayer* pPlayer, int nID);
	static void CompleteQuest(GEntityPlayer* pPlayer, int nID);
	static void CompleteQuestAllObjective(GEntityPlayer* pPlayer, int nID);
	static void DoneQuest(GEntityPlayer* pPlayer, int nID);
};
