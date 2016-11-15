#pragma once

class GEntityPlayer;
class GQObjectiveInfo;
class GPlayerQuest;
class GDBT_QUEST_OBJECT;
class GEntityNPC;

enum QOBJECTIVE_TYPE;

class GQObjUpdater : public MTestMemPool<GQObjUpdater>
{
public :
	GQObjUpdater();
	virtual ~GQObjUpdater();

	bool IncCollectQObj(GEntityPlayer* pPlayer, int nObjectiveParam1, int nVar);
	bool DecCollectQObj(GEntityPlayer* pPlayer, int nObjectiveParam1, int nVar);
	TEST_VIRTUAL bool UpdDestroyQObj(GEntityPlayer* pPlayer, int nObjectiveParam1, int nVar, GEntityNPC* pNPC);
	bool UpdScoutQObj(GEntityPlayer* pPlayer, int nObjectiveParam1, int nVar);
	bool UpdEscortQObj(GEntityPlayer* pPlayer, int nObjectiveParam1, int nVar);
	bool UpdInteractQObj(GEntityPlayer* pPlayer, int nQuestID, const vector<int>& vecQObjeciveID, MUID uidNPC);
	bool UpdActQObj(GEntityPlayer* pPlayer, int nQuestID, int nVar);

	bool CompleteAllQObj(GEntityPlayer* pPlayer, int nQuestID, QOBJECTIVE_TYPE nExceptType=(QOBJECTIVE_TYPE)0);

private:
	bool UpdateQObjectiveByParam1(GEntityPlayer* pPlayer, QOBJECTIVE_TYPE nObjectiveType, int nObjectiveParam1, int nVar, MUID uidNPC=MUID::ZERO);
	void UpdateQObjective(GEntityPlayer* pPlayer, int nVar, const vector<GQObjectiveInfo*>& vecQObjectiveInfo, MUID uidNPC=MUID::ZERO);
	vector<GEntityPlayer*> CollectQOUpdatePlayer(GEntityPlayer* pPlayer, GQObjectiveInfo* pQObjectiveInfo, MUID uidNPC=MUID::ZERO);

	void UpdateSingleQObjective(GEntityPlayer* pPlayer, GQObjectiveInfo* pQObjectiveInfo, const int nVar, bool bTrigger, MUID uidNPC=MUID::ZERO);

	void MakeDBTaskData(GEntityPlayer* pPlayer, GPlayerQuest* pPlayerQuest, GQObjectiveInfo* pQObjectiveInfo, int nProgress, bool bTrigger, MUID uidNPC, GDBT_QUEST_OBJECT& data);			
	bool IsScriptUpdatableObjectiveType(QOBJECTIVE_TYPE nType);
};