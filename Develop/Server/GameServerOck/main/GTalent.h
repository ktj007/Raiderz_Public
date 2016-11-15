#ifndef _GTALENT_H
#define _GTALENT_H

#include "GTalentEffector.h"
#include "GTalentMove.h"
#include "GTalentHitcapsule.h"
#include "GTalentScript.h"
#include "GTalentStress.h"
#include "GTalentHit.h"
#include "GTalentLogger.h"
#include "GTalentExpirer.h"
#include "GTalentEventChecker.h"

class GEntityActor;
class GTalentRouter;
class GTalent;
class GTalentInfo;
class GTalentPhaseFSM;

// 스킬 인스턴스, 엔티티가 실제 스킬을 사용하면 이 클래스가 생성된다.
class GTalent
{
protected:
	GTalentInfo*		m_pTalentInfo;
	GEntityActor*		m_pOwner;
	vec3				m_vInitialDir;		///< 처음 이 탤런트를 쓸 때의 방향
	vec3				m_vInitialPos;		///< 처음 이 탤런트를 쓸 때의 좌표
	vec3				m_vInitialTargetPos;///< 처음 이 탤런트를 쓸 때의 목표 좌표
	float				m_fElapsedTime;		///< 탤런트가 진행된 시간
	float				m_fActTime;			///< Act Phase에 진입한 시각
	TALENT_PHASE		m_nPhase;
	GTalentRouter*		m_pRouter;
	bool				m_bAutoCastingComplete;
	bool				m_bExpired;
	TALENT_TARGET_INFO	m_Target;
	GTalentEffector		m_TalentEffector;
	GTalentMove			m_TalentMove;
	GTalentHitcapsule	m_TalentHitcapsule;
	GTalentScript		m_TalentScript;
	GTalentStress		m_TalentStress;
	GTalentHit			m_TalentHit;
	GTalentLogger		m_TalentLogger;
	GTalentExpirer		m_TalentExpirer;
	GTalentEventChecker	m_TalentEventChecker;

	// 옵저버 집합
	vector<GTalentObserver*>	m_vecObservers;

private:
	GTalent();

	void			OnEnterPhase(TALENT_PHASE nPhase);
	void			OnLeavePhase(TALENT_PHASE nPhase);
	void			OnPhaseEnd();

	void			InitStates();
	float			InitExpiredTime();

	// 내장 옵저버를 전부 연결
	void			AttachObservers();
	// 내장 옵저버를 전부 분리
	void			DetachObservers();

	// 탤런트가 제대로 동작하여 효과를 얻게 함
	void			ApplyTalentEffect(TALENT_CONDITION nTalentCondition);

	// 히트캡슐일 경우, 히트캡슐 판정을 처리
	void			UpdateHit();

	void			ChangeNPCMode();
	
	virtual void	RouteActTalent();

	virtual void	SetExpierdTime();
	
protected:
	// phase
	virtual void	OnStart();
	// 탤런트가 종료될때 호출 (Finish or Canceled)
	virtual void	OnExit();
	virtual void	OnUpdatePhasePrepare(float fDelta);
	virtual void	OnUpdatePhaseAct(float fDelta);
	virtual void	OnUpdatePhaseExtra(float fDelta);
	virtual void	OnUpdatePhaseExtra2(float fDelta);
	virtual void	OnUpdatePhaseExtra3(float fDelta);
	virtual void	OnEnterPhasePrepare();
	virtual void	OnEnterPhaseAct();
	virtual void	OnEnterPhaseCanceled();
	virtual void	OnEnterPhaseFinish();
	virtual void	OnEnterPhaseExtra();
	virtual void	OnEnterPhaseExtra2();
	virtual void	OnEnterPhaseExtra3();
	virtual void	OnLeavePhaseAct();
	virtual void	OnLeavePhaseCanceled();
	virtual void	OnLeavePhasePrepare();
	virtual void	OnLeavePhaseFinish();
	virtual void	OnLeavePhaseExtra();
	virtual void	OnLeavePhaseExtra2();
	virtual void	OnLeavePhaseExtra3();
	virtual void	OnHitArea( GEntityActor* pVictim, uint16 nCheckTime, int8 nCapsuleGroupIndex, int8 nCapsuleIndex );

protected:
	void			ChangePhase(TALENT_PHASE nNextPhase);
	
public:
	GTalent(GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO uidTarget=TALENT_TARGET_INFO::Invalid(), bool bGainStress=true);
	virtual ~GTalent();

	bool			Update(float fDelta);
	void			UpdateExpired(float fDelta);
	virtual void	Start();

	void			Cancel(bool bPostCommand=true);
	void			CancelForce(bool bPostCommand);
	
	void			ActTalent(TALENT_TARGET_INFO Target);
	void			ApplyDelayedAct();

	void			OnGainDamage(GEntityActor* pTarget);

	bool IsCastingComplete();
	// 탤런트 사용자 반환
	GEntityActor*		GetOwner()			{ return m_pOwner; }
	// 탤런트 목표액터 반환
	TALENT_TARGET_INFO&	GetTarget()			{ return m_Target; }
	// 탤런트 목표액터 UID 반환
	MUID				GetTargetUID()		{ return m_Target(); }
	// 탤런트 원형정보 반환
	GTalentInfo*	GetInfo()		{ return m_pTalentInfo; }
	// 탤런트 아이디 반환
	int				GetID() { return m_pTalentInfo->m_nID; }
	// 탤런트 진행단계 반환
	TALENT_PHASE	GetPhase()			{ return m_nPhase; }
	// 피해자가 있는지 여부
	bool			HasVictim()			{ return m_TalentHit.HasVictim(); }

	vec3			GetInitPos()		{ return m_vInitialPos; }
	vec3			GetInitDir()		{ return m_vInitialDir; }
	vec3			GetInitTargetPos()	{ return m_vInitialTargetPos; }
	GTalentMove::MOVEINFO	GetExpectedMoveInfo(float fActElapsedTime);			///< 해당 ElapsedTime후의 예상되는 위치값
	GTalentRouter*	GetRouter()			{ return m_pRouter; }
	// 목표액터의 위치를 얻기, 반환값은 목표액터가 있는지 여부
	bool			GetTargetPos(vec3& vTargetPos);

	GEntityActor*  GetTargetActor();
	// Act Phase 후 경과된 시각 반환
	float			GetActElapsedTime();
	float			GetExpiredTime();
	
	bool			IsExpired();
	bool			IsNowInvincibleTime();
	bool			IsNowSuperarmorTime();
	bool			IsNowAvoidTime();
	bool			IsNowCasting();
	bool			IsCancelable();
	bool			IsMovable();

	// ACT시에 목표를 향해 방향을 설정할지 여부
	bool			IsAimingDirectionTalent();

	// 탤런트가 성공했음을 알림
	void			TalentFinish();

	void			HitArea(GEntityActor* pVictim, uint16 nCheckTime, int8 nCapsuleGroupIndex, int8 nCapsuleIndex);
	float			GetElapsedTime() { return m_fElapsedTime; }

	// 옵저버를 연결시킴
	void			AttachObserver(GTalentObserver* pOvr);	
};

#endif