#ifndef _XMODULE_NET_CONTROL_H
#define _XMODULE_NET_CONTROL_H

#include "XModuleMotion.h"
#include "CSMotionFactorInfo.h"
#include "XModuleActorControl.h"
#include "XNetActionState.h"

class XNetPlayer;
class XTalentInfo;
class XNetActionFSM;
class XNetActionGrappled;
class XNetActionSwallowed;
class XNetActionSit;
class XNetActionGuard;
class XNetActionGesture;
class XNetActionHit;
class XNetActionTalent;

enum CHANGE_ANI_STATE
{
	CHANGEANI_STOP = 0,
	CHANGEANI_MOVE
};

// 서버로부터 오는 메세지 처리
class XModuleNetControl : public XModuleActorControl, public MMemPool<XModuleNetControl>
{
	friend XNetPlayer;
	friend XNetActionGrappled;
	friend XNetActionSwallowed;
	friend XNetActionSit;
	friend XNetActionGuard;
	friend XNetActionGesture;
	friend XNetActionHit;
	friend XNetActionTalent;

protected:
	//////////////////////////////////////////////////////////////////////////
	// FSM
	//
	XNetActionFSM*			m_pActionFSM;
	void					InitStates();
	void					DoAction(NET_ACTION_STATE nState, void* pParam=NULL);			///< 액션 변경


	//////////////////////////////////////////////////////////////////////////
	// Movement관련
	//
	int							m_nMovementFlag;

	//////////////////////////////////////////////////////////////////////////
	// 기타
	//
	bool						m_bSwitchingWeapon;
	int8						m_nSwitchingWeaponSet;
	bool						m_bLooting;

	float						m_fCurrAppearTime;		// 나타나는 시간
	list<XObject*>				m_pAimingObj;			// Aiming Object - Target

	int							m_nTempSwallowedSynch;

	MF_STATE					m_nMFState;


	//////////////////////////////////////////////////////////////////////////
	// 모듈 초기화
	//
	virtual void				OnInitialized();


	//////////////////////////////////////////////////////////////////////////
	// OnEvent 콜백
	//
	virtual XEventResult		OnEvent(XEvent& msg);
	virtual void				OnSubscribeEvent();
	
	bool						StartGrappled(_DamageRepeatInfo* pInfo);
	bool						StartSwallowed(_DamageRepeatInfo* pInfo);
	void						DoSwitchingWeapon();

	virtual void				HitByTalent(XTalentHitParam * pInfo);
	


	//////////////////////////////////////////////////////////////////////////
	// OnUpdate : if pass CheckUpdatable,
	// OnUpdateState -> OnUpdateCommand -> OnUpdateSpecail
	//
	virtual void				OnUpdate(float fDelta);				///< 매 틱마다 호출

	virtual bool				OnUpdateCheckUpdatable(){ return true; }
	virtual void				OnUpdateState(float fDelta);
	virtual void				OnUpdateCommand(float fDelta);
	virtual void				OnUpdateSpecial(float fDelta);

	virtual void				OnPostUpdate(float fDelta);

	// OnUpdateSpecail, 목표 겨누기 관련
	void 						_UpdateAimingShoot(float fDelta);

	//////////////////////////////////////////////////////////////////////////
	// 기타
	//
	enum E_DIR_TYPE
	{
		E_DIR_NORMAL = 0,
		E_DIR_FORWARD, 
		E_DIR_BACKWARD,
		E_DIR_SIDE
	};
	E_DIR_TYPE					CalcMovementDirFromFlag(const vec3& vDir, int nFlag, vec3& _rOutPut);	// 네트워크에서 넘어온 플래그를 통해 Move의 방향과 타입 구하기
	bool						IsCanChangeAnimation(CHANGE_ANI_STATE nState, unsigned int nFlage = MT_DEFAULT); // 애니메이션 변경이 가능한 상태인가?


public:

	//////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸 / Owner
	//
	XModuleNetControl(XObject* pOwner=NULL);
	virtual ~XModuleNetControl(void);

	XNetPlayer*					GetOwner();


	//////////////////////////////////////////////////////////////////////////
	// 네트웍에서 넘어온 커맨드로 동작
	//
	virtual void 				OnUseTalent(int nTalentID, vec3& vPos, vec3& vDir);
	virtual void 				OnActTalent(int nTalentID, vec3& vDir);
	virtual void 				OnCancelTalent(); // Stop() 사용

	virtual void				OnActSpellProjectile(int nTalentID, vec3& vEntityDir, vec3& vTargetPos, MUID& uidTarget, int nGroupID, int nCapsuleID);

	virtual void				OnActArchery(int nTalentID, vec3& vDir, MUID& uidTarget);
	virtual void				OnActArcheryDetail(int nTalentID, vec3& vDir, MUID& uidTarget, int nGroupID, int nCapsuleID);

	virtual void 				OnHit(XTalentHitParam* pHitInfo); // Stop() 사용
	void 						OnAttackedGun(vec3& vPos, vec3& vDir, MUID& uidTarget, bool& bHitted);

	virtual void				OnRebirth();										// ActionRebirth()
	virtual void				OnDie(MF_STATE nMFState = MF_NONE, bool bSetAni = true);
			void				OnReInPlayer(TD_UPDATE_CACHE_PLAYER* pPlayerInfo);

	virtual void				OnSwitchingWeaponBegin();
	virtual void				OnSwitchingWeapon(int8 nWeaponSet);

	virtual void				OnHitLooting();

	virtual void				DoActionLootingItem(MUID& uidNPC, float fLootTime=0.f);
	virtual void				OnLootEnd();

	virtual	void				OnGuard(vec3& pos, vec3& dir);						// Stop(), Guard()
	virtual void				OnGuardReleased();									// ActionGuardRelease()
	virtual void				OnGuardKnockBack(vec3& vDir);
	virtual void				OnGuarded();

	virtual void				DoActionSitDown();
	virtual void				DoActionSitRise();

	virtual void				DoActionGesture(const wchar_t* szMotionName);;

	void 						OnChangeStance(CHAR_STANCE nStance);				// Stop()
	void 						OnMove(TD_PC_MOVE* pMI);							// StopMove(), UpdateNetMove()
	void						OnMoveStop(vec3* pPos);
	void						OnStop(vec3* pPos, vec3* pDir);						// Stop()
	void 						OnRotate(vec3& dir);
	void 						OnJump(vec3& vPos, vec3& vDir);						// Jump()
	void						OnFalling(vec3& pos, vec3& dir);					// ActionFalling()

	virtual void				DoActionIdle();
	virtual void				DoActionQuestInteraction();
	virtual void				DoActionDeactivate( const int nBuffID, const wchar_t* szUseAni);

	virtual bool				IsCurrentGuard();

	void						WaitSwallowedSynch(int nMFWeight);
	virtual void				OnSwallowedSynch(int nTalentID, MUID& uidAttacker);

	inline NET_ACTION_STATE		GetCurrentActionStateID();
	XNetActionState*			GetCurrentActionState();

	virtual void				HitByBuff(XTalentHitParam * pHitInfo);

	virtual void				SetAFK(bool bAFK);
};

//----------------------------------------------------------------------------------------------------
inline NET_ACTION_STATE XModuleNetControl::GetCurrentActionStateID()
{
	if(m_pActionFSM == NULL)
		return NET_ACTION_STATE_NONE;
	else
		return (NET_ACTION_STATE)m_pActionFSM->GetCurrentStateID();
}

inline XNetActionState* XModuleNetControl::GetCurrentActionState()
{
	if(m_pActionFSM == NULL)
		return NULL;
	else
		return m_pActionFSM->GetCurrentState();
}

#endif // _XMODULE_POST_H