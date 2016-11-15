#pragma once
#include "XModuleActorControl.h"
#include "XNPCActionState.h"

class XNPCActionFSM;
class XNPCActionMove;
class XNPCActionTalent;
class XNPCActionHit;
class XNPCAligner;

class XModuleNonControl : public XModuleActorControl, public MMemPool<XModuleNonControl>
{
	friend class XNPCActionTalent;
	friend class XNPCActionHit;
	friend class XNPCActionMove;

protected:

	//////////////////////////////////////////////////////////////////////////
	// Module 초기화
	//
	virtual void				OnInitialized();


	//////////////////////////////////////////////////////////////////////////
	// FSM
	//
	XNPCActionFSM*			m_pActionFSM;
	void					InitStates();
	void					DoAction(NPC_ACTION_STATE nState, void* pParam=NULL);			///< 액션 변경

	//////////////////////////////////////////////////////////////////////////
	// OnEvent 콜백
	//
	virtual void				OnSubscribeEvent();
	virtual XEventResult		OnEvent(XEvent& msg);

	virtual void				HitByTalent(XTalentHitParam * pInfo);

	void						DoNPCStop();

	void						LoadObjSound();
	void						ReleaseObjSound();

	//////////////////////////////////////////////////////////////////////////
	// OnUpdate : if pass CheckUpdatable,
	// OnUpdateState -> OnUpdateCommand -> OnUpdateSpecail
	//
	virtual void				OnUpdate(float fDelta);		///< 매 틱마다 호출

	virtual bool				OnUpdateCheckUpdatable(){ return true; }
	virtual void				OnUpdateState(float fDelta);
	virtual void				OnUpdateCommand(float fDelta);
	virtual void				OnUpdateSpecial(float fDelta);
	
	void						UpdateAlign(float fDelta);

	void						CheckMoveState();

	XNPCAligner*				m_pAligner;
public:

	//////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸 / Owner
	//
	XModuleNonControl(XObject* pOwner=NULL);
	virtual ~XModuleNonControl(void);

	XNonPlayer*					GetOwner();


	//////////////////////////////////////////////////////////////////////////
	// 네트웍에서 넘어온 커맨드로 동작
	//
	virtual void 				OnActTalent(int nTalentID, vec3& vDir);
	virtual void 				OnUseTalent(int nTalentID, vec3& vPos, vec3& vDir);
	virtual void				OnActSpellProjectile(int nTalentID, vec3& vEntityDir, vec3& vTargetPos, MUID& uidTarget, int nGroupID, int nCapsuleID);

	virtual void				OnActArchery(int nTalentID, vec3& vDir, MUID& uidTarget);
	virtual void				OnActArcheryDetail(int nTalentID, vec3& vDir, MUID& uidTarget, int nGroupID, int nCapsuleID);

	virtual void				OnCancelTalent();
	virtual void				OnHit(XTalentHitParam* pHitInfo);
	virtual void 				OnGuard(vec3& pos, vec3& dir);
	virtual void				OnGuardReleased();
	virtual void				OnDie(MF_STATE nMFState = MF_NONE, bool bSetAni = true);
	virtual void				OnDespawn();
	virtual void				OnRebirth();
	virtual void				OnSetLootable(bool bLootable);
	virtual void				OnSetDeSpawn();
	virtual void				OnDieAniEnd();
	virtual void				HitByBuff(XTalentHitParam * pHitInfo);

	virtual void				DoActionIdle();
	virtual void				DoActionMailOpen();;
	virtual void				DoActionMailClose();;

	virtual void				OnBallonText(wstring szID, wstring szChat, float fMaintainTime);

	void 						OnNPCMove(TD_NPC_MOVE* pNM, bool bRunning);
	void						OnNPCStop(vec3 vPos);
	void						OnNPCFaceTo(vec3 vPos, vec3 vDir);

	virtual void				OnSpew();
	virtual void				OnSpewInterrupt();
	virtual void				OnSwallowedSynch(int nTalentID, MUID& uidTarget);

	void						OnReInNonPlayer(TD_UPDATE_CACHE_NPC* pMonsterInfo);

	void						OnLootable(bool bLootable);

	XNPCAligner*				GetAligner()	{ return m_pAligner; }
	NPC_ACTION_STATE			GetActionStateID() { return (NPC_ACTION_STATE)m_pActionFSM->GetCurrentStateID(); }
	const wchar_t*				GetActionStateName();
};
