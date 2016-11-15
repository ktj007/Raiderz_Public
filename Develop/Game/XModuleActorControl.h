#ifndef _XMODULE_ACTOR_CONTROL_H
#define _XMODULE_ACTOR_CONTROL_H

#include "XModule.h"
#include "CTransData.h"
#include "XDeathController.h"
#include "XDuelFightChecker.h"
#include "XDuelGestureChecker.h"
#include <deque>

#include "XBPartWeaponController.h"

class XPlayer;
class XTalentInfo;
class XControlCmd;
class XMovableGoParam;

enum XMotionTypes;

#define BREAKABLEPARTS_NODE_NAME		"b_parts_01"
#define DISAPPEAR_TIME					3.0f
#define APPEAR_TIME						3.0f

//////////////////////////////////////////////////////////////////////////
enum DAMAGE_FLAG
{
	DF_HITBYTALENT = 0,
	DF_BUFF,
	DF_GUARD_PARTIAL
};

enum SWIM_STATE
{
	SS_NONE,
	SS_NOT_SWIM,
	SS_DOING_SWIM
};

struct _DamageInfo
{
	DAMAGE_FLAG				nDamageFlag;
	UIID					nAttackerUIID;
	int						nAttackedTalentID;
	int						nDamage;
	unsigned int			nFlags;

	MF_STATE				nMFState;

	wstring					strName;

	bool					bHitDodge;			// 빗맞음 여부
	bool					bSecondaryEffect;

	_DamageInfo() : nAttackerUIID(UIID_INVALID)
	{
		nAttackedTalentID	= 0;
		nDamage				= 0;
		nFlags				= 0;

		nDamageFlag			= DF_HITBYTALENT;
		nMFState			= MF_NONE;

		bHitDodge			= false;
		bSecondaryEffect	= false;

		strName.clear();
	}

	bool IsFlagMiss()			{ return (CheckBitSet(nFlags, CTR_MISS)) ? true : false;	}
	bool IsFlagRiposte()		{ return (CheckBitSet(nFlags, CTR_RIPOSTE)) ? true : false;	}
	bool IsFlagResistPerfect()	{ return (CheckBitSet(nFlags, CTR_RESIST_PERFECT)) ? true : false;	}
	bool IsFlagResistPartial()	{ return (CheckBitSet(nFlags, CTR_RESIST_PARTIAL)) ? true : false;	}
	bool IsFlagCritical()		{ return (CheckBitSet(nFlags, CTR_CRITICAL)) ? true : false;	}
	bool IsFlagDrain()			{ return (CheckBitSet(nFlags, CTR_DRAIN)) ? true : false;	}
	bool IsFlagZeroDamage()		{ return (CheckBitSet(nFlags, CTR_ZERO_TALENT_DAMAGE)) ? true : false;	}
	bool IsFlagImmune()			{ return (CheckBitSet(nFlags, CTR_IMMUNE)) ? true : false;	}
	bool IsFlagAvoid()			{ return (CheckBitSet(nFlags, CTR_AVOID)) ? true : false;	}
	bool IsFlagFalling()		{ return (CheckBitSet(nFlags, CTR_FALLING)) ? true : false;	}
	bool IsFlagDead()			{ return (CheckBitSet(nFlags, CTR_DEAD)) ? true : false;	}
	bool IsFlagBPart1()			{ return (CheckBitSet(nFlags, CTR_HIT_BPART1)) ? true : false;	}
	bool IsFlagBPart2()			{ return (CheckBitSet(nFlags, CTR_HIT_BPART2)) ? true : false;	}
	bool IsFlagBPart3()			{ return (CheckBitSet(nFlags, CTR_HIT_BPART3)) ? true : false;	}
	bool IsFlagBPart4()			{ return (CheckBitSet(nFlags, CTR_HIT_BPART4)) ? true : false;	}
	bool IsHitDodge()			{ return bHitDodge; }
};

class XTalentDelayParam;
class XTalentHitParam;
class XTalentGuardDefenseParam;
class _DamageRepeatInfo;

//////////////////////////////////////////////////////////////////////////

class XModuleActorControl : public XModule
{
private:
	DECLARE_ID(XMID_PLAYER_CONTROL);

protected:
	XModuleEntity*				m_pModuleEntity;
	XModuleAction*				m_pModuleAction;
	XModuleTalent*				m_pModuleTalent;
	XModuleMovable*				m_pModuleMovable;
	XModuleBuff*				m_pModuleBuff;

	XDeathController			m_DeathController;
	XDuelFightChecker			m_DuelFightChecker;
	XDuelGestureChecker			m_DuelGestureChecker;

	bool						m_bDoneMagicHitEffect;
	SWIM_STATE					m_eReservedSwimState;

	// 피격 예약
	bool						m_bReservedHitByTalent;
	int							m_nReservedHitByTalentCount;
	int							m_nApplicationDamageHpByTalentCount;

	_DamageInfo*				m_pDamageInfo;

	//////////////////////////////////////////////////////////////////////////
	// 브레이커블파츠 무기화
	//
	XBPartWeaponController*		m_pOverlappedEquipmentController;

	//////////////////////////////////////////////////////////////////////////
	virtual void				OnInitialized();
	virtual void				OnSubscribeEvent();
	virtual XEventResult		OnEvent(XEvent& msg);


	//////////////////////////////////////////////////////////////////////////
	// OnUpdate : if pass CheckUpdatable,
	// OnUpdateState -> OnUpdateCommand -> OnUpdateSpecail
	//
	virtual void				OnUpdate(float fDelta);
	virtual void				OnPostUpdate(float fDelta)		{}

	virtual bool				OnUpdateCheckUpdatable()		{ return true; }
	virtual void				OnUpdateState(float fDelta);
	virtual void				OnUpdateCommand(float fDelta)	{}
	virtual void				OnUpdateSpecial(float fDelta)	{}

	virtual void				OnHeal(int nAmount, UIID nUserUIID = UIID_INVALID);


	// 데미지 연속 효과 초기화
	_DamageRepeatInfo *			InitDamageRepeat(XTalentHitParam * pInfo);

	// 수영
	virtual void				CheckSwim(bool bIn, bool bFallingStart = false);				// 현재 수영여부


	void BirdTest_ShowDelay(XTalentDelayParam * pHitInfo);

	void						CheckReservedHitByTalent();

	// Hit
	void						UpdateHitDamage(int nDamage);
	void						FakeBeatenAndSetMultiplyColor( XTalentHitParam * pHitInfo, bool bFakeBeaten = false );

	void						OnReInField(vec3 vPos, vec3 vDir);

	void						UpdateDuelGestureChecker();

public:

	XModuleActorControl(XObject* pOwner = NULL);
	virtual ~XModuleActorControl(void);

	void						ReleaseOnMF();

	//----------------------------------------------------------------------------//
	// 서버와의 통신
	//----------------------------------------------------------------------------//

	virtual void				OnUseTalent(int nTalentID, vec3& vPos, vec3& vDir);
	virtual void				OnActTalent(int nTalentID, vec3& vDir);
	virtual void				OnActSpellProjectile(int nTalentID, vec3& vEntityDir, vec3& vTargetPos, MUID& uidTarget, int nGroupID, int nCapsuleID);
	virtual void				OnActArchery(int nTalentID, vec3& vDir, MUID& uidTarget);
	virtual void				OnActArcheryDetail(int nTalentID, vec3& vDir, MUID& uidTarget, int nGroupID, int nCapsuleID);
	virtual void				OnActSpellMagicArea(int nTalentID, vec3& vTargetPos);
	virtual void				OnActGoundTarget(int nTalentID, vec3& vEntityDir, vec3& vTargetPos);
	virtual void				OnCancelTalent();

	virtual void				OnReCastingTalent(int nTalentID, int nStep = 0);
	virtual void				OnExtraActTalent(XTalentInfo* pTalentInfo, const vec3& vPos, const vec3& vDir);

	virtual void				OnHit(XTalentHitParam* pHitInfo);
	virtual void				OnDie(MF_STATE nMFState = MF_NONE, bool bSetAni = true) {};
	virtual void				OnDespawn()												{};
	virtual void				OnRebirth()												{};
	virtual void				OnBreakParts(MUID& uidAttacker, int nPartsIndex);
	virtual void				OnBreakParts(int nPartsIndex, float fXYForce, float fZForce);
	virtual void				OnBreakPartEx(MUID& uidAttacker, int nPartsIndex, TD_USABLE_BPART* pUsableBpart, int nUsableBpartCount);
	virtual void				OnBreakPartsRecovery();

	virtual void				OnMove(TD_PC_MOVE* pMI)									{};

	virtual void				OnGuard(vec3& pos, vec3& dir)							{};
	virtual void				OnGuardReleased()										{};
	virtual void				OnGuardFailed();

	virtual void				OnSwitchingWeaponBegin()								{};
	virtual void				OnSwitchingWeapon(int8 nWeaponSet)						{};

	virtual void				DoActionLootingItem(MUID& uidNPC, float fLootTime=0.f)	{};

	virtual void				OnLootEnd()												{};
	virtual void				OnLootItemStart()										{};
	virtual void				OnSetLootable(bool bLootable)							{};
	virtual void				OnSetDeSpawn()											{};

	virtual void				OnBuffTalentGain(int nBuffID, float fLostRemained);
	virtual void				OnBuffTalentLost(int nBuffID);
	virtual void				OnBuffInstantEffectGain(int nBuffID);
	virtual void				OnBuffHit(int nBuffID);
	virtual void				OnBuffHit(TD_BUFF_HIT* pBuffHitInfo, bool bUseDamage);
	virtual void				OnHitByBuff(TD_BUFF_HIT* pBuffHitInfo);
	virtual void				OnBuffIncrease(int nBuffID, float fLostRemained);
	virtual void				OnBuffDecrease(int nBuffID);

	virtual void				OnWarp(const vec3& vPos, const vec3& vDir);
	void						GuardDefense(TD_TALENT_GUARD* pTalentGuardInfo);
	virtual void				OnGuardDefense(XTalentGuardDefenseParam* pTalentGuardInfo);

	virtual void				DoActionGesture(const wchar_t* szMotionName)	{};
	void						DoActionDuelGesture(const wchar_t* szMotionName);

	virtual void				OnBallonText(wstring szID, wstring szChat, float fMaintainTime);

	virtual void				OnTalentInstantEffectGain(XTalentInfo* pTalentInfo);

	void						OnFallingDamage(int nDamage);

	virtual void				OnSpew()												{};
	virtual void				OnSpewInterrupt()										{};
	virtual void				OnSwallowedSynch(int nTalentID, MUID& uidAttacker)		{};

	//----------------------------------------------------------------------------//
	// 클라와의 통신
	//----------------------------------------------------------------------------//

	//캐릭터 허리 돌리기;;;
	void						ActorRotation(vec3& CameraDir);
	void						SetActorSpineTarget(float fAngle1=0.f,float fAngle2=0.f);						
	void						RemoveSpineController();

	// XModuleAction
	void						Stop();
	void						StopMove(bool bForced=false);

	bool						IsCurAiming();
	vec3						GetAimingDir();

	// XModuleTalent
	bool						UseTalent(int nTalentID);

	//void						UpdateTargetList();

	void						StartAppear();
	void						StartDisappear();

	void						OnEnterGame(const vec3& vPos, const vec3& vDir, const XActor* pCallerOwnerActor);

	// 데미지, 피격 위치
	void						ShowHitAndDamage(_DamageInfo * pInfo, XObject * pAttacker, const vec3& vOffset =vec3::ZERO);
	_DamageInfo*				GetDamageInfo()	{ return m_pDamageInfo; }
	void						SetApplicationDamage();

	// 힐
	void						OnTalentHeal(UIID nUserUIID, int nTalentID, int nAmount);
	void						OnBuffHeal(UIID nUserUIID, int nBuffID, int nAmount);

	//--------------------------------------------------------------------------------
	virtual void				OnAttackedGun(vec3& vPos, vec3& vDir, MUID& uidTarget, bool& bHitted)	{};
	virtual void				OnAttackedArchery(MUID& uidPlayer, vec3& vArrowStartPos, vec3& vDir)	{};
	virtual void				OnHitLooting()															{};
	virtual void				OnGuardKnockBack(vec3& vDir);
	virtual void				OnGuarded();
	virtual void				OnDieAniEnd();

	virtual void				DoInnBeginSleep()	{};
	virtual void				DoInnEndSleep()	{};

	virtual void				DoActionQuestInteraction()	{};
	virtual void				DoActionIdle()				{};
	virtual void				DoActionDeactivate( const int nBuffID, const wchar_t* szUseAni) {};

	bool						IsReservedHitByTalent() { return m_bReservedHitByTalent; }

	bool						IsCurrentMotionGuardKnockback();

	virtual bool				IsCurrentGuard()			{ return false; }
	virtual bool				IsCurrentGrappled()			{ return false; }

	virtual void				DoActionSitDown()	{};
	virtual void				DoActionSitRise()	{};

	void						OnEffectLevelUP();
	void						OnEffectCreateGuild();
	void						OnEffectDyeDone();
	void						OnEffectFatigueBest();

	virtual void				HitByBuff(XTalentHitParam * pHitInfo) {};

	virtual void				DoActionMailOpen()	{};
	virtual void				DoActionMailClose()	{};
	virtual void				SetAFK(bool bAFK)	{};

	void						StartOverlappedEquipmentController(int nItemID);
	void						EndOverlappedEquipmentController(int nItemID);

	bool						IsOverlappedEquipmentControllerWork()					{ return m_pOverlappedEquipmentController->IsWork(); }
	bool						UsableActionState_OverlappedEquipmentItem(int nState, int nParam = 0)	{ return m_pOverlappedEquipmentController->UsableActionState(nState, nParam); }
	bool						BPartsAction(XVirtualKey nVirtualKey)					{ return m_pOverlappedEquipmentController->BPartsAction(nVirtualKey); }

	XModuleMovable*				GetModuleMovable()	{ return m_pModuleMovable; }
};

#endif // _XMODULE_ACTOR_CONTROL_H