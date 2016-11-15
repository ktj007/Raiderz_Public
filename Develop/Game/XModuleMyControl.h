#ifndef _XMODULE_MY_CONTROL_H
#define _XMODULE_MY_CONTROL_H

#include "XVirtualKeyDef.h"
#include "XMyActionState.h"
#include "CSMotionFactorInfo.h"
#include "XModuleActorControl.h"

#include "XMyHitController.h"
#include "XPvPAreaChecker.h"
#include "XMyCharacterAlphaMgr.h"
#include "XMapSignAreaChecker.h"
#include "XRestrictedAreaChecker.h"

class XModuleEntity;
class XMyActionState;
class XMyPlayer;
class XSpecialActionTransChecker;
class MockController;
class XModulePost;
class XSensorController;
class XMouseMove;
class XTargetIndicator;
class XChallengerQuestExitChecker;

class XReserveAttack
{
public:
	int m_nTalentID;
	TALENT_NORMAL_ATTACK_TYPE	m_nNextAttackType;
	uint32 m_nReservedTime;
	XReserveAttack() : m_nNextAttackType(NA_NA) {}
	void Reserve(TALENT_NORMAL_ATTACK_TYPE	nNextAttackType)
	{
		m_nNextAttackType = nNextAttackType;
		m_nReservedTime = XGetNowTime();
	}
	void ReserveUseTalent(int nTalentID)
	{
		m_nTalentID = nTalentID;
		m_nNextAttackType = NA_USE_TALENT__CLIENT_ONLY;
		m_nReservedTime = XGetNowTime();
	}

	void Release()
	{
		m_nTalentID = 0;
		m_nNextAttackType = NA_NA;
	}
};



class _ChangeWeaponInfo
{
public:
	SH_ITEM_SLOT	m_nSlot;
	MUID			m_uidWeaponID;

	_ChangeWeaponInfo() : m_nSlot( ITEMSLOT_NONE), m_uidWeaponID( MUID::ZERO)		{}
	_ChangeWeaponInfo( SH_ITEM_SLOT	nSlot, MUID uidWeaponID) : m_nSlot( nSlot), m_uidWeaponID( uidWeaponID)		{}

	void Set( SH_ITEM_SLOT	nSlot, MUID uidWeaponID)
	{
		m_nSlot = nSlot;
		m_uidWeaponID = uidWeaponID;
	}

	bool IsEmpty()
	{
		return ( (m_nSlot != ITEMSLOT_RWEAPON  &&  m_nSlot != ITEMSLOT_RWEAPON2)  ||  m_uidWeaponID == MUID::ZERO);
	}

	void Clear()
	{
		m_nSlot = ITEMSLOT_NONE;
		m_uidWeaponID = MUID::ZERO;
	}
};




/// @brief 내 플레이어의 조종을 담당하는 모듈
/// @author 남기룡
/// @see
///  - XMyPlayer만 이 모듈을 가지고 있다.
class XModuleMyControl : public XModuleActorControl
{
private:

	friend class XMyActionState;
	friend class XMyActionIdle;
	friend class XMyActionNormalAttack;
	friend class XMyActionDodgeAttack;
	friend class XMyActionBow;
	friend class XMyActionShootGun;
	friend class XMyActionGuard;
	friend class XMyActionHit;
	friend class XMyActionUseItem;
	friend class XMyActionHold;
	friend class XMyActionSwitchingWeapon;
	friend class XMyActionChangeState;
	friend class XMyActionLootingItem;
	friend class XMyActionRebirth;
	friend class XMyActionUseTalent;
	friend class XMyActionJump;
	friend class XMyActionFalling;
	friend class XMyActionGrappled;
	friend class XMyActionChangeChannel;
	friend class XMyActionQuestInteraction;
	friend class XMyActionDeactivate;
	friend class XPlayFrame;
	friend class XPlayFrame_Debugger;
	friend class MockController;
	friend class XMyActionSwallowed;
	friend class XMyActionSit;
	friend class XMyActionDeactivative;
	friend class XMyActionGesture;
	friend class XMyActionNPCInteraction;
	friend class XMyActionEnchant;
	//SoulHunterZ
	friend class XMyActionAttune;

	XModulePost*			m_pModulePost;

	//////////////////////////////////////////////////////////////////////////
	// 이동관련
	//
	bool					m_bPaused;					// 이동/회전/애니메이션 사용 금지상태
	float					m_fSpeed;
	bool					m_bAutoRun;
	bool					m_bAutoRunKeyRelease;
	bool					m_bRestoreCamera;			// 카메라 원위치 중인지 여부
	unsigned int			m_nMovementFlags;
	vec3					m_vCharacterFrontMoveDir;	// 캐릭터 이동 방향

	bool					m_bFallingStart;
	bool					m_bCurrentFreeLook;

	// 마우스 이동
	vec3					m_vMouseAutoMove;			// 마우스 이동 최종 위치
	bool					m_bMouseAutoMove;


	//////////////////////////////////////////////////////////////////////////
	// 액션관련
	//
	XSpecialActionTransChecker*		m_pAttackChecker;
	unsigned int			m_nLastCancelActionTime;
	bool					m_bCancelAction;
	int						m_nCancelParam;					// 0 : 모두캔슬  1 : uppercombo만 나감
	void					_UpdateReleaseCancelAction();

	bool					m_bOnTargetObject;				//현재 타켓 오브젝트가 존재유무.

	_ChangeWeaponInfo		m_ChangeWeaponInfo;				// 전투 스탠스시 사용중인 아이템을 바꿀때 정보


	//////////////////////////////////////////////////////////////////////////
	// 트리거 체크 관련
	//
	XSensorController*		m_pSensorController;
	XRegulator				m_TouchSensorCheckRegulator;

	//////////////////////////////////////////////////////////////////////////
	// 맵표지판 지역 체크 관련
	//
	XMapSignAreaChecker		m_MapSignAreaChecker;
	XRestrictedAreaChecker  m_RestrictedAreaChecker;

	//////////////////////////////////////////////////////////////////////////
	// 마우스 이동 관련
	//
	XMouseMove*				m_pMouseMove;

	//////////////////////////////////////////////////////////////////////////
	// 기타
	//
	XMyHitController		m_HitController;
	XMyCharacterAlphaMgr	m_CharacterAlphaMgr;
	XTargetIndicator*		m_pTargetIndicator;

	//////////////////////////////////////////////////////////////////////////
	// 연타 데미지
	// 
	bool					m_bDamageRepeat;
	_DamageInfo *			m_pDamageInfo;

	//////////////////////////////////////////////////////////////////////////
	// Module 초기화
	//
	virtual void			OnInitialized();

	//////////////////////////////////////////////////////////////////////////
	// 라이트(임시 삭제)
	//MUID					m_uidCharacterLightEffect;

	//////////////////////////////////////////////////////////////////////////
	// OnEvent 콜백 : Action and Hit
	//
	virtual void			OnSubscribeEvent();
	virtual XEventResult	OnEvent(XEvent& msg);

	// Action
	XMyActionFSM*			m_pActionFSM;
	void					InitStates();

	bool					DoAction(ACTION_STATE nState, void* pParam=NULL);			///< 액션 변경
	bool					DoAction(ACTION_STATE nState, int nParam);

	bool					IsCanCancelAllMotion();
	bool					IsCanCancelUpper();	

	bool					IsCanStopMouseToggle();

	// Hit
	virtual void			HitByTalent(XTalentHitParam * pInfo);


	//////////////////////////////////////////////////////////////////////////
	// OnUpdate : if pass CheckUpdatable,
	// OnUpdateState -> OnUpdateCommand -> OnUpdateSpecail
	//
	virtual void			OnUpdate(float fDelta);
	virtual bool			OnUpdateCheckUpdatable();
	virtual void			OnUpdateState(float fDelta);
	virtual void			OnUpdateCommand(float fDelta);
	virtual void			OnUpdateSpecial(float fDelta);
	virtual void			OnPostUpdate(float fDelta);

	void					_UpdateMyCharacterAlpha();
	void					_UpdateRestrictedArea(float fDelta);

	// OnUpdateCommand, 이동관련
	void					_UpdateMyDir( float fDelta, const vec3& dir );
	void					_UpdateFrontDirAndSetCamera( float fDelta, vec3& dir);
	bool					_UpdateMyMovement(float fDelta, XModuleEntity* pModuleEntity, vec3& vDir);
	//void					_UpdateMovementPost(float fDelta, vec3& pos, vec3& dir, bool bKeyAction=false);
	//void					_UpdateRotationPost(float fDelta, XModuleEntity* pModuleEntity);

	// OnUpdateSpecail, 센서관련
	void					_UpdateTouchSensorTrigger(float fDelta);
	void					_UpdateTouchAreaTrigger();
	void					_UpdateTouchAreaTrigger( float fDelta );

	// 마우스 인터렉션 관련
	void					MouseMoveClick(MPoint& pt);
	void					SetMouseMove(vec3& vMovePos);
	void					InitMouseMove()		{ m_bMouseAutoMove = false; m_vMouseAutoMove = vec3::ZERO; }

	//////////////////////////////////////////////////////////////////////////
	// Current 체크함수
	//
	float					GetExtSpeedRatio();					// 현재 상황에 따른 이동 속도를 가져온다.
	XMotionTypes			GetCurrMotionType();				// 현재 모션(앞으로 달리기, 뒤로 달리기 등등)
	bool					CheckSpecialActionKey( SPECIAL_ACTION_COMMAND& eCmd );			// 현재 키 입력 시퀀스
	bool					CheckUsableSpecialActionKey(XEvent& msg);	
	bool					CheckActionEventAcceptable(XEvent* pEvent = NULL);		// 현재 이벤트 수신이 가능한지 여부
	void					CheckMouseMoveState();
	bool					CheckActionEventAcceptableByMouseVisible(XEvent* pEvent = NULL);
	bool					CheckAttackReturnAnimation();
	bool					CheckMouseMovable();
	bool					CheckGuardKnockback();

	//////////////////////////////////////////////////////////////////////////
	// 상태 업데이트
	void					UpdateStateFalling();

	//////////////////////////////////////////////////////////////////////////
	// 라이트(임시 삭제)
	//bool					CreateCharacterLightEffect();
	//void					_UpdateLightEffectPos(vec3 vCameraDir);

protected:
	virtual void			CheckSwim(bool bIn);				// 현재 수영여부

	void					DoActionDie();

	bool					StartGrappled(_DamageRepeatInfo* pInfo);
	bool					StartSwallowed(_DamageRepeatInfo* pInfo);

	XChallengerQuestExitChecker*	m_pChallengerQuestExitChecker;

public:

	//////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	XModuleMyControl(XObject* pOwner=NULL);
	virtual ~XModuleMyControl(void);


	//////////////////////////////////////////////////////////////////////////
	// 네트웍에서 넘어온 커맨드로 동작
	//
	virtual void 			OnHit(XTalentHitParam* pHitInfo);
	virtual void 			OnDie(MF_STATE nMFState = MF_NONE, bool bSetAni = true);
	virtual void			OnRebirth();
	virtual void			OnSwitchingWeaponBegin();
	virtual void			OnSwitchingWeapon(int8 nWeaponSet);
	virtual void			OnLootEndReady();
	virtual void			OnHitLooting();
	virtual void			OnHeal(int nAmount, UIID nUserUIID = UIID_INVALID);

	virtual void			OnWarp(const vec3& vPos, const vec3& vDir);

	virtual void			DoActionLootingItem(MUID& uidNPC, float fLootTime=0.f);
	virtual void			OnLootEnd();
	virtual void			OnLootItemStart();

	virtual void			OnGuardKnockBack(vec3& vDir);
	virtual void			OnGuarded();
	virtual void			OnGuardFailed();

	virtual void			DoActionSitDown();
	virtual void			DoActionSitRise();

	virtual void			OnHitByBuff(TD_BUFF_HIT* pBuffHitInfo);

	bool					DoActionDye(void* pParam)		{ return DoAction(ACTION_STATE_DYE, pParam);	}
	void					DoActionHold()					{ DoAction(ACTION_STATE_HOLD);	}
	void					DoActionUseItem(int nSlotID);

	void					DoActionUseTalent(int nTalentID);
	void					DoActionEquipItem( SH_ITEM_SLOT nSlot, MUID uidItemID);
	void					DoActionChangeChannel();
	virtual void			DoActionIdle();
	void					DoActionIdleWithSpecialState();
	virtual void			DoActionQuestInteraction();
	virtual void			DoActionDeactivate( const int nBuffID, const wchar_t* szUseAni);
	virtual void			DoActionGesture(const wchar_t* szMotionName);
	void					DoActionInteraction(MUID uidTarget);
	bool					DoActionEnchantUseItem(int nSlotID);
	void					DoActionMapObjInteraction(MUID uidSearch);

	void					ReleaseCancelAction();
	void					SetCancelAction(int nParam);

	XMyPlayer*				GetMyPlayerOwner();
	inline ACTION_STATE		GetCurrentActionStateID();
	XMyActionState*			GetCurrentActionState();

	bool					IsMoving();
	bool					IsJumping();		// tripleJ 110420

	void					SetAutoRun(bool bAutoRun)	{ m_bAutoRun = bAutoRun; }
	bool					IsAutoRun()					{ return m_bAutoRun;	}

	void					Init_ForChangeField();
	void					Final_ForChangeField();

	void					InitStand();
	void					InitAutoMove();
	void					InitSensorController(XGameState* pState);

	void					OnRebirthSoulBinding();
	void					Rebirth();

	void					StopRun();

	void					SetPause( bool bPause);
	bool					IsPaused()					{ return m_bPaused;		}

	bool					ChangeWeapon();

	virtual void			DoInnBeginSleep();
	virtual void			DoInnEndSleep();

	void					DoGrappledEnd();

	XReserveAttack			m_ReserveAttack;

	bool					IsCurrentGuardEnabled();

	virtual void			OnTalentInstantEffectGain(XTalentInfo* pTalentInfo);
	void					FallingDamage();

	virtual bool			IsCurrentGuard();
	virtual bool			IsCurrentGrappled();

	XMyHitController*		GetMyHitController() { return &m_HitController; }

	bool					IsPostPosSync();
	bool					IsSendablePositionPost();

	void					InitFallingStartZ();
	void					UseRefillPotion();

	void					SyncPlayerDirToCameraDir();

	bool					IsMouseMove()		{ return m_bMouseAutoMove; }
	bool					IsHelpMessage();

	virtual void 			OnUseTalent(int nTalentID, vec3& vPos, vec3& vDir);

	// Target NPC Info
	void					ChangeIndicatorEffectPos();
	void					SetTargetIndicatorToggle(bool bShow);

	void					RequestCancel_PreInteraction();

	void					StartChallengerQuestExitTimer();
	void					StopChallengerQuestExitTimer();

	virtual void			HitByBuff(XTalentHitParam * pHitInfo);

	void					DoCameraLock(MUID& uidTarget, wstring& strBoneName, float& fDurationTime);
	void					DoForceCameraLock();

	virtual void			SetAFK(bool bAFK);

	// LockOnTarget 풀기	_by tripleJ 110520
	void					ClearLockOnTarget( const UIID& uidObj );
};

//----------------------------------------------------------------------------------------------------
inline ACTION_STATE XModuleMyControl::GetCurrentActionStateID()
{
	if(m_pActionFSM == NULL)
		return ACTION_STATE_NONE;
	else
		return (ACTION_STATE)m_pActionFSM->GetCurrentStateID();
}

inline XMyActionState* XModuleMyControl::GetCurrentActionState()
{
	if(m_pActionFSM == NULL)
		return NULL;
	else
		return m_pActionFSM->GetCurrentState();
}
//----------------------------------------------------------------------------------------------------

#endif // #ifndef _XMODULE_MY_CONTROL_H