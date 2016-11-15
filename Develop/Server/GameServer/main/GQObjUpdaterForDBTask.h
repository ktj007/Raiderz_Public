#pragma once

class GDBT_QUEST_OBJECT;
class GEntityPlayer;
class GPlayerQuest;
class GQObjectiveInfo;
class GPlayerQObjective;

enum QOBJECTIVE_TYPE;

class GQObjUpdaterForDBTask : public MTestMemPool<GQObjUpdaterForDBTask>
{
public: 
	void Update(const GDBT_QUEST_OBJECT& data);
	
private:
	void UpdateQObjective(GPlayerQuest* pPlayerQuest, int nQObejctiveID, int nProgress, bool& bChangeToComplete , bool& bChangeToIncomplete);

	void UpdateQuestComplete(GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest, GQObjectiveInfo* pQObjectiveInfo, bool bTrigger, bool bChangeToComplete, bool bChangeToIncomplete);
	void ChangeToQuestComplete(GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest, GQObjectiveInfo* pQObjectiveInfo, bool bTrigger);
	void ChangeToQuestIncomplete(GEntityPlayer* pPlayer, GQObjectiveInfo* pQObjectiveInfo);

	void UpdateQObjectiveScript(GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest, GPlayerQObjective* pPlayerQObjective, bool bTrigger, MUID uidNPC);
	bool IsScriptUpdatableObjectiveType(QOBJECTIVE_TYPE nType);
	void UpdateUsableSensor( GEntityPlayer* pPlayer );

	void Route(GEntityPlayer* pPlayer, GPlayerQObjective* pPlayerQObjective);
};