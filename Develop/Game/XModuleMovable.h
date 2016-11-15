#ifndef _XMODULE_MOVABLE_H
#define _XMODULE_MOVABLE_H

#include "MTypelist.h"

#include "XModule.h"
#include "XMovableGoParam.h"
#include "XMovableProcessor.h"
#include "XMotion.h"
#include "XCharacter.h"
#include "XTimer.h"
#include "XObjectManager.h"

class XModuleEntity;
class XModuleCollision;

// 오브젝트 이동을 관리하는 모듈
class XModuleMovable : public XModule, public MMemPool<XModuleMovable>
{
	DECLARE_ID(XMID_MOVABLE);

	friend class XMovableProcessor;
	friend class XMovableProcInterpolation;
	friend class XMovableProcObject;
	friend class XMovableProcTerrain;
	friend class XMovableProcTerrainNPC;
	friend class XMovableProcWater;
	friend class XMovableProcThrow;

	//////////////////////////////////////////////////////////////////////////
	// ModuleMovable은 이동명령을 받았을때 해당 이동이 타당한지를 충돌확인
	// 등을 통해 판단하여 최종적으로 ModuleEntity에 좌표를 설정한다.
	//
	XModuleEntity*			m_pModuleEntity;
	XModuleCollision*		m_pModuleCollision;
	E_OWNER_TYPE			m_eOwnerType;


	//////////////////////////////////////////////////////////////////////////
	// Movable Factor
	//
	XMovableFactor			m_MovableFactor;
	bool					m_bActive;

	//////////////////////////////////////////////////////////////////////////
	// OnUpdate
	// _PreUpdate -> _RunMovableProcessor -> _PostUpdate 순서로 호출
	// _PreUpdate에서는 속도와 관련된 값들을 변경
	// _RunMovableProcessor에서는 움직임에 관련된 보간/충돌등의
	// 프로세서 통과후 실제 이동좌표 결정
	// _PostUpdate에서는 상태확인및 변경상태에 대한 메시지 보내기
	//
	void					_PreUpdateEnvironment();			// 환경에 관련된 값들
	void					_PreUpdateAnimationDummyNode();		// 더미노드 관련 값
	void					_PreUpdateKnockback(float fDelta);	// 넉백 관련 값
	void					__PreUpdateKnockback_UpVelocity( float fDelta, XMovableFactor::SKnockBack &KNOCK_BACK, vec3 &vForce );

	void					_RunMovableProcessor(float fDelat);	// 움직임에 관련된 프로세서 통과. 실제 이동좌표가 결정된다.

	void					_PostUpdateJump();					// 점프
	void					_PostUpdateFalling();				// 떨어짐 - 점프에 의한 낙하상태는 아니다.
	void					_PostUpdateReleaseJumpAndFall();	// 점프(Jump)와 떨어짐(Falling) 상태 풀림 - Landing
	void					_PostUpdateHoldExpception(float fDelta);		// 지형에 끼었을때 예외 처리
	void					_PostUpdateThrow();					// 던지기 상태 확인
	void					_PostUpdateSwim();					// 수영 상태 확인
	void					_PostUpdateInterpolation();			// 보간 움직임(Interpolation)에 의한 상태 확인
	void					_PostUpdateKnockback();				// 넉백
	void					_PostUpdateDie();					// 죽었을때 처리
	void					_PostUpdateMouseMove(float fDelta);	// 마우스 이동처리

	//////////////////////////////////////////////////////////////////////////
	// Movable Processor
	//
	enum MOVABLE_PROCESSOR
	{
		E_MOVABLE_PROCESSOR_INTERPOLATION,
		E_MOVABLE_PROCESSOR_THROW,
		E_MOVABLE_PROCESSOR_OBJECT,
		E_MOVABLE_PROCESSOR_TERRAIN,
		E_MOVABLE_PROCESSOR_WATER,

		E_MOVABLE_PROCESSOR_NUM
	};
	XMovableProcessor*		m_MovableProcessors[E_MOVABLE_PROCESSOR_NUM];

	void					DebugCheckRunProc(XMovableProcessor* pMovableProcessor, float fDelta, vec3 vCurrentPos, vec3 vTo);

	bool					CheckUsableDummyLoc();

protected:

	virtual void			OnInitialized();
	virtual void			OnUpdate(float fDelta);
	virtual XEventResult	OnEvent(XEvent& msg);
	virtual void			OnSubscribeEvent();

public:

	XModuleMovable(XObject* pOwner=NULL);
	virtual ~XModuleMovable(void);

	void SetInitialFactors(bool bUsingGravity);

	//////////////////////////////////////////////////////////////////////////
	//
	// 업데이트시에 필요한 정보들을 변경해주는 메쏘드
	// 스테이트 플래그와 Movable Factor 값을 변경해서
	// OnUpdate시에 영향을 줍니다.
	//
	//////////////////////////////////////////////////////////////////////////

	void					TriggerGo( XMovableGoParam* pParam);	///< XMovableGoParam 설정대로 움직임 알림
	inline void				TriggerStop();							///< 움직임 멈춤. 속도 0
	void					TriggerRotate(XMovableGoParam* pParam);

	void					TriggerSwim(bool bSwim);				///< 수영상태
	inline bool				TriggerJump(vec3* vResultDir = NULL, float fRatio =1.0f);	///< 점프상태, 올라가기/내려오기 모두 포함
	inline void				TriggerFall();							///< 점프에 의한 하강이 아닌 떨어짐상태
	inline void				TriggerDie(bool bDie);

	inline void				TriggerTalentActive(int nTalentActiveID);
	inline void				TriggerTalentDeActive();
	inline void				TriggerStance();
	inline void				TriggerStunActive();
	inline void				TriggerStunDeActive();

	inline void				TriggerGuardDefense(bool bDefense);

	//////////////////////////////////////////////////////////////////////////
	// 넉백상태 트리거
	//
	void					TriggerKnockback(const vec3& dir, const vec3& pos, const vec3& target, float fPower, float fVelocity, float fKBEventDist);
	void					TriggerUppered( const vec3& dir, const vec3& pos, const vec3& target, float fPower, float fUPDownTime, float fUpVelocity );
	void					TriggerDrag(const vec3& dir, const vec3& pos, const vec3& target, float fPower, float fKBEventDist);


	//////////////////////////////////////////////////////////////////////////
	// 잡혔을때, 먹혔을때 상태 트리거
	//
	inline void				TriggerGrappledStart( const MUID& _vAttacker, float fVelocity, vec3 vHitPos, float fInterpolationTime = 0.0f);
	void					TriggerGrappledBeThrown();
	inline void				TriggerGrappledEnd();

	//////////////////////////////////////////////////////////////////////////
	// 강제 설정
	//
	void					ForceStop(bool bAlsoStopAccelation = false);
	void					ForceJump(vec3* vResultDir = NULL, float fRatio =1.0f);

	//////////////////////////////////////////////////////////////////////////
	// 워프
	//
	void					Warp(const vec3& vPos, const vec3& vDir);

	//////////////////////////////////////////////////////////////////////////
	// Movable Factor
	//
	const XMovableFactor&	GetMovableFactor()	{ return m_MovableFactor; }
	void					InitMovableFactorFalling();
	
	//////////////////////////////////////////////////////////////////////////
	// ShovePlayer 업데이트
	//
	void					UpdateShove(vec3& vTo, float fDelta);

	void SetActive(bool b) { m_bActive = b; }
};

//----------------------------------------------------------------------------------------------------
inline void XModuleMovable::TriggerStop()
{
	if (((m_MovableFactor.GENERAL_STATE.m_bGoingDirection) && (m_MovableFactor.GENERAL_STATE.m_bLanding)) ||
		m_MovableFactor.MOUSE_MOVE.m_bWorking)
		ForceStop();
}
//----------------------------------------------------------------------------------------------------
inline void XModuleMovable::ForceStop(bool bAlsoStopAccelation)
{
	m_MovableFactor.GENERAL_STATE.m_bGoingDirection = false;
	m_MovableFactor.INTERPOLATION.m_bWorking = false;
	m_MovableFactor.MOUSE_MOVE.m_bWorking = false;

	m_MovableFactor.VELOCITY.m_vVelocity.Set(vec3::ZERO);
	m_MovableFactor.VELOCITY.m_bUsingAccel = !bAlsoStopAccelation;
}
//----------------------------------------------------------------------------------------------------
inline bool XModuleMovable::TriggerJump(vec3* vResultDir, float fRatio)
{
	if(m_MovableFactor.GENERAL_STATE.m_bJumping || m_MovableFactor.GENERAL_STATE.m_bSliding || m_MovableFactor.GRAPPLE.m_bGrappled) 
		return false;

	ForceJump(vResultDir, fRatio);
	return true;
}
//----------------------------------------------------------------------------------------------------
inline void XModuleMovable::ForceJump(vec3* vResultDir, float fRatio)
{
	vec3 vCurrentVel( m_MovableFactor.VELOCITY.m_vVelocity );
	vCurrentVel.z = 0.f;
	vCurrentVel.Normalize();

	if ( vResultDir)
		vResultDir->Set( vCurrentVel);


	vCurrentVel.x *= XCONST::MOVE_SPEED_NONE * fRatio;
	vCurrentVel.y *= XCONST::MOVE_SPEED_NONE * fRatio;
//	vCurrentVel.z = XCONST::JUMP_SPEED * ( (fRatio > 1.0f) ? (1.0f / fRatio) : fRatio);
	vCurrentVel.z = XCONST::JUMP_SPEED * fRatio;

	m_MovableFactor.GENERAL_STATE.m_bLanding		= false;
	m_MovableFactor.GENERAL_STATE.m_bJumping		= true;
	m_MovableFactor.GENERAL_STATE.m_bJumpAscending	= true;
	m_MovableFactor.VELOCITY.m_vVelocity = vCurrentVel;
	m_MovableFactor.GENERAL_STATE.m_vJumpPosForCamera = m_pOwner->GetPosition();
	m_MovableFactor.INTERPOLATION.m_fJumpSpeedRatio = fRatio;
}
//----------------------------------------------------------------------------------------------------
inline void XModuleMovable::TriggerFall()
{
	m_MovableFactor.GENERAL_STATE.m_bFallingButNotJump = true;
}
//----------------------------------------------------------------------------------------------------
inline void	XModuleMovable::TriggerGrappledStart( const MUID& _vAttacker, float fVelocity, vec3 vHitPos, float fInterpolationTime /*= 0.0f*/)
{
	m_MovableFactor.GENERAL_STATE.m_bJumping			= false;
	m_MovableFactor.GENERAL_STATE.m_bFallingButNotJump	= false;
	m_MovableFactor.GENERAL_STATE.m_bLanding			= false;
	m_MovableFactor.GENERAL_STATE.m_bSwim				= false;
	m_MovableFactor.GENERAL_STATE.m_bColTerrain			= false;

	m_MovableFactor.GRAPPLE.m_bGrappled = true;
	m_MovableFactor.GRAPPLE.m_uidGrappledAttacker = _vAttacker;
	m_MovableFactor.GRAPPLE.m_vGrappledVelocity = fVelocity;
	m_MovableFactor.GRAPPLE.m_bGrappledBeThrown = false;
	m_MovableFactor.GRAPPLE.m_fGrappledElapsed = 0.0f;
	m_MovableFactor.GRAPPLE.m_fGrappledStartInterpolationTime = fInterpolationTime;
	m_MovableFactor.GRAPPLE.m_fGrappledRemainStartInterpolationTime = fInterpolationTime;
	m_MovableFactor.GRAPPLE.m_vGrappledStartDir = m_pOwner->GetDirection();
	m_MovableFactor.GRAPPLE.m_vGrappledReadyPos = m_pOwner->GetPosition();
	m_MovableFactor.GRAPPLE.m_vGrappledStartPos = vHitPos;
	m_MovableFactor.GRAPPLE.m_vGrappledEndDir = m_pOwner->GetDirection();
}
//----------------------------------------------------------------------------------------------------
inline void	XModuleMovable::TriggerGrappledEnd()
{
	m_MovableFactor.GRAPPLE.m_bGrappled = false;
	m_MovableFactor.GRAPPLE.m_bGrappledStuck = false;
	m_MovableFactor.GRAPPLE.m_uidGrappledAttacker = MUID::ZERO;
	m_MovableFactor.GRAPPLE.m_bGrappledBeThrown = false;
	m_MovableFactor.GRAPPLE.m_vGrappledVelocity = vec3::ZERO;
}
//----------------------------------------------------------------------------------------------------
inline void XModuleMovable::TriggerTalentActive(int nTalentActiveID)
{
	m_MovableFactor.GENERAL_STATE.m_bTalentActive		= true;
	m_MovableFactor.GENERAL_STATE.m_nTalentActiveID		= nTalentActiveID;
}
//----------------------------------------------------------------------------------------------------
inline void	XModuleMovable::TriggerTalentDeActive()
{
	m_MovableFactor.GENERAL_STATE.m_bTalentActive		= false;
	m_MovableFactor.GENERAL_STATE.m_nTalentActiveID		= -1;
}
//----------------------------------------------------------------------------------------------------
inline void XModuleMovable::TriggerGo( XMovableGoParam* pParam )
{
	_ASSERT( pParam != NULL );

	// 기본 사용 초기화
	m_MovableFactor.GENERAL_STATE.m_bGoingDirection = false;
	m_MovableFactor.INTERPOLATION.m_bWorking = false;
	m_MovableFactor.MOUSE_MOVE.m_bWorking = false;

	if(pParam->m_nType == XMovableGoParam::E_GO_DIRECTION)
	{
		// 속도 설정
		float fSpeed = pParam->m_fMoveSpeed;
		vec3& vMovementDir = pParam->m_vTargetDir.Normalize();
		if (! m_MovableFactor.GENERAL_STATE.m_bLanding)
		{
			fSpeed *= 0.2f;
		}

		m_MovableFactor.VELOCITY.m_vVelocity.x = vMovementDir.x * fSpeed;
		m_MovableFactor.VELOCITY.m_vVelocity.y = vMovementDir.y * fSpeed;

		// 관련 플래그 설정
		m_MovableFactor.GENERAL_STATE.m_bGoingDirection = true;
	}
	else if(pParam->m_nType == XMovableGoParam::E_GO_INTERPOLATION)
	{
		// 인터폴레이션 프로세서를 통한 이동.
		// SetPosDir을 할때 인터폴레이션 프로세서도 활성화 된다.
		// 속도는 인터폴레이션 프로세서 가 처리해준다.
		m_MovableFactor.INTERPOLATION.Set(
			pParam->m_vTargetPos, pParam->m_vTargetDir, pParam->m_fMoveSpeed, pParam->m_fRotateSpeed, pParam->m_nBoostFlags,
			pParam->m_nMovementFlags, pParam->m_bRunMotion, pParam->m_fLimitTime, pParam->m_bForceInterpolation, pParam->m_eMoveCheckType);

		m_MovableFactor.INTERPOLATION.m_vStartPosForDummyMove = m_pOwner->GetPosition();
	}
	else if(pParam->m_nType == XMovableGoParam::E_GO_MOUSE_MOVE)
	{
		// 마우스로 이동
		// 인터폴레이션과 동일한 이동이다.
		// 타입을 분류로 통해서 제어를 하기 위해서 추가 된것이다.
		m_MovableFactor.INTERPOLATION.Set(
			pParam->m_vTargetPos, pParam->m_vTargetDir, pParam->m_fMoveSpeed, pParam->m_fRotateSpeed, pParam->m_nBoostFlags,
			pParam->m_nMovementFlags, pParam->m_bRunMotion, pParam->m_fLimitTime, pParam->m_bForceInterpolation, pParam->m_eMoveCheckType);

		m_MovableFactor.MOUSE_MOVE.m_bWorking = true;
	}
}
//----------------------------------------------------------------------------------------------------
inline void XModuleMovable::TriggerStance()
{
	m_MovableFactor.GENERAL_STATE.m_bStance = true;
}
//----------------------------------------------------------------------------------------------------
inline void XModuleMovable::TriggerStunActive()
{
	m_MovableFactor.GENERAL_STATE.m_bStun = true;
}
//----------------------------------------------------------------------------------------------------
inline void XModuleMovable::TriggerStunDeActive()
{
	m_MovableFactor.GENERAL_STATE.m_bStun = false;
}
//----------------------------------------------------------------------------------------------------
inline void XModuleMovable::TriggerDie( bool bDie )
{
	ForceStop();

	m_MovableFactor.GENERAL_STATE.m_bDie = bDie;
}

inline void XModuleMovable::TriggerGuardDefense( bool bDefense )
{
	m_MovableFactor.GENERAL_STATE.m_bGuardDefense = bDefense;
}
//----------------------------------------------------------------------------------------------------
#endif // _XMODULE_MOTION_H