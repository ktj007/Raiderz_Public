#pragma once

#include "CSPartyMember.h"
#include "MReferenceCounted.h"

class ZPartyMember: public CSPartyMember, public MReferenceCounted
{
public:
	typedef pair<int, MUID>				pair_field;			// nFieldGroupID, uidFieldGroup
	typedef set<int>					set_quest;			// nQuestID
	typedef set_quest::const_iterator	quest_iterator;

public:
	ZPartyMember();
	ZPartyMember(MUID uidMember, wstring strMemberName, int nMemberServerID, int nMemberCID);
	virtual ~ZPartyMember();

	// 추가 정보
	int GetGameServerID(void)	const;
	void SetGameServerID(int id);
	bool IsMoveServer(void) const;
	void SetMoveServer(bool isMoveServer);
	bool IsOffline(void) const;
	void SetOffline(bool isOffline);

	// FieldGroup
	int GetFieldGroupID(void) const;
	MUID GetFieldGroupUID(void) const;
	void SetFieldGroup(int nFieldGroupID, MUID uidFieldGroup);
	void ClearFieldGroup(void);
	bool IsEmptyFieldGroup(void) const;

	// QuestID
	void AddQuest(int nQuestID);
	void RemoveQuest(int nQuestID);
	void ClearQuest(void);
	bool IsEmptyQuest(void) const;
	quest_iterator BeginQuest(void) const;
	quest_iterator EndQuest(void) const;
	quest_iterator FindQuest(int nQuestID) const;
	bool IsExistQuest(int nQuestID) const;

	int GetCID(void) const;

private:
	int			m_nGameServerID;
	int			m_nCID;
	bool		m_isMoveServer;
	bool		m_isOffline;

	pair_field	m_pairFieldGroup;
	set_quest	m_setQuestID;
};
