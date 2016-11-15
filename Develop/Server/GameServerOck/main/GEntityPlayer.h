#ifndef _GENTITY_PLAYER_H
#define _GENTITY_PLAYER_H

#include "GEntitySync.h"
#include "GEntityActor.h"
#include "CTransData.h"
#include "MTime.h"
#include "CSChrInfo.h"
#include "GPlayerPassiveTalents.h"
#include "MMemPool.h"
#include "GDynamicField.h"
#include "GActorMeshInfo.h"
#include "GPlayerField.h"
#include "GPlayerCRTLogger.h"

class GEntityPlayer;
class GTalentInfo;
class GCalculator;
class GPartySystem;
class GTradeSystem;
class GBuffInfo;
class GNPCInteractionElementInfo;
class GNPCShopInfo;
class GPlayerInteraction;
class GPlayerEnemyInfoSender;
class GDuel;
class GPalette;
class GPlayerTalent;
class GDynamicFieldMaster;
class GNullGluePlayer;
class GGluePlayer;
class GItemExpirer;
class GPlayerQuests;
class GPlayerFalling;
class GPlayerBattleArena;
class GDynamicFieldPolicy_TrialField;
class GPlayerEnemyInfoSender;
class GPlayerLoot;
class GPlayerFactions;
class GEntityPlayerDBAsyncMonitor;
class GPlayerPVPArea;
class GPlayerPVPChecker;
class GWarpInfoCollector;
class GPlayerCombatChecker;
class GPlayerSensorCacheUpdater;
class GPlayerDoing;
class GPlayerStance;
class GPlayerCutscene;
class GPlayerRecipe;
class GPlayerTargetInfoSender;
class GItemHolder;
class GPlayerEmblem;
class GPlayerTrade;
class GCheatChecker;
class GPlayerNPCIconSender;
class GPlayerObject;
class GPlayerMailBox;
class GPlayerEnchant;
class GPlayerMailReserver;
class GPlayerDye;
class GQPKillRewardOwner;
class GQPKillRewarder;
class GPlayerRole;
class GPlayerAFK;
class GPlayerSpawnedNPC;
class GPlayerSit;


struct GDBCACHEDATA_CHARACTER;

class GTimeSynchor
{
private:
	MRegulator	m_rgrInvterval;
public:
	GTimeSynchor();
	void Update( float fDelta, GEntityPlayer* pPlayer );
	void RouteTimeSynch(GEntityPlayer* pPlayer);
	void SetActive(bool bActive);
};

////////////////////////////////////////////////////////////////

class GPlayTimeUpdater
{
protected :
	float m_fPlayTimeDelta;

public :
	GPlayTimeUpdater();
	virtual ~GPlayTimeUpdater() {}

	void Update(const float fDelta, PLAYER_INFO* pPlayerInfo);
};

//////////////////////////////////////////////////////////////////////////
class GPlayerFieldListener
{
public:
	// 필드이동이 실행될때
	virtual void OnGateExecute(GField* pField, vec3 vPos, vec3 vDir)				{}
	// 필드이동이 완료될때
	virtual void OnGateComplete(GField* pField, vec3 vPos, vec3 vDir)				{}
	// 동적필드에 진입할때
	virtual void OnEnterDynamicField(GDynamicField* pField, vec3 vPos, vec3 vDir)	{}
	// 공유필드에 진입할때
	virtual void OnEnterSharedField(GSharedField* pField, vec3 vPos, vec3 vDir)		{}
	// 같은필드에서 순간 이동할떄
	virtual void OnWarp(vec3 vPos, vec3 vDir)										{}
};

class GPlayerTDCacheInfoBuilder;

struct PLAYER_DIE_INFO
{
	PLAYER_DIE_INFO()
	{
		nNpcID		= 0;
		uidKiller	= 0;
		nCode		= 0;
		nKillerCode	= 0;
	}

	int		nNpcID;
	MUID	uidKiller;
	int		nCode;
	int		nKillerCode;
};

////////////////////////////////////////////////////////////////
/// 실제 플레이어 개체
class GEntityPlayer : public GEntityActor, public GPlayerFieldListener, public MMemPool<GEntityPlayer>
{
friend class GPlayerSetterForTest;

protected:
	friend GCalculator;
	friend class GPlayerFieldEntry;
	friend class GPlayerTDCacheInfoBuilder;

	GActorMeshInfo				m_MeshInfo;

	PLAYER_INFO*				m_pPlayerInfo;
	GPlayerQuests*				m_pQuests;
	GPlayerTalent*				m_pTalent;
	GPlayerInteraction*			m_pInteraction;
	GPalette*					m_pPalette;
	CSPlayerModifier			m_PlayerModifier;

	GItemExpirer*				m_pItemExpirer;

	GItemHolder*				m_pItemHolder;

	GPlayerEnemyInfoSender*		m_pEnemyInfoSender;
	GPlayerTargetInfoSender*	m_pTargetInfoSender;
	GPlayerFalling*				m_pPlayerFalling;	
	GPlayerLoot*				m_pPlayerLoot;
	GPlayerFactions*			m_pPlayerFactions;
	GEntityPlayerDBAsyncMonitor* m_pDBAsyncMonitor;
	GPlayerPVPArea*				m_pPvPArea;
	GPlayerPVPChecker*			m_pPvPChecker;
	GPlayerCombatChecker*		m_pCombatChecker;
	GPlayerCutscene*			m_pCutscene;
	GPlayerRecipe*				m_pPlayerRecipe;
	GPlayerEmblem*				m_pPlayerEmblem;
	GPlayerNPCIconSender*		m_pNPCIconSender;
	GPlayerMailBox*				m_pMailbox;
	GPlayerMailReserver*		m_pMailReserver;
	GPlayerEnchant*				m_pPlayerEnchant;
	GPlayerDye*					m_pDye;
	GQPKillRewardOwner*	m_pQPKillRewardOwner;
	GQPKillRewarder*			m_pQPKillRewarder;
	GPlayerRole*				m_pRole;
	GPlayerAFK*					m_pAFK;
	GPlayerSit*					m_pSit;
	GPlayerSpawnedNPC*			m_pSpawnedNPC;

	GPlayerSetterForTest*		m_pPlayerSetterForTest;
	
	GPlayerStance*				m_pStance;
	GPlayerDoing*				m_pDoing;							///< 현재 하고 있는 행위

	GPlayerPassiveTalent		m_PassiveTalents;

	int							m_nCastingTalentID;

	// 어떤 공격을 맞고 죽었나?
	GTalentInfo*				m_pKillTalentInfo;

	// 파티
	MUID						m_uidParty;							///< 등록되어있는 파티 UID
	MUID						m_uidInvitor;						///< 초대해준 플레이어 UID
	MUID						m_uidInvitee;						///< 가입요청한 플레이어 UID
	bool						m_bPartyLeader;
	bool						m_isMoveServer;						///< 서버이동 시작

	// 결투
	GDuel*						m_pDuel;

	// 투기장
	GPlayerBattleArena*			m_pPlayerBattleArena;

	// 필드 이동
	GPlayerField*				m_pPlayerField;

	// 센서
	GPlayerSensorCacheUpdater*	m_pSensorCacheUpdater;

	int							m_nTriggedSensorID;					///< 목적지가 중복돼 선택된 센서	
	GWarpInfoCollector*			m_pWarpInfoCollector;				///< 센서의 워프목록 조건 확인 및 보관용
	int							m_nFieldGroupID;					///< 필드 따라가기 참조용

	MRegulator					m_DecFatigueRegulator;				///< 피로도 감소 타이머	
	GTimeSynchor				m_TimeSynchor;

	//set<GDynamicFieldPolicy_TrialField*>	m_setFieldObservers;
	set<MUID>					m_setTrialField;
	
	GCheatChecker*				m_pCheatCheck;

	// 싱크
	vec3						m_vReservedPosSync;

	static GPlayerCRTLogger		m_PlayerCRTLogger;

	GPlayTimeUpdater			m_PlayTimeUpdater;

	bool						m_isMovedFromOtherGameServer;


	bool						m_bInteractionCheck;	


	// 채팅 메시지
	uint32						m_nLastAuctionMsgTime;


protected:
	GPlayerTrade*				m_pPlayerTrade;
protected:
	virtual void OnEnter() override;
	virtual void OnLeave() override;

public:
	virtual MCommand* CreateFieldInCmd() override;
	virtual MCommand* CreateFieldOutCmd() override;
private:
	virtual void OnVisitedNewSectorPC(const MUID& newSightedPC,MCommand* const fieldInCmd) override;
	virtual void OnVisitedNewSectorNPC(const MUID& newSightedNPC,MCommand* const fieldInCmd) override;
	virtual void OnVisitedOldSectorNPC(const MUID& oldSIghtedNPC,MCommand* const fieldOutCmd) override;
	virtual void OnSectorChanged(const MPoint& oldSectorPt) override;

	virtual void OnUpdate(float fDelta);

	virtual void	RouteInEntity(const vector<MUID>& vecPlayers) override;
	virtual void	RouteOutEntity(const vector<MUID>& vecPlayers) override;

	virtual MCommand* MakeCmd_Whois(GEntitySync* pReqSync) override;
	
	// 센서 관련 -----------------
	virtual bool _HandleAutoWarpTrigger(const SENSOR_INFO& infoSensor);
	virtual bool _HandleInnroomWarpTrigger(const SENSOR_INFO& infoSensor, GATE_TYPE eGateType);
	virtual bool _HandleCheckpointTrigger(const SENSOR_INFO& infoSensor);	
			void _HandleMultipleWarpTrigger(void);
			bool _HandleSingleWarpTrigger(int nWarpInfoCollectorIndex, int nSensorID, int nExitMarkerID);
			bool _GetMarkerFromSensorTrigger(const unsigned int nSensorID, const SENSOR_WARP_INFO& info, const MARKER_INFO*& pMarkerInfo);
			void _ClearTriggerHistory();
	virtual void _HandleQuestTrigger(const SENSOR_INFO& infoSensor);	
	bool HandleManualWarpTrigger(const SENSOR_INFO& infoSensor);	
	void RouteWarpFullHouseError(void);

	virtual GGlueActor*	NewDelegator() override;

	virtual void OnRebirth() override;
	virtual void OnRebirthBySelf() override;
	virtual void OnRebirthByOther() override;

public:
	unsigned int				m_nLastPrepareAttackTime;
	void RefreshStatusFromLevelup();
public:
	typedef GNullGluePlayer NullType;

	GEntityPlayer();
	virtual ~GEntityPlayer();

	virtual bool Create(MUID& uid);
	virtual void Destroy();

	int64 GetAID() const;

	virtual const wchar_t* GetName() const override;

	void GameStart();
	
	void GetSimpleStatus(int8& nHP, int8& nEN, int8& nSTA) const;
	bool	IsStayDynamicField();

	virtual void OnUpdatedStatus();
			void OnSynchQuest();
			void OnQuestComplete(int nQuestID);
			void OnQuestDrop(int nQuestID);

	virtual float GetSpellPower() const override;

	bool IsDeveloper() const;
	bool IsGM() const;
	bool IsGod() const;

	// 기본 전투 관련 ------------------
	virtual void doMakePeace() override;	
	virtual bool IsNowCombat() const override;
	virtual bool IsHittable( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo );
	void SetReservedPosSync(vec3& vPos) { m_vReservedPosSync = vPos; }
	void SyncMovingMotionFactorFinishPos(vec3& vPos);

	virtual bool doGuard(int nGuardTalentID);
	virtual bool doGuardReleased();	
	virtual void doTryHit(GEntityActor* pAttacker, GTalentInfo* pTalentInfo, int nAddtiveDamage, uint16 nCheckTime=0, int nCapsuleGroup=INVALID_ID, int nCapsuleIndex=INVALID_ID);
	virtual bool doDamage(MUID uidAttacker, DAMAGE_ATTRIB nDamageAttrib, uint32 nDamage, EFFECT_SOURCE_TYPE nEffectSourceType = EST_NONE, int nEffectSourceID = INVALID_ID) override;	// return : 피해를 입고 죽었는가
	virtual void doDie();

	virtual float DefaultWalkSpeed() const override;

	void SetDie();

	void		 SetKillTalent(GTalentInfo* pTalentInfo)			{ m_pKillTalentInfo = pTalentInfo; }
	GTalentInfo* GetKillTalent() const				{ return m_pKillTalentInfo; }

	virtual bool DoScheduleMove();
	virtual void OnEndMotionFactor();

	// 전투 판정 이벤트
	virtual void OnHit(GHitInfo infoHit, bool bRouteHit) override;
	virtual void OnHitAll(GTalentInfo* pTalentInfo) override;
	virtual void OnHitSuccess(GTalentInfo* pTalentInfo) override;
	virtual void OnHitEnemy(uint32 nCombatResultFalg, GEntityActor* pVictim, GTalentInfo* pTalentInfo) override;
	virtual void OnMiss(uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override;
	virtual void OnMissEnemy(uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo) override;
	virtual void OnGuard(GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override;
	virtual void OnPerfectGuard(GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override;
	virtual void OnAbsoluteGuard(GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo) override;
	virtual void OnGuardEnemy(GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo) override;
	virtual void OnMagicHit(GEntityActor* pAttacker, GTalentInfo* pTalentInfo) override;
	virtual void OnMagicHitEnemy(GEntityActor* pTarget, GTalentInfo* pTalentInfo) override;
	virtual void OnMagicActTalent( GTalentInfo* pTalentInfo ) override;
	virtual void OnMeleeHit(GEntityActor* pAttacker, GTalentInfo* pTalentInfo) override;	
	virtual void OnMeleeHitEnemy(GEntityActor* pTarget, GTalentInfo* pTalentInfo) override;	

	// 수영 ----------------
	void doSwimming();
	void doSwimmingRelease();
	
	// 아이템 관련 ----------------
	virtual bool IsEquipShield() const override;
	
	// 피해정보를 탤런트에 근거하여 계산후 반환, 탤런트정보가 없다면 액터단위로 계산
	virtual DAMAGE_ATTRIB GetDamageType( const GTalentInfo* pTalentInfo=NULL )  override;

	// 탤런트 관련 ----------------
	bool HasPassiveTalent(TALENT_EXTRA_PASSIVE_TYPE nType);
	bool HasPassiveTalent(TALENT_EXTRA_PASSIVE_TYPE nType, int nExtraPassiveLevel);
	int GetPassiveValue(TALENT_EXTRA_PASSIVE_TYPE nType) const { return m_PassiveTalents.GetPassiveValue(nType); }
	int GetPassiveRank(TALENT_EXTRA_PASSIVE_TYPE nType) { return m_PassiveTalents.GetRank(nType); }	
	virtual bool doUseTalent(GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target = TALENT_TARGET_INFO::Invalid(), bool bCheckEnabled=true, bool bGainStress=true) override;
	virtual bool doUseTalent(int nTalentID, TALENT_TARGET_INFO Target = TALENT_TARGET_INFO::Invalid(), bool bCheckEnabled=true, bool bGainStress=true) override;
	virtual void OnFinishTalent(GTalentInfo* pTalentInfo);

	// 패시브 효과를 얻음, 패시브 탤런트가 아니라면 무시
	void GainPassiveTalentEffect(GTalentInfo* pTalentInfo);
	// 패시브 효과를 잃음, 패시브 탤런트가 아니라면 무시
	void LostPassiveTalentEffect(GTalentInfo* pTalentInfo);

	// 버프 관련
	void GetBuffRemainTime(vector<REMAIN_BUFF_TIME>& outvecBuffRemainTime, bool bExceptFocusBuff=false, bool bExceptNoSaveDB = false);
	void InsertBuffRemainTime(const vector<REMAIN_BUFF_TIME>& vecBuffRemainTime);
	
	// 파티 관련 ----------------
	bool HasParty();
	MUID GetPartyUID() const;
	void SetParty(MUID val);
	bool IsPartyLeader();
	bool SetPartyLeader(bool bPartyLeader);
	MUID GetPartyInvitorUID() const { return m_uidInvitor; }
	void SetPartyInvitorUID(MUID val) { m_uidInvitor = val; }
	MUID GetPartyInviteeUID() const { return m_uidInvitee;	}
	void SetPartyInviteeUID(MUID val) { m_uidInvitee = val;	}
	void SetMoveServer(bool isMoveServer)	{ m_isMoveServer = isMoveServer;	}
	bool IsMoveServer(void)					{ return m_isMoveServer;	}

	// 결투 관련
	bool	HasDuel() const			{ return m_pDuel!=NULL; }
	GDuel*	GetDuel()				{ return m_pDuel; }
	void	SetDuel(GDuel* val)		{ m_pDuel = val; }
	// 센서 관련 -----------------
	void UpdateUsableSensor(bool bRoute=true);
	void SelectWarpTrigger(int nSelection);
	void SensorInteraction(vector<uint32>& vecSensorID);	
	int	 GetWarpTriggerHistoryQty(void);
	void SetFollowFieldGroupID(int nFieldGroupID);
	int GetFollowFieldGroupID(void) const;
	void RouteFollowMeTrialField(MUID uidStartField, int nFieldGroupID, int nReceiveLimitCount);

	// 트레이드 관련 ----------------
	GPlayerTrade& GetPlayerTrade()			{ return *m_pPlayerTrade; }	

	// TransData 구조체를 만들어 반환
	void MakeTDCacheInfo(TD_UPDATE_CACHE_PLAYER& outTDUpdateCachePlayer);	
	void MakeDBCacheData(GDBCACHEDATA_CHARACTER& data);

	// 메세지 관련 ----------------
	virtual void RouteToMe(MCommand* pCommand) const;	
	virtual void RouteToParty(MCommand* pCommand, const MUID& uidExceptMember=MUID::ZERO);	
	bool FailAndRouteSystemMsg(CCommandResultTable nRet);
	void RouteSystemMsg(CCommandResultTable nRet);
	bool FailAndRouteAdviceMsg(CCommandResultTable nRet);
	void RouteAdviceMsg(CCommandResultTable nRet);
	void RouteGuard();
	void RouteGuardFailed();
	virtual void CollectEssentialReceiver( vector<MUID>& vecReceiverUID ) override;
	
	// 투명화상태인지 여부
	virtual bool IsNowInvisibility() const override;
	bool IsNowGhost() const;

	// 인터랙션 관련 ---------------
	GPlayerInteraction&		GetInteraction()		{ assert(m_pInteraction); return *m_pInteraction; }

	// 피로도 관련 ---------------
	MRegulator&				GetDecFatigueRegulator(){ return m_DecFatigueRegulator; }

	// 파인드 관련 -------------------------
	GEntity* FindEntity(const MUID& uidEntity);
	GEntityPlayer* FindPlayer(const MUID& uidPlayer);
	GEntityNPC* FindNPC(const MUID& uidNPC);
	GEntityNPC* FindNPC(const UIID nNPCUIID);
	GEntityActor* FindActor(const MUID& uidActor);
	void GetNeighborNPC(const float fRange, vector<GEntityNPC*>& vecOutNeighborNPC);

	// Attribute Modifier
	virtual int GetMaxHPProto() const override;
	virtual int GetMaxENProto() const override;
	virtual int GetMaxSTAProto() const override;
	int GetSTRProto() const { return m_pPlayerInfo->nSTR + m_pPlayerInfo->nLevel-1; }
	int GetDEXProto() const { return m_pPlayerInfo->nDEX + m_pPlayerInfo->nLevel-1; }
	int GetINTProto() const { return m_pPlayerInfo->nINT + m_pPlayerInfo->nLevel-1; }
	int GetCHAProto() const { return m_pPlayerInfo->nCHA + m_pPlayerInfo->nLevel-1; }
	int GetCONProto() const { return m_pPlayerInfo->nCON + m_pPlayerInfo->nLevel-1; }

	int GetSTR() const { return m_PlayerModifier.nSTR.Calc(GetSTRProto()); }
	int GetDEX() const { return m_PlayerModifier.nDEX.Calc(GetDEXProto()); }
	int GetINT() const { return m_PlayerModifier.nINT.Calc(GetINTProto()); }
	virtual int GetCHA() const { return m_PlayerModifier.nCHA.Calc(GetCHAProto()); }
	int GetCON() const { return m_PlayerModifier.nCON.Calc(GetCONProto()); }
	virtual int GetAP() const override;
	CSPlayerModifier&	GetPlayerModifier()		{ return m_PlayerModifier; }

	void ApplyAllPassiveTalents();

	// 캐릭터 상태 관련 -------------------------
	bool		IsStanceChangable();

	// 무적상태인지 여부
	virtual  bool IsNowInvincibility() override;
	// NPC에게 투명상태인지 여부
	virtual  bool IsNowInvisibilityToNPC() override;

	// 이동되는 모션팩터(넉백/쓰로우업등)가 적용중인가?
	bool		HasMovingMotionfactor();	
	// 모션팩터가 적용중인가?
	bool		HasMotionfactor();
	// 테스트 모션팩터 문자열 출력
	wstring		GetMotionFactorString();

	// 특정 포커스가 걸려있는지 여부
	virtual bool HasFocus(TALENT_FOCUS_TYPE nFocus);
	// 포커스가 하나라도 걸려있는지 여부
	virtual bool HasFocus();
	// 광포 버블 갯수 반환
	int GetBerserkBubble();

	// == 안티치팅 관련 ===========================================
	// 플레이어가 걸어서 이동한 다음에 꼭 호출해야함
	void		CheatCheck_Walk();
	// 플레이어가 워프로 이동한 다음에 꼭 호출해야함
	void		CheatCheck_Warp();
	// 플레이어가 모션팩터로 이동한 다음에 꼭 호출해야함
	void		CheatCheck_MotionFactorMove();

	// 테스트용 행동을 수행
	void		RunDebugAction();

	// 기타 GetSet ----------------
	PLAYER_INFO*					GetPlayerInfo() const				{ return m_pPlayerInfo; }
	GPlayerGrade					GetPlayerGrade() const				{ assert(m_pPlayerInfo); return m_pPlayerInfo->nPlayerGrade; }
	void							SetPlayerGrade(GPlayerGrade nGrade)	{ assert(m_pPlayerInfo); m_pPlayerInfo->nPlayerGrade = nGrade; }
	SBID							GetSoulBindingID() const			{ assert(m_pPlayerInfo); return m_pPlayerInfo->nSoulBindingID; }
	GPlayerQuests&					GetQuests()							{ assert(m_pQuests); return *m_pQuests; }
	GPalette&						GetPalette() 						{ assert(m_pPalette); return *m_pPalette; }
	GPlayerTalent&					GetTalent()							{ assert(m_pTalent); return *m_pTalent; }

	GItemHolder*					GetItemHolder()						{ _ASSERT(m_pItemHolder); return m_pItemHolder;	}
	const GItemHolder*				GetItemHolder()	const				{ _ASSERT(m_pItemHolder); return m_pItemHolder;	}	
	GItemExpirer*					GetItemExpirer()					{ return m_pItemExpirer; }
	GPlayerFalling*					GetPlayerFalling()					{ return m_pPlayerFalling; }
	GPlayerField&					GetPlayerField()					{ assert(m_pPlayerField); return *m_pPlayerField; }
	GPlayerBattleArena&				GetPlayerBattleArena()				{ assert(m_pPlayerBattleArena); return *m_pPlayerBattleArena; }	
	GPlayerLoot&					GetPlayerLoot()						{ assert(m_pPlayerLoot); return *m_pPlayerLoot; }	
	GPlayerFactions&				GetPlayerFactions()					{ assert(m_pPlayerFactions); return *m_pPlayerFactions; }	
	GEntityPlayerDBAsyncMonitor&	GetDBAsyncMonitor()					{ _ASSERT(m_pDBAsyncMonitor); return *m_pDBAsyncMonitor; }
	GPlayerPVPArea&					GetPlayerPVPArea()					{ _ASSERT(m_pPvPArea); return *m_pPvPArea; }
	GPlayerCombatChecker&			GetPlayerCombatChecker()			{ _ASSERT(m_pCombatChecker); return *m_pCombatChecker; }	
	static GPlayerCRTLogger&		GetPlayerCRTLogger()				{ return m_PlayerCRTLogger; }	
	GPlayerPassiveTalent&			GetPlayerPassiveTalent()			{ return m_PassiveTalents; }
	GPlayerCutscene&				GetCutscene()						{ return *m_pCutscene; }
	GPlayerDoing&					GetDoing()							{ return *m_pDoing; }
	GPlayerRecipe&					GetPlayerRecipe()					{ return *m_pPlayerRecipe; }
	GPlayerTargetInfoSender&		GetTargetInfoSender()				{ return *m_pTargetInfoSender; }
	GPlayerEmblem&					GetEmblem()							{ return *m_pPlayerEmblem; }
	GPlayerSetterForTest&			GetPlayerSetterForTest()			{ return *m_pPlayerSetterForTest; }
	GPlayerNPCIconSender&			GetNPCIconSender()					{ return *m_pNPCIconSender; }
	GPlayerMailBox&					GetMailbox()						{ return *m_pMailbox; }
	GPlayerMailReserver&			GetMailReserver()					{ return *m_pMailReserver;	}
	GPlayerEnchant&					GetEnchant()						{ return *m_pPlayerEnchant; }
	GPlayerDye&						GetDye()							{ return *m_pDye;	}
	GQPKillRewardOwner&		GetQPKillRewardOwner()				{ return *m_pQPKillRewardOwner;	}
	GQPKillRewarder&			GetQPKillRewarder()					{ return *m_pQPKillRewarder;	}
	GPlayerRole&					GetRole()							{ return *m_pRole; }
	GPlayerAFK&						GetAFK()							{ return *m_pAFK; }
	GPlayerSit&						GetSit()							{ return *m_pSit; }
	GPlayerSpawnedNPC&				GetSpawnedNPC()						{ return *m_pSpawnedNPC; }
	GPlayerEnemyInfoSender&			GetEnemyInfoSender()				{ return *m_pEnemyInfoSender; }
	GPlayerStance&					GetStance()							{ return *m_pStance; }
	
	void							PostDieLog();
	void							GetKillerIDAndBasicCode(PLAYER_DIE_INFO& dinfo);
	void							GetPVPCode(PLAYER_DIE_INFO& dinfo);
		
	int GetCID() const;
	int GetGID() const;
	virtual GActorMeshInfo* GetMeshInfo();	
	bool	IsSameName(GEntityPlayer* pOtherPlayer);
	bool	IsSameName(const wchar_t* szName);	
	void SetTimeSynchorActive(bool bActive) { m_TimeSynchor.SetActive(bActive); }
	virtual float GetStandupTime() override;


	void AttachFieldObserver(MUID uidTrialField);
	void DetachFieldObserver(MUID uidTrialField);

	// 적극적인 행동이 가능한지 여부
	virtual bool IsBeginUnableAction() override;

	// Functions for Lua	
	GGluePlayer* GetDelegator();

	bool UpdateQuestVar(int nQuestID, int nVar);
	int	GetQuestVar(int nQuestID);
	bool AddQuest(int nQuestID);
	void RuinQuest(int nQuestID);
	bool BindSoul(uint8 nSoulBindingID);
	bool CheckCondition(int nConditionID);

	void SetFatigue(int nPercent);
	
	bool AddMoney(int nMoney);
	bool RemoveMoney(int nMoney);
	int GetMoney();
	int GetStorageMoney();
		
	virtual void Warp(vec3 vecPos, vec3 vecDir, bool bRouteToMe=true) override;

	virtual void Narration(const wchar_t* szText) override;
	virtual void NarrationNow(const wchar_t* szText) override;
	void Tip(const wchar_t* szText);
	
	// 게이트 관련 콜백
	virtual void OnGateExecute(GField* pField, vec3 vPos, vec3 vDir) override;
	virtual void OnGateComplete(GField* pField, vec3 vPos, vec3 vDir) override;
	virtual void OnEnterDynamicField(GDynamicField* pField, vec3 vPos, vec3 vDir) override;
	virtual void OnEnterSharedField(GSharedField* pField, vec3 vPos, vec3 vDir) override;
	virtual void OnWarp(vec3 vPos, vec3 vDir) override;

	bool IsMovedFromOtherGameServer(void);
	void OnMovedFromOtherGameServer(void);

	void OnLeaveFromParty();

	void DisableICheck()	{ m_bInteractionCheck = false; }
	void EnableICheck()		{ m_bInteractionCheck = true; }
	bool IsDisableICheck()	{ return !m_bInteractionCheck; }	

	// 채팅 메시지
	bool CheckAllowAuctionMsgTime(uint32 nNowTime);

private:
	bool SetSensorExitMarker(const GFieldInfo* pFieldInfo, int nMarkerID, CHANNELID nChannelID);
	virtual void OnUseTalentFailed(int nTalentID, CCommandResultTable nFailCause) override;	
};

GEntityPlayer* ToEntityPlayer(GEntity* pEntity);


#endif
