#pragma once

#include "MReferenceCounted.h"
#include "CSDef_Party.h"

class ZPartyMember;
enum LOOTING_RULE_COMMON;
enum LOOTING_RULE_RARE;
enum LOOTING_RULE_RARE_FOR;

class ZParty: public MReferenceCounted
{
public:
	typedef map<MUID, ZPartyMember*>	map_member;
	typedef map_member::const_iterator	member_iterator;

public:	
	ZParty(MUID uidParyt, MUID uidLeader, wstring strLeaderName, int nLeaderServerID, int nLeaderCID);
	virtual ~ZParty();

	void SetAutoParty();
	// Party
	MUID GetUID(void) const;
	MUID GetLeader(void) const;
	void ChangeLeader(MUID uidNewLeader);
	void ChangeMemberUID(MUID uidOldMember, MUID uidNewMember);
	MUID GetAloneMember(void) const;
	int GetOnlineMemberCount(void) const;	
	int GetOfflineMemberCount(void) const;
	bool IsExistAllOnlineMembersIn(int nGameServerID) const;
	MUID FindCandidateForLeader(void) const;
	MUID FindCandidateForLeaderOtherServer(void) const;

	// Member
	ZPartyMember* AddMember(MUID uidMember, wstring strMemberName, int nMemberServerID, int nMemberCID);
	void RemoveMember(MUID uidMember);
	member_iterator BeginMember(void) const;
	member_iterator EndMember(void) const;
	member_iterator FindMember(MUID uidMember) const;
	ZPartyMember*	FindMember(wstring strName) const;
	bool IsFull(void) const;
	bool IsEmpty(void) const;	
	int GetCount(void) const;
	bool IsExistMember(MUID uidMember) const;
	ZPartyMember* GetMember(MUID uidMember) const;
	MUID GetFirstOfflineMember() const;
	
	// Field
	MUID GetFieldGroupUID(int nFieldGroupID);		
	void RemoveFieldSelf(int nFieldGroupID);	

	// SyncReady
	bool IsSyncReady(void);

	// PartySetting
	const PARTY_SETTING& GetPartySetting() const;
	void SetPartySetting(const PARTY_SETTING& partySetting);

	void SetName(wstring strName);
	void SetLootingRule(LOOTING_RULE_DATA rule);
	void SetAutoPartyQuestID(int nQuestID);
private:
	MUID m_uidParty;	
	map_member	m_mapMembers;
	bool m_isSyncReady;

	PARTY_SETTING m_partySetting;
};
