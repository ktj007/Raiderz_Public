#pragma once

class GField;
class GEntityPlayer;
class GParty;

class GAutoPartyMgr : public MTestMemPool<GAutoPartyMgr>
{
public:
	typedef vector<PlayerUID>	PlayerUIDList;
	typedef vector<PartyUID>	PartyUIDList;
	typedef vector<GEntityPlayer*> PlayerList;
	typedef set<PlayerUID> PlayerUIDSet;
	typedef set<PartyUID> PartyUIDSet;

	struct WaiterGroup
	{
		PlayerUIDSet	setSingles;
		PartyUIDSet		setParties;
	};
public:
	GAutoPartyMgr(GField* pField);
	~GAutoPartyMgr(void);
	bool Enqueue( QuestID nQuestID, GEntityPlayer* pPlayer );
	bool Dequeue( QuestID nQuestID, GEntityPlayer* pPlayer );
	AUTOPARTY_STATE GetAutopartyState( int nQuestID, GEntityPlayer* pPlayer );
	void ChangeAutopartyState(GEntityPlayer* pPlayer, int nQuestID, AUTOPARTY_STATE nState);

	void Clear();
	void EraseSingle(PlayerUID uidPlayer);

	void CheckMatch();

	void Update(float fDelta);

	GAutoPartyMgr::WaiterGroup* GetEntries( QuestID nQuestID );
	int GetSingleCount(QuestID nQuestID) const;
	int GetSingleCountAll() const;
	int GetPartyCount(QuestID nQuestID) const;
	int GetPartyCountAll() const;

	static void Cooking();
	static void ResetStatics();
	static int GetPartyMemberLimit(QuestID nQuestID);
	static int GetNeedHealerCount(int nMemberLimit);

	void ProcessMatchedSingles( QuestID nQuestID );
	void ProcessMatchedParties( QuestID nQuestID );
	void ProcessMatching( QuestID nQuestID );
	void ProcessAllMatchings();

	void RouteCheckSensor( GEntityPlayer* pPlayer, int nQuestID, AUTOPARTY_STATE nState );
private:
	void OnEnqueue( QuestID nQuestID, GEntityPlayer* pPlayer );

	void OnDequeue( QuestID nQuestID, GEntityPlayer* pPlayer );
	void OnSinglesMatched( QuestID nQuestID, PlayerUIDList setEntries );
	void OnPartyMemberAdded( GParty* pParty, QuestID nQuestID, PlayerUIDList setEntries );

	GEntityPlayer* FindPlayer( const PlayerUID uidPlayer );
	void TestNarration(GEntityPlayer* pPlayer, const wchar_t* pFormat, ...);

	bool EnqueueSingle( QuestID nQuestID, GEntityPlayer* pPlayer );
	bool EnqueueParty( QuestID nQuestID, GEntityPlayer* pPlayer );
	bool DequeueSingle( QuestID nQuestID, GEntityPlayer* pPlayer );
	bool DequeueParty( QuestID nQuestID, GEntityPlayer* pPlayer );
	bool ProcessMatchedImpl( QuestID nQuestID, PlayerUIDList& listMatchedPlayers, int nExistMemberCount = 0, int nExistHealerCount = 0 );
private:
	typedef map<QuestID, WaiterGroup>	WaiterMap;
	typedef map<QuestID, int>			PartymemberCountMap;
		
	static PartymemberCountMap s_mapPartymemberCount;

	GField* m_pField;
	WaiterMap m_maptWaiters;
	MRegulator m_rgrMatchProcess;
};
