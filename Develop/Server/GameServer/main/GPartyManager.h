#pragma once

class GParty;
class GEntityPlayer;

/// 파티 매니져
class GPartyManager : public MTestMemPool<GPartyManager>
{
public:
	typedef map<MUID, GParty*>	PARTY_MAP;
public:
	GPartyManager() {}
	virtual ~GPartyManager();

	virtual GParty* NewParty(MUID uidParty);
	void AddParty(GParty* pParty);
	void DeleteParty(const MUID& uidParty);
	void Clear();

	GParty* FindParty(MUID uidParty);
	GParty* FindParty( GEntityPlayer* pReqPlayer );

	const PARTY_MAP& GetPartyMap() const;
private:	
	PARTY_MAP		m_mapEntries;
};
