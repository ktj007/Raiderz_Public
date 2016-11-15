#pragma once

class GParty;
class GEntityPlayer;

/// 파티 매니져
class GPartyManager : public MTestMemPool<GPartyManager>
{
	typedef tr1::unordered_map<uint64, GParty*>		PARTY_MAP;		// MUID, GParty*
public:
	GPartyManager() {}
	virtual ~GPartyManager();

	virtual GParty* NewParty(MUID uidParty);
	void AddParty(GParty* pParty);
	void DeleteParty(const MUID& uidParty);
	void Clear();

	GParty* FindParty(MUID uidParty);
	GParty* FindParty( GEntityPlayer* pReqPlayer );
private:	
	PARTY_MAP		m_mapEntries;
};
