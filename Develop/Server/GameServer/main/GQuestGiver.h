#pragma once

class GEntityPlayer;
class GQuestInfo;
class GPlayerQuest;

class GDBT_QUEST_ACCEPT;

class GQuestGiver : public MTestMemPool<GQuestGiver>
{
public:	
	bool Give(GEntityPlayer* pPlayer, int nQuestID, bool bForce=false);
	void GiveForGM(GEntityPlayer* pPlayer, int nQuestID);	

	void GiveForDBTask(GDBT_QUEST_ACCEPT& data);	
	bool CheckForChallengerQuest(GEntityPlayer* pPlayer, int nQuestID, const GQuestInfo* pQuestInfo);
	
private:
	bool Check(GEntityPlayer* pPlayer, int nQuestID, const GQuestInfo* pQuestInfo, bool bForce=false);	
	bool MakeDBTaskData(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo, GDBT_QUEST_ACCEPT& data);	

	void Route(GEntityPlayer* pPlayer, int nQuestID);
	void RefreshNPCIcon(GEntityPlayer* pPlayer);
	void Script(GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest, MUID uidNPC);
	void UpdateQObject(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo);
	void AddRemoveItem(GEntityPlayer* pPlayer, GDBT_QUEST_ACCEPT& data);
	void ShareQuest(GEntityPlayer* pPlayer, GQuestInfo* pQuestInfo);
	void UpdateUsableSensor(GEntityPlayer* pPlayer);	
};
