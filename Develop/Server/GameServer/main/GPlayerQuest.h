#ifndef _G_PLAYER_QUEST_H_
#define _G_PLAYER_QUEST_H_

class GPlayerQObjective;
class GGlueQuest;
class GNullGlueQuest;
class GQuestInfo;
class GEntityPlayer;

enum QUEST_STATE_TYPE;

typedef map<int, GPlayerQObjective*>	MAP_PLAYERQOBJECTIVE;

class GPlayerQuest : public MTestMemPool<GPlayerQuest>
{
private:
	GQuestInfo*		m_pQuestInfo;
	GEntityPlayer*	m_pOwner;

	uint8	m_nSlotID;
	MAP_PLAYERQOBJECTIVE	m_mapPlayerQObjective;		///< 퀘스트의 목적 달성 상태
	int		m_nVar;										///< 퀘스트 여러군데에서 사용되는 변수
	QUEST_STATE_TYPE		m_nState;					///< 퀘스트 상태
	uint32	m_nAcceptTime;								///< 퀘스트 시작 시간
	uint32	m_nLimitTime;								///< 퀘스트 완료 제한 시간
	wstring m_strAcceptDt;								///< 퀘스트 받은 시간.
	wstring m_strExpiDt;								///< 퀘스트 만료 시간.

	GGlueQuest*	m_pDelegator;

public:
	typedef GNullGlueQuest NullType;

	GPlayerQuest();										///< Glue에 바인딩 하기 위한 생성자
	GPlayerQuest(GQuestInfo* pQuestInfo, GEntityPlayer* pOwner, const uint8 nSlotID, uint32 nAcceptTime, const wstring& strAcceptDt, const wstring& strExpiDt);
	virtual	~GPlayerQuest();

	void Clear();
	MAP_PLAYERQOBJECTIVE& GetQObjectiveContainer() { return m_mapPlayerQObjective; }
	GQuestInfo*	 GetInfo()					{ return m_pQuestInfo; }
	GEntityPlayer* GetOwner()				{ return m_pOwner; }
	void Update(float fDelta);

	GPlayerQObjective* Get(int nObjectiveID);
	
	void UpdateQuestVar(int nVar);
	int  GetQuestVar() { return m_nVar; }

	void UpdateQObjective(int nObjectiveID, int nProgress, bool& bChangeToComplete, bool& bChageToIncomplete);	///< 목적 달성 상태를 변경 한다.
	bool IsQObjectiveUpdatableVar(int nObjectiveID, int nVar, int& nUpdateProgress);							///< 목적 달성 상태를 변경 시킬만한 값인지?

	void AllQObjectiveComplete();
	bool IsAllQObjectiveComplete();	

	void Fail();
	
	void Complete();
	void Incomplete();

	bool IsComplete();
	bool IsFail();
	bool IsActive();

	int GetQuestNeedItemQuantity(int nItemID);
	vector<pair<int,INT>> GetQuestNeedItem();

	uint32	GetAcceptTime() { return m_nAcceptTime; }
	uint32	GetLimitTime() { return m_nLimitTime; }	
	QUEST_STATE_TYPE GetState();
	
	void CreateDelegator();	
	GGlueQuest* GetDelegator(void);

	uint8 GetSlotID();
	wstring& GetAcceptDt();	
	wstring& GetExpiDt();
};



#endif//_G_PLAYER_QUEST_H_