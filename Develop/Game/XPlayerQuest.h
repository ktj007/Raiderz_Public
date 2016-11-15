#ifndef _X_PLAYER_QUEST_H_
#define _X_PLAYER_QUEST_H_

class XQuestInfo;
class XQObjectiveInfo;

// 플레이어 퀘스트의 상태(DB에서도 사용)
enum PLAYER_QUEST_STATE
{
	PQS_NONE		= 0,
	PQS_NOEXIST		= 1,	// 퀘스트를 받지 않음
	PQS_EXIST		= 2,	// 퀘스트를 받았지만 아직 비활성화 상태
	PQS_COMPLETED	= 3,	// 퀘스트를 받았고 활성화 상태

	PQS_MAX
};

/// 플레이어의 퀘스트 목적 상태
// TODO : 그냥 CSQObjectiveInfo 를 상속 받으면 안되나요? [3/18/2008 madduck]
struct XPlayerQObjective
{
public:
	XQObjectiveInfo* m_pQObjectiveInfo;

	int nProgress;
	bool bComplete;

	XPlayerQObjective(XQObjectiveInfo* pQObjectiveInfo);
};

typedef map<int, XPlayerQObjective*>	MAP_PLAYERQOBJECTIVE;


/// 플레이어의 수행중인 퀘스트 상태
class XPlayerQuest
{
public:
	XQuestInfo*		m_pQuestInfo;

	MAP_PLAYERQOBJECTIVE	m_mapPlayerQObjective;		///< 퀘스트의 목적 달성 상태
	QUEST_STATE_TYPE		m_nState;
	uint32	m_nAcceptTime;								///< 퀘스트 시작 시간
	uint32	m_nLimitTime;
	bool	bChallengerQuest;

public:
	XPlayerQuest(XQuestInfo* pQuestInfo);
	virtual	~XPlayerQuest();

	void Clear();

	XPlayerQObjective* Get(int nObjectiveID);
	bool UpdateObjective(int nObjectiveID, int nProgress, bool bCompleted);	///< 목적 달성 상태를 변경 한다.
	uint32 GetRemainTime();
	bool	IsTimeAttackType()
	{
		if (m_nLimitTime == NO_LIMIT_TIME) return false;

		return true;
	}
	bool IsComplete()
	{
		return (QST_COMPLETE == m_nState);
	}
	bool IsFail()
	{
		return (QST_FAIL == m_nState);
	}
};

typedef map<int, XPlayerQuest*>		MAP_PLAYERQUEST;

/// 플레이어가 가지고 있는 퀘스트의 상태들
class XPlayerQuests
{
public:
	MAP_PLAYERQUEST		m_mapPlayerQuest;
	void			Delete(int nQuestID);

public:
	XPlayerQuests()	{}
	virtual	~XPlayerQuests();

	void			Clear();

	XPlayerQuest*	Get(int nQuestID);
	bool			GetIndex( size_t nIndex, XPlayerQuest** ppOutQuest);

	size_t			GetCount() const		{ return m_mapPlayerQuest.size();	}

	XPlayerQuest*	Insert(int nQuestID);
	void			Cancel(int nQuestID);
	void			Reward(int nQuestID);
	
	bool			IsPlayerQuest(int nQuestID);
	const wchar_t*		GetQuestIDList(wstring& szText);
};


#endif//_X_PLAYER_QUEST_H_