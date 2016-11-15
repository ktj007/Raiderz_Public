#pragma once

#include "MReferenceCounted.h"
#include "GPartyDef.h"

class GProxyParty: public MReferenceCounted, public MTestMemPool<GProxyParty>
{
public:
	GProxyParty(MUID uidParty, MUID uidLeader, wstring strLeaderName, int nLeaderCID);
	virtual ~GProxyParty();

	void AddMember(MUID uidMember, wstring strMemberName, int nMemberCID);
	void RemoveMember(MUID uidMember);
	void ChangeLeader(MUID uidLeader);
	void ChangeUID(MUID uidOld, MUID uidNew);
	MUID GetUID(void) const;
	MUID GetLeader(void) const;
	wstring GetName(MUID uidMember) const;
	int GetCID(MUID uidMember) const;
	int GetCount(void) const;	
	bool IsFull(void) const;
	MUID Find(wstring strName) const;
	MUID Find(int nCID) const;
	partymember_iterator BeginMember(void) const;
	partymember_iterator EndMember(void) const;
	partymember_iterator FindMember(MUID uidMember) const;
	bool IsExistMember(MUID uidMember) const;

private:
	MUID m_uidParty;
	MUID m_uidLeader;
	map_partymember m_mapMembers;
};

class GProxyPartyManager : public MTestMemPool<GProxyPartyManager>
{
public:
	typedef map<MUID, GProxyParty*>		GProxyPartyMap;

public:
	GProxyPartyManager();
	virtual ~GProxyPartyManager();

	GProxyParty* AddParty(MUID uidParty, MUID uidLeader, wstring strLeaderName, int nLeaderCID);
	void RemoveParty(MUID uidParty);
	void AddMember(MUID uidParty, MUID uidMember, wstring strMemberName, int nMemberCID);
	void RemoveMember(MUID uidParty, MUID uidMember);
	void ChangeLeader(MUID uidParty, MUID uidNewLeader);

	void Clear(void);
	GProxyParty* Find(MUID uidParty) const;
	size_t GetCount(void) const;

private:
	GProxyPartyMap						m_PartyMap;
};
