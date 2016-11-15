#pragma once

class ZParty;
class ZPartyCommandRouter;

class ZFixedPartyLogic
{
public:
	struct FixedPartyNode
	{
		MUID		uidParty;
		set<MUID>	setMemberUID;
	};

	typedef map<int, FixedPartyNode>	waiting_map;

public:
	ZFixedPartyLogic();
	virtual ~ZFixedPartyLogic();

	int GetFixedPartyID(wstring strName);
	ZParty* FindParty(int nFixedPartyID);
	void AddWaiting(int nFixedPartyID, MUID uidMember);
	void CreateFixedParty(int nFixedPartyID);

	int GetWaitingPartyCount(void);
	int GetWaitingMemberCount(int nFixedPartyID);

private:
	waiting_map	m_mapWaiting;
	ZPartyCommandRouter* m_pRouter;
};
