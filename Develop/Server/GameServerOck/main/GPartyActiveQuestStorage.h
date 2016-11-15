#pragma once

class GPartyActiveQuestStorage : public MTestMemPool<GPartyActiveQuestStorage>
{
public:
	typedef map<int,int>	MAP_ACTIVE_QUEST;		// <QuestID, ContainCount>

public:
	GPartyActiveQuestStorage();
	virtual ~GPartyActiveQuestStorage();

	void AddQuest(int nQuestID);
	void AddQuest(int nQuestID, int nCount);
	void RemoveQuest(int nQuestID);
	bool HasQuest(int nQuestID) const;
	void ClearQuest(void);

private:	
	MAP_ACTIVE_QUEST		m_mapActiveQuests;
};
