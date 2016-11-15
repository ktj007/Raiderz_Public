#pragma once

#include "GPartyDef.h"

class GEntityPlayer;

class GPartyMemberStorage : public MTestMemPool<GPartyMemberStorage>
{
public:
	GPartyMemberStorage(int nLimitMemberCount);
	
	// Members
	bool AddMember(MUID uidMember, wstring strMembersName, int nMemberCID);
	void RemoveMember(MUID uidMember);
	bool IsEmpty(void) const;
	bool IsFull(void) const;	
	bool IsExistMember(MUID uidMember) const;
	bool IsLeader(MUID uidLeader) const;
	int GetMemberCount(void) const;	
	int GetOnlineMemberCount(void) const;
	MUID GetAloneMember(void) const;
	partymember_iterator GetMemberBegin(void) const;
	partymember_iterator GetMemberEnd(void) const;
	partymember_iterator FindMember(MUID uidMember) const;
	MUID FindMemberUID(int nCID);
	vector<int> CollectMemberCID();

	// Leader
	bool SetLeader(MUID uidNewLeader);
	MUID GetLeader(void) const;
	int  GetLeaderCID() const;
	MUID FindCandidateForLeader(void) const;

	// Name
	wstring FindName(MUID uidMember) const;

	// Off-line
	void AddOfflineMember(MUID uidMember);
	void RemoveOfflineMember(MUID uidMember);
	MUID FindOfflineMember(wstring strName) const;
	int GetOfflineMemberCount(void) const;

	bool ChangeMemberUID(MUID uidOldMember, MUID uidNewMember);	
	

private:
	int						m_nLimitMemberCount;
	MUID					m_uidLeader;
	map_partymember			m_mapMember;
	set_partymember_offline	m_setOfflineMember;
};
