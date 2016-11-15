#pragma once

class GEntityNPC;

class GNPCEscortQuest : public MTestMemPool<GNPCEscortQuest>
{
private:
	GEntityNPC*	m_pOwner;
	int			m_nQuestID;
	int			m_nQObjectiveID;
	set<MUID>	m_setEscortPlayerUID;

public:
	GNPCEscortQuest(GEntityNPC* pOwner, int nQuestID, int nQObjectiveID);

	int GetQuestID() const					{ return m_nQuestID; }
	int GetQObjectiveID() const				{ return m_nQObjectiveID; }
	void GetEscortPlayerUID(set<MUID>& setEscortPlayerUID) const;

	bool IsEscortPlayerEmpty() const	{ return m_setEscortPlayerUID.empty(); }

	void InsertEscortPlayer(MUID uidEscortPlayer);
	void DeleteEscortPlayer(MUID uidEscortPlayer);
	void CompleteEscortQuestObjective();
};
