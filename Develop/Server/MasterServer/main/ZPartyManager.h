#pragma once


class ZParty;

class ZPartyManager
{
public:
	typedef map<MUID, ZParty*>	ZPartyMap;

public:
	ZPartyManager();
	virtual ~ZPartyManager();

	ZParty* AddParty(MUID uidParty, MUID uidLeader, wstring strLeaderName, int nLeaderServerID, int nLeaderCID);
	ZParty* AddParty(MUID uidLeader, wstring strLeaderName, int nLeaderServerID, int nLeaderCID);
	void RemoveParty(MUID uidParty);
	void AddMember(MUID uidParty, MUID uidMember, wstring strMemberName, int nMemberServerID, int nMemberCID);
	void RemoveMember(MUID uidParty, MUID uidMember);
	void ChangeLeader(MUID uidParty, MUID uidNewLeader);	
	int GetCount(void) const;
	ZParty* Find(MUID uidParty);
	void Clear(void);

	bool IsPartyMember(MUID uidParty, MUID uidPlayer);
	MUID GetFieldGroupUID(MUID uidParty, int nFieldGroupID);
	const ZPartyMap& GetPartyMap(void) const;
	
private:
	ZPartyMap					m_mapParty;
	MUIDGenerator				m_UIDGenerator;
};
