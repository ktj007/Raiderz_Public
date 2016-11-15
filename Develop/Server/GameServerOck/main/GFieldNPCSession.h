#pragma once

class GJob;
class GEntityActor;
class GGlueNPC;
class GGlueActor;
class GJobS_Blocking;
class GJobS_Wait;
class GJobS_AddNPC;
class GJobS_RemoveNPC;
class GJobS_EndSession;
class GJobS_ChangeState;
class GJobS_MakePeace;
class GJobS_MakeCombat;

class GFieldNPCSession : public MTestMemPool<GFieldNPCSession>
{
	friend GFieldNPCSession;

	class Blocker
	{
	public:
		Blocker();

		bool IsBlocking() const;
		void NonBlocking();
		void Blocking();
	private:
		bool			m_bIsNonBlocking;
	};

public:
	class Scene : public MTestMemPool<Scene>
	{
	public:
		struct Condition
		{
			string strNextStateName;
			MUID uidNPC;
			string strConditionType;
			vector<int> vecParams;
		};
	public:
		Scene(GFieldNPCSession* pOwner);
		bool Update( float fDelta );
		const char* GetName();
		void Change( const char* szName, bool bResetJobs=true );
		void AddChangeRule(Condition& condition);
		bool HasChangeRule() const;
		size_t GetConditionCount() const;

	private:
		bool CheckAllConditions();
		bool CheckCondition(Condition& condition);

	private:
		GFieldNPCSession*	m_pOwner;
		MRegulator			m_rgrUpdateTick;
		string				m_strName;
		vector<Condition>	m_vecConditions;
	};

public:
	enum eCombatState
	{
		COMBATSTATE_NONE	= 0,
		COMBATSTATE_ALL		= 1,
		COMBATSTATE_SESSION	= 2,
	};

public:
	GFieldNPCSession(MUID uid, MUID uidField, string strName);
	~GFieldNPCSession(void);

	static GFieldNPCSession* New(string strName, MUID uidField);
	MUID GetUID() const		{ return m_UID; }
	string GetName() const	{ return m_strName; }
	void Update(float fDelta);
	void EndSession();
	void Reset();
	void ResetJobs();
	bool HasExpired() const;
	void Expired();
	bool HasCombatingNPC();
	void MakePeaceAllNPC();
	void MakeCombatAllNPC();
	void ResetStatusAllNPC();
	size_t GetScneneConditionCount();
	
	bool IsHittable( GEntityActor* pAttacker, GEntityActor* pVictim ) const;
	bool IsThinkable() const;
	void ChangeCombatState(int nState);
	int GetCombatState();
	bool IsFirstStrike() const;

	// External Events =====================
	void OnJobFinished( const GJob* pJob );
	// Job =================================
	void PushJob(GJob* pJob);
	size_t GetJobQty() const;
	bool IsEmptyJob() const;
	bool HasRunningJob() const;
	const deque<GJob*>& GetScheduledJobs() const { return m_vecScheduledJobs; }

	// Blocking ============================
	bool IsBlocking() const;
	void NonBlocking();
	void Blocking();

	// Wait ===============================
	void Wait(float fDuration);
	float GetWaitRemainTime();

	// NPC °ü¸® ============================
	void AddNPC(MUID uid);
	void RemoveNPC(MUID uid);
	size_t GetNPCQty() const;
	bool HasJoinnedNPC(MUID uid) const;

	// Scene ======================
	const char* GetSceneName();
	void ChangeScene(const char* szName, bool bResetJobs=true);
	void AddChangeSceneRule(Scene::Condition& condition);

	// Bookmark ======================
	void AddBookmark(const char* szName, GGlueActor* pActor);
	GGlueActor* FindBookmark(const char* szName);

	// Glues ===============================
	void AddNPC_ForGlue(GGlueNPC* pNPC);
	void RemoveNPC_ForGlue(GGlueNPC* pNPC);
	void AddChangeSceneRule_ForGlue( const char* szNextState, GGlueNPC* pNPC, const char* szConditionType, MWLua::table tableParams );
	void ChangeState_ForGlue(const char* szName);
	void MakeCombat_ForGlue();
	void MakePeace_ForGlue();
	void CombatSession_ForGlue();
	void CombatAll_ForGlue();
	void CombatNone_ForGlue();
private:
	void AddNPCImpl(MUID uid);
	void RemoveNPCImpl(MUID uid);
	void EndSessionImpl();
	void PopJob(deque<GJob*>& vecJobs);
	void PushDoneJob( const GJob* pJob );

	bool OnProcessSessionJob( GJob* pRawJob );
	void RunJobs(deque<GJob*>& vecJobs);
	void RunCallback(string strFuncname);
	void SendFinishMsg();
private:
	MUID			m_UID;
	MUID			m_uidField;
	string			m_strName;
	deque<GJob*>	m_vecScheduledJobs;
	vector<const GJob*>	m_vecDoneJobs;
	set<const GJob*>		m_setRunningJobs;
	set<MUID>		m_setNPCs;
	Blocker			m_Blocker;
	Scene*			m_pScene;
	MRegulator		m_rgrWait;
	bool			m_bEndedSession;
	bool			m_bExpired;
	eCombatState	m_nCombatState;
	map<const char*, MUID>	m_setBookmark;
	string			m_strChangeSceneName;
};
