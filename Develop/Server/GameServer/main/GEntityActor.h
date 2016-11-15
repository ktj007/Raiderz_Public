#ifndef _GENTITY_ACTOR_H
#define _GENTITY_ACTOR_H

#include "GEntitySync.h"
#include "CSDef.h"
#include "CSChrInfo.h"
#include "GTalentInfo.h" // for L'TALENT_TYPE'
#include "GFieldInfo.h" // for L'MARKER_INFO'
#include "MTime.h"
#include "GItem.h"
#include "GMotionFactor.h"
#include "GRegenUpdater.h"
#include "MAgent.h"
#include "GItemSystem.h"
#include "GTalentEffector.h"
#include "GRiposte.h"
#include "GImmuneHandler.h"
#include "GIndexedIDMap.h"
#include "GActorObserverMgr.h"

class GModuleCombat;
class GModuleBuff;

class GActorMeshInfo;
class GField;
class GBuffInfo;
class GActorObserver;
class GGlueActor;
class GGlueNPC;
class GNullGlueActor;
class GEnemyNPCRef;
class GActorInvisibility;
class GActorCounter;
class GActorCooltimeChecker;
class GActorHateTracker;
class GTalent;
class GActorBuffEntity;
class GActorTalent;


enum ACTION_STATE
{
	AS_NORMAL = 0,		// 평상시
	AS_MF,				// MF 적용중
	AS_USING_TALENT,	// 스킬 사용중
	AS_CHANGING_STANCE,	// 스탠스 변환중(PC만)
	AS_SWIMMING,		// 수영중
};

struct GEntityActorGuardInfo
{
	bool					bGuarding;			///< 방어중인지 여부
	int						nGuardingTalentID;	///< 방어중인 탤런트 ID
	uint32					nGuardStartTime;		///< 방어 시작 시간
	TALENT_GUARD_DIRECTION	nDirection;			///< 방어하고 있는 방향
	GEntityActorGuardInfo() : bGuarding(false), nGuardStartTime(0), nGuardingTalentID(0), nDirection(GUARD_DIRECTION_FRONT) {}
};

struct GHitInfo
{
	bool					bHitProcessed;
	int32					nDamageType;
	int32					nDamage;
	int32					nHealAmount;
	MOTION_FACTOR_GROUP*	pMotionFactorGroup;			// script에서만 사용
	uint16					nCheckTime;
	int						nCapsuleGroup;
	int						nCapsuleIndex;
	uint32 					nCombatResultFlags;
	bool					bTryGuard;
	bool					bCriticalHit;
	bool					bBackHit;
	GTalentInfo*			pTalentInfo;
	GEntityActor*			pAttacker;
	GEntityActor*			pVictim;
	GGlueActor*	pAttackerForLua;
	GGlueNPC*		pVictimForLua;	// 스크립트에서 핸들링할때 피해자는 항상 NPC이다 (부분판정은 NPC만 가능)	
	bool					bGuarded;
	int						nTalentID;

	GHitInfo(): bHitProcessed(true), nDamageType(DA_NONE), nDamage(0), nHealAmount(0), nCheckTime(0), pMotionFactorGroup(NULL), nCapsuleGroup(INVALID_ID), nCapsuleIndex(INVALID_ID), nCombatResultFlags(0L), bTryGuard(false), pTalentInfo(NULL), pAttacker(NULL), pVictim(NULL), pAttackerForLua(NULL), pVictimForLua(NULL), bCriticalHit(false), bGuarded(false), bBackHit(false), nTalentID(INVALID_ID)
	{}

	bool IsValidInfo() const
	{
		return !bHitProcessed;
	}
};

struct GMFApplyArgs
{
	MOTION_FACTOR_GROUP* pMotionFactorGroup;	
	GEntityActor* pOpponent;
	MF_STATE nNotApplyMF;
	MF_STATE nForceMF;
	int nForceMFWeight;
	bool bKnockbackDirSyncToOpponentDir;	
	float fStateMotionFactor;
	GMFApplyArgs() : nNotApplyMF(MF_NONE), nForceMF(MF_NONE), nForceMFWeight(0), pMotionFactorGroup(NULL), pOpponent(NULL), bKnockbackDirSyncToOpponentDir(false), fStateMotionFactor(1.0f) {}
	GMFApplyArgs(MOTION_FACTOR_GROUP* mfg, GEntityActor* opponent) : nNotApplyMF(MF_NONE), nForceMF(MF_NONE), nForceMFWeight(0), pMotionFactorGroup(mfg), pOpponent(opponent), bKnockbackDirSyncToOpponentDir(false), fStateMotionFactor(1.0f) {}
};

struct GMFApplyReturnValue
{
	MF_STATE nMotionFactor;
	int nWeight;
	GMFApplyReturnValue(MF_STATE mf, int weight) : nMotionFactor(mf), nWeight(weight) {}
};

/// 행동할 수 있는 개체 - PC, NPC
class GEntityActor : public GEntitySync, public GActorObserverMgr
{
	friend class GRegenUpdater;
	friend class GActorDamageCalculator;
protected:
	UIID					m_nUIID;			///< m_UID의 인덱스 ID

	GRegenUpdater			m_RegenUpdater;
	GModuleCombat*			m_pModuleCombat;
	GModuleBuff*			m_pModuleBuff;
	CHR_INFO*				m_pChrInfo;
	CHR_STATUS				m_ChrStatus;
	ACTION_STATE			m_nActionState;			///< 액션 상태
	GMotionFactor			m_MotionFactor;
	
	MUID					m_uidKiller;			///< 날 죽인 액터의 UID

	MRegulator				m_MFReleaser;

	bool					m_bScheduleMove;
	vec3					m_vScheduleMovePos;
	list<uint32>			m_listEnteredSensorID;
	
	GEntityActorGuardInfo	m_GuardInfo;

	bool					m_isMoving;
	bool					m_isJumping;

	bool					m_bCancelAction;
	
	GTalentEffector			m_TalentEffector;

	uint32					m_nTalentStartTime;

	// 마지막에 피격당했을때의 정보
	GHitInfo				m_LastHitinfo;
	
	// 되받아치기 관리 객체
	GRiposte				m_Riposte;
	MUID					m_uidLastHitOrHittedEnemy;	///< 가장 최근에 내가 공격을 했거나, 공격을 당한 적
	// 면역처리자
	GImmuneHandler			m_ImmuneHandler;

	// 행동불가능한 상태인지 여부
	bool					m_bNowUnableAction;

	// 투명화 처리자
	GActorInvisibility*		m_pInvisibility;
	// 카운터 처리자
	GActorCounter*			m_pCounter;
	// 쿨타임 체커
	GActorCooltimeChecker*	m_pCoolTimeChecker;
	// 헤이트 추적기
	GActorHateTracker*		m_pHateTracker;
	// 버프 엔티티 관리자
	GActorBuffEntity*		m_pBuffEntity;
	// 탤런트 관리자
	GActorTalent*			m_pActorTalent;


	GGlueActor*				m_pDelegator;
	
	bool					m_isAllowCalcVictimDodge;		///< 방어자 회피율 적용여부
	bool					m_bRegen;


	virtual void OnRebirth();
	virtual void OnRebirthBySelf();
	virtual void OnRebirthByOther();
protected:
	virtual void RegisterModules();
	virtual void UnregisterModlues();
	virtual void OnUpdate(float fDelta);

	virtual void OnUpdatedStatus() {}

	void _UpdateMotionFactor(float fDelta);
	virtual void OnChangeActionState(ACTION_STATE nNextState);
	
	// 센서 관련
	virtual bool	_HandleCheckpointTrigger(const SENSOR_INFO& infoSensor) { return false; }
	virtual bool	_HandleAutoWarpTrigger(const SENSOR_INFO& infoSensor) { return false; }
	virtual bool	_HandleInnroomWarpTrigger(const SENSOR_INFO& infoSensor, GATE_TYPE eGateType) { return false; }
	virtual void	_HandleQuestTrigger(const SENSOR_INFO& infoSensor) {}

	void _FillRegenVar();

	virtual GGlueActor*	NewDelegator();

	// 투명체일때의 자신을 발견한 플레이어들에게 메세지 전송
	void RouteToInvisibleDetectors(MCommand* pCommand, MUID uidExcept) const;
public:
	typedef GNullGlueActor NullType;

	GEntityActor();
	virtual ~GEntityActor();
	GModuleCombat*			GetModuleCombat() const	{ return m_pModuleCombat; }
	GModuleBuff*			GetModuleBuff()	 const	{ return m_pModuleBuff; }

	GGlueActor* GetDelegator();

	virtual void UpdateCombat(float fDelta);
	virtual void UpdateBuff(float fDelta);

	virtual bool Create(MUID& uid);
	virtual void Destroy();
	void CreateDelegator();
	
	virtual bool SetPos(const vec3& pos) override;

	float	DistanceTo(GEntityActor* pTarget);
	
	virtual float GetSpellPower() const { return 1.0f; }

	uint32 GetTalentStartTime()		{ return m_nTalentStartTime;	}

	// 상대방을 바라보게 하기
	void SetDirForwardToActor(GEntityActor* pTarget, bool bRoute=false);
	// 상대방을 바라보게 하기
	void SetDirForwardToActor(MUID uidTarget, bool bRoute=false);

	// 마지막에 피격당했을때의 정보 반환
	GHitInfo			GetLastHitInfo()	{ return m_LastHitinfo; }
	void				ClearLastHitInfo();

	// 적극적인 행동이 가능한지 여부
	virtual bool IsBeginUnableAction();
			bool IsEndUnableAction();
	virtual void OnBeginUnableAction();
	virtual void OnEndUnableAction();
			bool IsNowUnableAction();

	virtual void OnDestroy() override;

	bool		IsDie();
	virtual void doDie();

	void RouteDie();
	virtual bool doDamage(MUID uidAttacker, DAMAGE_ATTRIB nDamageAttrib, uint32 nDamage, EFFECT_SOURCE_TYPE nEffectSourceType = EST_NONE, int nEffectSourceID = INVALID_ID);	// return : 피해를 입고 죽었는가
	void doHeal(MUID uidAttacker, uint32 nHealAmount);
	virtual void doAttack(MUID uidTarget, int nDamage);
	virtual void doTryDamage(MUID& uidAttacker)	{}
	virtual void doTryAttack(MUID& uidTarget)		{}
	virtual bool doUseTalent(int nTalentID, TALENT_TARGET_INFO Target = TALENT_TARGET_INFO::Invalid(), bool bCheckEnabled=true, bool bGainStress=true);
	virtual bool doUseTalent(GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target = TALENT_TARGET_INFO::Invalid(), bool bCheckEnabled=true, bool bGainStress=true);
	// 탤런트를 취소 (탤런트정보의 cancelable속성이 맞을 경우에만 해당)
	virtual void doCancelTalent(bool bPostCommand=true);
	// 탤런트를 강제로 취소
	void doCancelTalentForce(bool bPostCommand=true);

	void doRebirthBySelf();
	void doRebirthByOther();

	virtual void doTryHit(GEntityActor* pAttacker, GTalentInfo* pTalentInfo, int nDamage, uint16 nCheckTime, int nCapsuleGroup, int nCapsuleIndex);
	
	// 면역관리자 반환
	GImmuneHandler& GetImmuneHandler()		{ return m_ImmuneHandler; }
	// 면역 처리, 반환값은 면역처리 됐는지 여부
	bool HandleImmune( GEntityActor* pAttacker, GTalentInfo* pTalentInfo );
	bool HandleImmune( GEntityActor* pAttacker, GBuffInfo* pBuffInfo );

	void OnImmuned( GTalentInfo* pTalentInfo, GEntityActor* pAttacker );
	void OnAvoid( GTalentInfo* pTalentInfo, GEntityActor* pAttacker );

	void		 SetKiller(MUID uidKiller)			{ m_uidKiller = uidKiller; }
	MUID		 GetKiller() const					{ return m_uidKiller; }

	// 무적상태인지 여부
	virtual  bool IsNowInvincibility();
	// 투명화상태인지 여부
	virtual bool IsNowInvisibility() const;
	// NPC에게 투명상태인지 여부
	virtual  bool IsNowInvisibilityToNPC();
	// 회피상태인지 여부
	bool IsNowAvoidTime();

	// 피해정보를 탤런트에 근거하여 계산후 반환, 탤런트정보가 없다면 액터단위로 계산
	virtual DAMAGE_ATTRIB GetDamageType(const GTalentInfo* pTalentInfo=NULL);

	// 탤런트를 사용 불가능한 상태인지 여부
	bool IsDisableTalent(GTalentInfo* pTalentInfo);

	void GetBuffList(vector<int>& vBuffList) const;
	void GetBuffList(int* Buffs);
	bool CheckTalentRange(GEntityActor* pTargetActor, int nTalentID);

	virtual float GetStandupTime() { assert(false); return -1.0f; }

	// 피격 이벤트
	virtual void OnHit(GHitInfo infoHit, bool bRouteHit);
	// 모든 히트캡슐 피격
	virtual void OnHitAll(GTalentInfo* pTalentInfo);
	// 모든 히트캡슐 피격
	virtual void OnHitSuccess(GTalentInfo* pTalentInfo);
	// 적을 공격했을때 이벤트
	virtual void OnHitEnemy(uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pTalentInfo);
	// 치명타 피격 이벤트
	virtual void OnCriHit( GEntityActor* pAttacker, GTalentInfo* pTalentInfo );
	// 적을 치명타로 공격했을때 이벤트
	virtual void OnCriHitEnemy( GEntityActor* pTarget, GTalentInfo* pTalentInfo );
	// 빗맞음 이벤트
	virtual void OnMiss(uint32 nCombatResultFalg, GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo);
	// 적의 공격을 회피했을때 이벤트
	virtual void OnMissEnemy(uint32 nCombatResultFalg, GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo);
	// 방어 이벤트
	virtual void OnGuard( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo );
	// 완전 방어 이벤트
	virtual void OnPerfectGuard(GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo);
	// 절대 방어 이벤트
	virtual void OnAbsoluteGuard( GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo );
	// 적의 공격을 막았을때 이벤트
	virtual void OnGuardEnemy(GEntityActor* pTarget, GTalentInfo* pAttackTalentInfo);
	// 탤런트 사용이 완료됐을때 호출되는 이벤트
	virtual void OnFinishTalent(GTalentInfo* pTalentInfo);
	// 탤런트 사용이 효과가 발동될때 호출되는 이벤트
	virtual void OnActTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo ) override;


	// 되받아치기 정보를 추가, Rate는 되받아칠 보정치, 같은 종류의 되받아치기는 중첩됨
	void	AddRiposte(CSRiposte infoRiposte);
	// 되받아치기 정보를 제거, Rate는 되받아칠 보정치
	void	EraseRiposte(CSRiposte infoRiposte);

	virtual bool IsEquipShield() const			{ return false; }


	GMFApplyReturnValue ApplyMotionFactor(const GMFApplyArgs args);
	virtual void DoMotionFactor(MF_STATE nMF, int nWeight, GEntityActor* pOpponent, bool bKnockbackDirSyncToOpponentDir);
	void ReleaseMotionFactor(bool bStateToNormal=true);

	void ChangeActionState(ACTION_STATE nState);
	virtual bool IsNowRunningCancelableTalent()		{ return true; }
	virtual void OnUseTalentFailed(int nTalentID, CCommandResultTable nFailCause) {}
	
	void SetHP(int nHP);
	void SetEN(int nEN);
	void SetSTA(int nSTA);

	// 센서 관련
	bool			IsValidSensor(const unsigned long nSensorID, vec3 vEventPos);
	void			EnterSensor(uint32 nSensorID);
	void			LeaveSensor(uint32 nSensorID);
	void			ClearEnteredSensorID(void);
	uint32			GetForcusedSensorID();
	void			SensorUseTalent(const int nTalentID);
	bool			IsEnteredSensorID(int nSensorID);
	void			AddEnteredSensorID(int nSensorID);
	void			RemoveEnteredSensorID(int nSensorID);
	
	vec3 GetScheduleMovePos() const;
	void SetScheduleMove(vec3 pos);
	virtual bool DoScheduleMove();
	virtual bool doGuard(int nGuardTalentID);
	virtual bool doGuardReleased();

	void doJump()	{ m_isJumping = true;	}
	void doLand()	{ m_isJumping = false;	}
	void doMove()	{ m_isMoving = true;	}
	void doStop()	{ m_isMoving = false;	}

	int	 GetGuardTalentID();
	bool IsGuarding()		{ return m_GuardInfo.bGuarding; }	// 방어 중인지 여부
	bool IsMoving()			{ return m_isMoving;			}	// 이동 중인지 여부
	bool IsJumping()		{ return m_isJumping;			}	// 점프 중인지 여부
	bool IsKnockdowned()	{ return GetCurrentMotionFactor() == MF_KNOCKDOWN; }// 넉다운인지 여부
	bool IsTalentCasting();
	virtual bool IsHittable(GEntityActor* pAttacker, GTalentInfo* pAttackTalentInfo);		///< 판정 가능한지 여부
	virtual bool IsHittable(GBuffInfo* pBuffInfo );
	bool IsGuardableDirection(GEntityActor* pAttacker);
	// 이동할 수 있는 여부 (능동적인 이동)
	virtual bool IsMovable();
	// 움직여질 수 있는지 여부 (모든 이동)
	bool IsSticky();
	
	bool IsAllowCalcVictimDodge();		// 방어자 회피율 적용여부 확인	

	void RouteActionCancelNextAttack(UIID nEntityUIID, CANCEL_NEXT_ATTACK_PARAM nParam);


	// 모션팩터 관련
	MF_STATE	 GetCurrMF()				{ return m_MotionFactor.GetCurrMF(); }
	int			 GetCurrWeight()			{ return m_MotionFactor.GetCurrWeight(); }
	GMotionFactor& GetMotionfactorForTest() { return m_MotionFactor; }
			bool SetMFModValue( int nType, float fValue );
			bool SetMFModWeight( int nType, float fWeight );
	virtual void OnBeginMotionFactor()		{}
	virtual void OnEndMotionFactor()		{}


	// 메세지 관련
	virtual void RouteToThisCell(MCommand* pCommand, MUID uidExcept=0) const override;
	virtual void RouteToMe(MCommand* pCommand) const;


	void RouteRebirth();
	void RouteTalentDrain(const GEntityActor* pAttacker, int nTalentID, TALENT_DRAIN_TYPE nDrainType, int nDrainAmount);
	void RouteSimpleStatus(MUID uidListener);
	virtual void RouteFaceTo( vec3 vDir ) {}
	void RouteCancelTalent( int nTalentID );	// 클라 쿨타임 동기용으로 쓰임
	void RouteUseTalentRequestFailed( int nTalentID );
	void RouteImmune( GEntityActor* pAttacker, GTalentInfo* pTalentInfo );
	void RouteAvoid( GEntityActor* pAttacker, GTalentInfo* pTalentInfo );
	void RouteMiss( GEntityActor* pAttacker, GTalentInfo* pTalentInfo );

	virtual MCommand* MakeCmd_Whois(GEntitySync* pReqSync) { return NULL;}

	virtual CHR_INFO*	GetChrInfo() const					{ return m_pChrInfo; }
			void		SetChrInfo(CHR_INFO* infoChar)		{ m_pChrInfo = infoChar; }
			CHR_STATUS*	GetChrStatus()						{ return &m_ChrStatus; }


	virtual void doMakePeace();	
	virtual bool IsNowCombat() const { return false; };	///< 지금 전투중인가?

	// 특정 버프가 걸렸는지 여부
	bool HasBuff(int nBuffID);
	// 특정 이로운 버프가 걸렸는지 여부
	int GetBuffQty();
	// 특정 디버프가 걸렸는지 여부
	int GetDebuffQty();
	// 특정 버프를 제거하기
	void LostBuff(int nBuffID);
	// 특정 버프 슬롯 전부를 제거
	void LostBuffStack(int nBuffIDSlot);

	// 특정 포커스가 걸려있는지 여부
	virtual bool HasFocus(TALENT_FOCUS_TYPE nFocus)			{ return false; }
	// 포커스가 하나라도 걸려있는지 여부
	virtual bool HasFocus()									{ return false; }

	// Attribute Modifier
	virtual int GetMaxHPProto() const { return m_pChrInfo->nMaxHP; }
	virtual int GetMaxENProto() const { return m_pChrInfo->nMaxEN; }
	virtual int GetMaxSTAProto() const { return m_pChrInfo->nMaxSTA; }

	void TrimStats();
	int GetMaxHP() const;
	int GetMaxEN() const;
	int GetMaxSTA() const;
	virtual int GetAP() const { return m_ChrStatus.ActorModifier.nAP.Calc(m_pChrInfo->nAP); }
	int GetABS() const { return m_ChrStatus.ActorModifier.nABS.Calc(m_pChrInfo->nABS); }
	virtual float GetMoveSpeed() { return GetWalkSpeed(); }
			float GetWalkSpeed() const { return DefaultWalkSpeed() * (1 + m_ChrStatus.ActorModifier.fMoveSpeed); }
	virtual float DefaultWalkSpeed() const;
	int GetHPRegen() const { return m_ChrStatus.ActorModifier.nHPRegen.Calc(m_pChrInfo->nHPRegen); }
	uint8 GetHPPercent() { if (GetMaxHP() <= 0) return 0;	return (GetHP() * 100) / GetMaxHP(); }
	int GetENRegen() const { return m_ChrStatus.ActorModifier.nENRegen.Calc(m_pChrInfo->nENRegen); }
	int GetSTARegen() const { return m_ChrStatus.ActorModifier.nSTARegen.Calc(m_pChrInfo->nSTARegen); }
	int GetResist(TALENT_RESIST_TYPE nResistType) const;

	void RegUIID(UIID nUIID)			{ m_nUIID = nUIID; }
	const UIID	GetUIID(void) const	{ return m_nUIID; }


	int GetHP()		{ return m_ChrStatus.nHP; }
	int GetEN()		{ return m_ChrStatus.nEN; }
	int GetSTA()	{ return m_ChrStatus.nSTA; }
	virtual int GetCHA() const	{ return 0; }
	
	virtual const wchar_t* GetName() const { return L"actor"; }
	
	ACTION_STATE	GetActionState() const 			{ return m_nActionState; }
	void			SetCurrentMotionFactor(MF_STATE nState, int nWeight) { m_MotionFactor.Force(nState, nWeight); }
	MF_STATE		GetCurrentMotionFactor() const	{ return m_MotionFactor.GetCurrMF(); }
	bool			HasMotionFactor() const;
	float			GetColRadius();
	float			GetHitRadius();
	virtual float	GetValidInteractionDistance();		///< 인터랙션 가능 거리
	virtual GActorMeshInfo*	GetMeshInfo()				{ return NULL; }
	virtual int GetLevel() const					{ return m_pChrInfo->nLevel; }
	virtual void SetLevel(int nLevel)				{ m_pChrInfo->nLevel = nLevel; }

	virtual void ForceStandUp();	// 강제로 일으켜세운다.
	bool SetPosToGroundVertical(vec3* pvTarPos=NULL);	// 위치를 지면에 맞게 z좌표를 조정해준다.(pvTarPos가 NULL이면 현재 위치를 지면에 맞춤.)

	virtual int GetStepTalentID(int nTalentGroupID, int nStepIndex) { return 0; }

	void Warp(vec3 vPos);
	virtual void Warp(vec3 vecPos, vec3 vecDir, bool bRouteToMe)	{ }

	bool RemoveBuff(int nBuffID);
	virtual bool GainBuff( int nBuffID, GTalentInfo* pTalentInfo=NULL, GEntityActor* pUser=NULL );
	bool GainBuffDetail( int nBuffID, float fDuration, float fPeriod, GTalentInfo* pTalentInfo=NULL, GEntityActor* pUser=NULL );
	void SetRegenActive(bool bActive) { return m_RegenUpdater.SetActive(bActive); }
	bool IsRegenActive() { return m_RegenUpdater.IsActive(); }
	void SetMFRegenActive(bool bActive) { return m_MotionFactor.SetActive(bActive); }
	bool IsMFRegenActive() { return m_MotionFactor.IsActive(); }
	virtual int8 GetGrade() const { return PLAYER_GRADE; }
	virtual ARMOR_ATTRIB GetArmorAttrib() const 	{ return ARMOR_ATT_NEUTRAL; }
	virtual DAMAGE_ATTRIB GetEnchantAttrib() const;
	GMotionFactor& GetMotionFactorForTest() { return m_MotionFactor; }
	GEntityActor* FindActor(const MUID& uidActor) const;	

	MUID GetLastHitOrHittedEnemy() { return m_uidLastHitOrHittedEnemy; }
	GTalent* GetCurrentTalent();
	
	virtual void Narration(const wchar_t* szText)		{ return; }
	virtual void NarrationNow(const wchar_t* szText)	{ return;	}

	GActorInvisibility& 	GetActorInvisibility()		{ _ASSERT(m_pInvisibility); return (*m_pInvisibility);  }
	GActorCounter& 			GetActorCounter()			{ _ASSERT(m_pCounter); return (*m_pCounter);  }
	GActorCooltimeChecker&	GetActorCooltimeChecker()	{ _ASSERT(m_pCoolTimeChecker); return *m_pCoolTimeChecker; }
	GActorHateTracker&		GetActorHateTracker()		{ _ASSERT(m_pHateTracker); return *m_pHateTracker; }
	GActorBuffEntity&		GetActorBuffEntity()		{ _ASSERT(m_pBuffEntity); return *m_pBuffEntity; }
	GActorTalent&			GetActorTalent() const		{ _ASSERT(m_pActorTalent); return *m_pActorTalent; }

	void DisableRegen()		{ m_bRegen = false; }
	void EnableRegen()		{ m_bRegen = true; }
	bool IsDisableRegen()	{ return !m_bRegen; }


	bool HasBuff_ForTest( int nBuffID );
	

};

GEntityActor* ToEntityActor( GEntity* pEntity );


#endif