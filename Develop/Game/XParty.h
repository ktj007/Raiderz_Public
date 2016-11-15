#ifndef _XPARTY_H
#define _XPARTY_H

enum MEMBER_EFFECT
{
	ME_NONE,
	ME_HIGHTLIGHT,
};

struct XPartyMember
{
public:
	MUID m_uid;
	wstring m_strName;
	uint32 nStatusFlag;
	uint32 nEffectFlag;
	int8 nHP;
	int8 nEN;
	int8 nSTA;
	int8 nLevel;

	vector<int>	nBuffIDList;

	bool IsDead();
	bool IsOffline();
	bool IsHightLight();

	XPartyMember()
		:nHP(0)
		,nEN(0)
		,nSTA(0)
		,nLevel(0)
		,nEffectFlag(0)
	{}
};

typedef map<const MUID, XPartyMember> PARTY_SET;

class XParty : private PARTY_SET
{
public:
	int				nQuestIDWithCurrentSensor;

private:
	MUID			uidParty;			//파티의 UID
	PARTY_SETTING	PartySetting;

	AUTOPARTY_STATE		m_nAutoPartyState;

private:
	void					SetMember(XPartyMember& Member,const TD_PARTY_MEMBER& tdMember);

public:
	XParty();
	virtual ~XParty();

	PARTY_SET::iterator		Begin()				{ return begin(); }
	PARTY_SET::iterator		End()				{ return end(); }
	void					Clear();
	bool					Empty()				{ return empty(); }

	//const char*				GetMemberUIDList(wstring& szText);
	bool					GetIndex( size_t nIndex, XPartyMember** ppOutMember);

	bool					Find( const MUID& uidPlayer );
	bool					Find(const MUID& uidPlayer, XPartyMember& outMember);

	void					Init(const TD_PARTY& tdPartyInfo);				// 파티정보 세팅(uidParty, uidLeader)	
	//void 					ChangeName(wstring strName);
	void 					ChangeLeader(MUID uidLeader);
	void 					ChangeLootingRule(LOOTING_RULE_DATA rule);

	int						GetMemberCounter()	{ return (int)size(); };
	void					AddMember(const TD_PARTY_MEMBER& tdMember);
	void					DeleteMember(const MUID& uidMember);
	void					ChangeStatus(const TD_PARTY_MEMBER& tdMember);

	MUID					GetUID() const		{ return uidParty; }
	MUID					GetLeader() const	{ return PartySetting.m_uidLeader; }
	bool					IsLeader();

	bool					IsPartyMemberDead(const MUID& uidPlayer);
	bool					IsPartyMemberOffline(const MUID& uidPlayer);
	bool					IsPartyMember(const MUID& uidPlayer);
	bool					IsPartyMemberHightlight(const MUID& uidPlayer);

	const wchar_t*			GetPartyMemberName(const MUID& uidPlayer);
	int						GetPartyMemberHP(const MUID& uidPlayer);
	int						GetPartyMemberEN(const MUID& uidPlayer);
	int						GetPartyMemberSTA(const MUID& uidPlayer);
	int						GetPartyMemberLevel(const MUID& uidPlayer);
	bool					IsJoined();

	void					SetSelectedMember(const MUID& uidPlayer);

	bool					SetPartyMemberBuffID(const MUID& uidPlayer, int nBuffID);
	//const char*				GetPartyMemberBuffIDList(const MUID& uidPlayer, wstring& szText);
	void					SetPartyMemberEffect(const MUID& uidPlayer, MEMBER_EFFECT eEffect);

	void					ClearPartyMemberEffect(MEMBER_EFFECT eEffect);

	bool					IsPartyMemberReconnect(const TD_PARTY_MEMBER& tdMember);	// 오프라인에서 재접속하는 파티원

	PARTY_SETTING* const	GetPartySetting()	{ return &PartySetting;	}

	void					SetPartyState(int nQuestID, int nAutoPartyState);
	AUTOPARTY_STATE			GetPartyState()	{ return m_nAutoPartyState; }

	void					QuestGive(int nQuestID);
	void					QuestCancel(int nQuestID);
	void					ChangeAutoPartyState( int nQuestID, int nAutoPartyState );
};
#endif // _XPARTY_H