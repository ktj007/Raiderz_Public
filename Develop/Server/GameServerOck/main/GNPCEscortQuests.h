#pragma once

class GNPCEscortQuest;
class GEntityNPC;

typedef map<pair<int, int>, GNPCEscortQuest*>	MAP_NPCESCORTQUEST;

class GNPCEscortQuests : public MTestMemPool<GNPCEscortQuests>
{
private:
	GEntityNPC*			m_pOwner;
	MAP_NPCESCORTQUEST	m_mapNPCEscortQuest;

private:
	GNPCEscortQuest* New(int nQuestID, int nQObjectiveID);
	void Delete(GNPCEscortQuest* pNPCEscortQuest);
	void Insert(GNPCEscortQuest* pNPCEscortQuest);
	

public:
	GNPCEscortQuests(GEntityNPC* pOwner);
	~GNPCEscortQuests(void);

	void Clear();

	GNPCEscortQuest* Get(int nQuestID, int nQObejctiveID);

	void InsertEscortPlayer(MUID uidPlayer, int nQuestID, int nQObjectiveID);
	void DeleteEscortPlayer(MUID uidPlayer, int nQuestID, int nQObjectiveID);

	vector<MUID> GetEscortPlayerUID();
};
