#include "StdAfx.h"
#include "XModuleMovable.h"
#include "XModuleEntity.h"
#include "XModuleMotion.h"
#include "XModuleCollision.h"
#include "XModuleTalent.h"
#include "XModuleBuff.h"
#include "XEventID.h"
#include "XGame.h"
#include "XWorld.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "XMovableDebugger.h"
#include "XMotionFactorHelper.h"

#include "XMovableProcInterpolation.h"
#include "XMovableProcObject.h"
#include "XMovableProcTerrain.h"
#include "XMovableProcTerrainNPC.h"
#include "XMovableProcThrow.h"
#include "XMovableProcWater.h"

#include "RDebugPassRenderer.h"

#ifdef _DEBUG_SHOW_MOVABLE_PROC_PASSEED
std::deque<bool> bPassed;
#endif

#define UPPERED_MAX_VELOCITY		2000.0f
#define DRAG_MAX_VELOCITY			2000.0f

XModuleMovable::XModuleMovable(XObject* pOwner) : XModule(pOwner), m_pModuleEntity(NULL), m_bActive(true)
{
	// 프로세서 컨테이너 초기화
	ZeroMemory( m_MovableProcessors, sizeof(m_MovableProcessors) );

	// 초기 환경값 업데이트
	_PreUpdateEnvironment();
}

XModuleMovable::~XModuleMovable(void)
{
	for (int i = 0; i < E_MOVABLE_PROCESSOR_NUM; ++i )
	{
		if (m_MovableProcessors[i])
		{
			m_MovableProcessors[i]->Fini();
			delete m_MovableProcessors[i];
		}
	}
}

void XModuleMovable::OnInitialized()
{
	XModule::OnInitialized();

	m_pModuleEntity = m_pOwner->GetModuleEntity();
	_ASSERT(m_pModuleEntity);

	m_pModuleCollision = m_pOwner->GetModuleCollision();
	_ASSERT(m_pModuleCollision);

	// set owner type
	m_eOwnerType = DecideOwnerType();

	// Movable Processor 생성
	// 공통 프로세서
	m_MovableProcessors[E_MOVABLE_PROCESSOR_INTERPOLATION]	= new XMovableProcInterpolation;
	m_MovableProcessors[E_MOVABLE_PROCESSOR_THROW]			= new XMovableProcThrow;
	m_MovableProcessors[E_MOVABLE_PROCESSOR_OBJECT]			= new XMovableProcObject;
	m_MovableProcessors[E_MOVABLE_PROCESSOR_WATER]			= new XMovableProcWater;

	// 개별 프로세서
	switch( m_eOwnerType )
	{
	case E_OWNER_MY_PLAYER:
	case E_OWNER_NET_PLAYER:
		{
			m_MovableProcessors[E_MOVABLE_PROCESSOR_TERRAIN]	= new XMovableProcTerrain;
		}
		break;

	case E_OWNER_ACTOR:
	case E_OWNER_NPC:
		{
			m_MovableProcessors[E_MOVABLE_PROCESSOR_TERRAIN]	= new XMovableProcTerrainNPC;
		}
		break;
	}

	// Movable Processor 초기화
	for (int i = 0; i < E_MOVABLE_PROCESSOR_NUM; ++i )
	{
		_ASSERT(m_MovableProcessors[i] != NULL);
		m_MovableProcessors[i]->Init(this);
	}
}

void XModuleMovable::OnUpdate(float fDelta)
{
	if (m_bActive == false) return;

	// 최대 틱이 1초 이상일 수 없다.
	if (fDelta > 1.0f)
	{
		fDelta = 1.0f;
	}

	PFI_BLOCK_THISFUNC(307);
	PFC_THISFUNC;

	vec3 vCurrentPos(m_pOwner->GetPosition());
	vec3 vCurrentDir(m_pOwner->GetDirection());
	m_MovableFactor.m_vResultPos.Set( vCurrentPos );
	m_MovableFactor.m_vResultDir.Set( vCurrentDir );

	// Update Movable Factor
	_PreUpdateEnvironment();
	_PreUpdateAnimationDummyNode();
	_PreUpdateKnockback(fDelta);

	// run processor
	_RunMovableProcessor(fDelta);

	// post update and dispatch event
	_PostUpdateKnockback();
	_PostUpdateJump();
	_PostUpdateFalling();
	_PostUpdateSwim();
	_PostUpdateReleaseJumpAndFall();
	_PostUpdateThrow();
	_PostUpdateHoldExpception(fDelta);
	_PostUpdateMouseMove(fDelta);
	_PostUpdateDie();

	//if(m_pOwner->GetType() == XOBJ_NET_PLAYER)
	//{
	//	float fDummyMove = m_MovableFactor.MOVING_DUMMY.m_vDummyPosElipsed.Length();
	//	float fRealMove = (m_MovableFactor.m_vResultPos - vCurrentPos).Length();
	//	if(fDummyMove > 0)
	//	{
	//		mlog("위치 체크 %f : %f\n", fDummyMove, fRealMove);
	//	}
	//}
	// 최종적으로 좌표 설정
	//m_pModuleEntity->SetRenderPosDir(vec3::ZERO, vec3::ZERO);
	m_pModuleEntity->SetUpVector(m_MovableFactor.m_vResultUpVec, this);
	m_pModuleEntity->SetDirection(m_MovableFactor.m_vResultDir, this);
	m_pModuleEntity->SetPosition(m_MovableFactor.m_vResultPos, this);

	// 최종 좌표후 처리해야하는 함수
	_PostUpdateInterpolation();

	//RMatrix matT;
	//matT.SetLocalMatrix(m_MovableFactor.m_vResultPos, m_MovableFactor.m_vResultDir, m_MovableFactor.m_vResultUpVec);
	//REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugBoxAndAxis( matT, 10, 10.0f, D3DCOLOR_XRGB(255, 255, 0));
}


// 에니메이션의 변한 위치값
void XModuleMovable::_PreUpdateAnimationDummyNode()
{
	// 초기화
	XMovableFactor::SMovingDummy& MOVING_DUMMY = m_MovableFactor.MOVING_DUMMY;
	XMovableFactor::SInterpolation& INTERPOL = m_MovableFactor.INTERPOLATION;

	//XModuleMotion* pModuleMotion = m_pOwner->GetModuleMotion();

	// 조건에 관계없이 애니메이션에 더미 노드가 있는지 체크합니다. (예전에는 조건을 따졌는데... 그거 없애 봅니다.)
	if ( m_pModuleEntity->IsHaveDummy() == true/* &&
		CheckUsableDummyLoc()*/)
	{
#ifdef _MADDUCK2
		CheckUsableDummyLoc();
#endif
		// Get dummy position
		vec3 vAniPos;
		m_pModuleEntity->GetMovingDummyPos( vAniPos);

		// 내 캐릭터는 현재 바라보는 방향에 대해서 변환하고, 그렇지 않으면 보간 시작 방향에 대해서 변환한다
		MMatrix tm;
		tm.SetLocalMatrix(vec3::ZERO, (m_eOwnerType == E_OWNER_MY_PLAYER) ? -m_pOwner->GetDirection() : -INTERPOL.m_vStartDir, vec3::AXISZ);
		vAniPos = (vAniPos * tm) * m_pOwner->GetScale();

		// Get dummy rotation
		vec3 vAniDir;
		m_pModuleEntity->GetMovingDummyDir( vAniDir);

		vec3 dir = -vec3::AXISY;
		float _rotate = dir.AngleToXY( vAniDir);

		MOVING_DUMMY.m_bHaveDummy		= true;
		MOVING_DUMMY.m_vDummyPosElipsed	= vAniPos - MOVING_DUMMY.m_vDummyPosition;
		MOVING_DUMMY.m_vDummyPosition	= vAniPos;
		MOVING_DUMMY.m_vDummyDirElipsed	= MOVING_DUMMY.m_vDummyDirection.AngleToXY( vAniDir);
		MOVING_DUMMY.m_vDummyDirection	= vAniDir;
	}
	else
	{
		MOVING_DUMMY.m_bHaveDummy		= false;
		MOVING_DUMMY.m_vDummyPosition	= vec3::ZERO;
		MOVING_DUMMY.m_vDummyPosElipsed	= vec3::ZERO;
		MOVING_DUMMY.m_vDummyDirection	= -vec3::AXISY;
		MOVING_DUMMY.m_vDummyDirElipsed	= 0.0f;
	}


}

// 주위 환경에 의한 위치 변경값
void XModuleMovable::_PreUpdateEnvironment()
{
	if (!m_MovableFactor.VELOCITY.m_bUsingGravity)
	{
		return;
	}

	if (!m_MovableFactor.VELOCITY.m_bUsingAccel)
	{
		m_MovableFactor.VELOCITY.m_vAccel.Set(0.f, 0.f, 0.f);
		return;
	}

	float fMoveSpeedRatio = 1.0f;
	if ( m_pOwner->GetEntityType() == ETID_NPC  ||  m_pOwner->GetEntityType() == ETID_PLAYER)
	{
		XModuleBuff* pModuleBuff = m_pOwner->GetModuleBuff();
		if ( pModuleBuff)
		{
			XBuffAttribute attrBuff = pModuleBuff->GetBuffAttribute();
			fMoveSpeedRatio = attrBuff.m_fMoveSpeedRatio;
		}
	}

	vec3 vEnv = vec3(0.0f, 0.0f, GRAVITY * fMoveSpeedRatio);		// 중력값
	m_MovableFactor.VELOCITY.m_vAccel = vEnv;
}

void XModuleMovable::_PreUpdateKnockback(float fDelta)
{
	// Knockback 처리
	XMovableFactor::SKnockBack& KNOCK_BACK = m_MovableFactor.KNOCK_BACK;

	if (KNOCK_BACK.m_bKnockback)
	{
		vec3 vForce = KNOCK_BACK.m_vKnockbackDir;
		vForce.Normalize();

		const float KB_MOVE_TIME_RATE		= 0.3f;
		const float KB_SLIDING_TIME_RATE	= 1.0f - KB_MOVE_TIME_RATE;
		const float KB_MOVE_TIME			= KNOCK_BACK.m_fKBFullTime * KB_MOVE_TIME_RATE;
		const float KB_SLIDING_TIME			= KNOCK_BACK.m_fKBFullTime * KB_SLIDING_TIME_RATE;
		const float KB_SLIDING_LEN			= KNOCK_BACK.m_fKBFullPower * 0.2f;
		const float KB_MOVE_LEN				= KNOCK_BACK.m_fKBFullPower - KB_SLIDING_LEN;

		KNOCK_BACK.m_fKBElapsedTime -= fDelta;
		if(KNOCK_BACK.m_fKBElapsedTime <= 0.0f)
			KNOCK_BACK.m_fKBElapsedTime = 0.0f;

		float fCurrPower = KNOCK_BACK.m_fKBPower;
		float fCurrMove = 0.0f;

		fCurrMove = (KB_MOVE_LEN / KB_MOVE_TIME) * fDelta;
		fCurrPower = KNOCK_BACK.m_fKBPower - fCurrMove;

		if(fCurrPower < 0.0f)
			fCurrMove = KNOCK_BACK.m_fKBPower;

		// 넉백 슬라이딩
		if(fCurrPower <= KB_SLIDING_LEN)
		{
			float x = MMath::HALFPI * ((KNOCK_BACK.m_fKBElapsedTime) / KB_SLIDING_TIME);
			float fSlidingMove = KB_SLIDING_LEN * (1.0f - cosf(x));

			if(fSlidingMove < 0.0f)
			{
				fSlidingMove = 0.0f;
			}

			fCurrMove = KNOCK_BACK.m_fKBPower - fSlidingMove;
			fCurrPower = fSlidingMove;

			if(fCurrMove < 0.0f)
				fCurrMove = KNOCK_BACK.m_fKBPower;
		}

		vForce = fCurrMove * vForce;
		KNOCK_BACK.m_fKBPower = fCurrPower;


		// 높이 Delta 추가
		__PreUpdateKnockback_UpVelocity(fDelta, KNOCK_BACK, vForce);



		KNOCK_BACK.m_vKnockBackDelta.Set(vForce);

		bool bWillSendEvent = false;
		if (KNOCK_BACK.m_fKBEventDistance > 0.0f && 
			KNOCK_BACK.m_fKBPower < KNOCK_BACK.m_fKBEventDistance)
		{
			bWillSendEvent = true;
		}
		if (KNOCK_BACK.m_fUPDownTime > 0.0f &&
			KNOCK_BACK.m_fKBFullTime - KNOCK_BACK.m_fKBElapsedTime >= KNOCK_BACK.m_fUPDownTime)
		{
			bWillSendEvent = true;
		}

		if (bWillSendEvent)
		{
			if (!KNOCK_BACK.m_bKBSentEvent)
			{
				XEvent msg;
				msg.m_nID = XEVTL_ON_MOTION_EVENT; 
				msg.m_pData = MOTION_EVENT_MF_KNOCKBACK_EVENT;
				m_pOwner->Event(msg);

				KNOCK_BACK.m_bKBSentEvent = true;
			}
		}

		//vec3 bottom = m_pOwner->GetPosition() + KNOCK_BACK.m_vKnockBackDelta + vec3(0, 0, 10);
		//vec3 top	= bottom + vec3(0, 0, 100);
		// 에머럴드 색
		//REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugCapsule(MCapsule(bottom, top, 10), 5, D3DCOLOR_XRGB(255, 255, 0));

		return;
	}

	KNOCK_BACK.m_vKnockBackDelta.Set(vec3::ZERO);
}

void XModuleMovable::__PreUpdateKnockback_UpVelocity( float fDelta, XMovableFactor::SKnockBack &KNOCK_BACK, vec3 &vForce )
{
	if (KNOCK_BACK.m_fKBUpVelocity > 0.01f)
	{
		const float fElapsedTime = KNOCK_BACK.m_fKBFullTime - KNOCK_BACK.m_fKBElapsedTime;

		float fKBEventTime = (KNOCK_BACK.m_fKBEventDistance / 100.0f) * KNOCKBACK_TIME;

		if (fElapsedTime <= KNOCK_BACK.m_fUPDownTime)
		{
			float fDecTime = KNOCK_BACK.m_fUPDownTime - 0.2f;
			if (fElapsedTime >= fDecTime)
			{
				KNOCK_BACK.m_fKBUpVelocity -= ((KNOCK_BACK.m_fKBUpVelocity * fDelta) * 5.0f);

				KNOCK_BACK.m_fKBUpVelocity = max(0.0f, KNOCK_BACK.m_fKBUpVelocity);
			}

			vForce.z += ((KNOCK_BACK.m_fKBUpVelocity + 800.0f) * fDelta);

			// 낙하 데미지를 주려고..
			m_MovableFactor.GENERAL_STATE.m_fFallingStartHeight = 
				max(m_MovableFactor.m_vResultPos.z, m_MovableFactor.GENERAL_STATE.m_fFallingStartHeight);

		}
	}
}

void XModuleMovable::_RunMovableProcessor(float fDelta)
{
	// 디버기 정보
	#ifdef _DEBUG_SHOW_MOVABLE_PROC_PASSEED
	bPassed.clear();
	#endif

	vec3 vCurrentPos( m_pModuleEntity->GetPosition() );
	vec3 vCurrentDir( m_pModuleEntity->GetDirection() );

	XMovableFactor::SVelocity& VEL = m_MovableFactor.VELOCITY;
	const XMovableFactor::SKnockBack& KNOCK_BACK = m_MovableFactor.KNOCK_BACK;
	const XMovableFactor::SMovingDummy& MOVING_DUMMY = m_MovableFactor.MOVING_DUMMY;
	XMovableFactor::SInterpolation& INTERPOL = m_MovableFactor.INTERPOLATION;


	// 속도 구하기
	VEL.m_vVelocity += VEL.m_vAccel * fDelta;

	// 포지션 구하기
	vec3 vTo = (VEL.m_vVelocity * fDelta) + KNOCK_BACK.m_vKnockBackDelta;
	vec3 vDir = vCurrentDir;
	vec3 vUpVec = vec3::AXISZ;


	// 내 플레이어인 경우에는 직접 더미 이동 값을 처리한다.  원래는 내 플레이어도 보간 처리를 항상 할 수 있도록 했어야 한다. =ㅅ=;
	if ( m_eOwnerType == E_OWNER_MY_PLAYER)
	{
		vTo += MOVING_DUMMY.m_vDummyPosElipsed;

// 		vec3 _dir;
// 		_dir.x = vCurrentDir.x * cos( MOVING_DUMMY.m_vDummyDirElipsed) - vCurrentDir.y * sin( MOVING_DUMMY.m_vDummyDirElipsed);
// 		_dir.y = vCurrentDir.y * cos( MOVING_DUMMY.m_vDummyDirElipsed) + vCurrentDir.x * sin( MOVING_DUMMY.m_vDummyDirElipsed);
// 		XMath::NormalizeZ0( _dir);
// 		vDir = vCurrentDir = _dir;
	}


	// 충돌등에 의해 움직임을 결정하도록 프로세서 수행
	if(vTo != vec3::ZERO)
	{
		// run processor
		for (int i = 0; i < E_MOVABLE_PROCESSOR_NUM; ++i)
		{
			XMovableProcessor* pMovableProcessor = m_MovableProcessors[i];

			// 디버깅 정보
			_ASSERT(pMovableProcessor != NULL);
			#ifdef _DEBUG_SHOW_MOVABLE_PROC_PASSEED
			bPassed.push_back(pMovableProcessor->IsProcessable(m_MovableFactor));
			#endif

			if (pMovableProcessor->IsProcessable(m_MovableFactor) == true)
			{
				vec3 vToFirst = vTo;

				pMovableProcessor->Run(m_MovableFactor, vCurrentPos, vCurrentDir, vTo, vDir, vUpVec, fDelta);

				DebugCheckRunProc(pMovableProcessor, fDelta, vCurrentPos, vTo - vToFirst);
			}
		}
	}

	// 제한 시간내에 이동을 완료하지 못했다면 워프 시킴
	if ( INTERPOL.m_fLimitTime > NO_LIMIT_TIME  &&  INTERPOL.m_fElipsedTime > INTERPOL.m_fLimitTime)
	{
#ifdef _DEBUG
		mlog( "이동 제한시간 초과. 워프되었습니다. position(%.1f, %.1f)\n", INTERPOL.m_vTargetPos.x, INTERPOL.m_vTargetPos.y);
#endif
		m_MovableFactor.m_vResultPos.Set( INTERPOL.m_vTargetPos);
		m_MovableFactor.m_vResultDir.Set(vDir);
		m_MovableFactor.m_vResultUpVec.Set(vUpVec);
	}

	// 계산된 위치로 이동시킴
	else
	{
		m_MovableFactor.m_vResultPos.Set(vCurrentPos + vTo);
		m_MovableFactor.m_vResultDir.Set(vDir);
		m_MovableFactor.m_vResultUpVec.Set(vUpVec);
	}

	//////////////////////////////////////////////////////////////////////////
	// 디버그 메시지
	//
	#ifdef _DEBUG_SHOW_MOVABLE_PROC_PASSEED
	if (m_eOwnerType == E_OWNER_MY_PLAYER)
	{
		mlog("무버블 : MyPlayer, %d Passed\n", (int)bPassed.size() );
		if(bPassed[E_MOVABLE_PROCESSOR_INTERPOLATION] == true)
		{
			mlog(" - 인터폴레이션인터폴레이션 하면 안돼는데...\n");
		}
	}
	else if (m_eOwnerType == E_OWNER_NET_PLAYER)
	{
		mlog("무버블 : NetPlayer, %d Passed\n", (int)bPassed.size() );
	}
	else if (m_eOwnerType == E_OWNER_NPC)
	{
		mlog("무버블 : NPC, %d Passed\n", (int)bPassed.size() );
	}
	else
	{
		mlog("무버블 : 뭥미?\n");
	}
	#endif
}

XEventResult XModuleMovable::OnEvent(XEvent& msg)
{
	XModule::OnEvent(msg);

	switch (msg.m_nID)
	{

	//////////////////////////////////////////////////////////////////////////
	// 모션 변경시 검사해주어야 할것들
	//

	// 모든 모션에 대해
	case XEVTL_ON_CHANGE_ANIMATION:
	case XEVTL_ON_CHANGE_MOTION:
		{
			XMovableFactor::SMovingDummy& MOVING_DUMMY = m_MovableFactor.MOVING_DUMMY;
			XMovableFactor::SInterpolation& INTERPOL = m_MovableFactor.INTERPOLATION;

			MOVING_DUMMY.m_vDummyPosition	= vec3::ZERO;
			MOVING_DUMMY.m_vDummyPosElipsed = vec3::ZERO;
			MOVING_DUMMY.m_vDummyDirection	= -vec3::AXISY;
			MOVING_DUMMY.m_vDummyDirElipsed	= 0.0f;

			INTERPOL.m_vStartPos = INTERPOL.m_vTargetPos;
			INTERPOL.m_vStartDir = INTERPOL.m_vTargetDir;

			return MR_TRUE;
		}

	// IDLE 모션으로 변경후에
	case XEVTL_ON_CHANGED_MOTION_TO_IDLE:
		{
			// 스턴스 중이었는지...
			if(m_MovableFactor.GENERAL_STATE.m_bStance)
			{
				m_MovableFactor.GENERAL_STATE.m_bStance = false;
			}

			return MR_TRUE;
		}

	case XEVTD_MESH_LOADING_COMPLETE:
		{
			return MR_TRUE;
		}
	case XEVTL_THROW:
		{
			m_MovableFactor.GENERAL_STATE.m_bFallingStartRecording = true;
			m_MovableFactor.GENERAL_STATE.m_fFallingStartHeight = m_MovableFactor.m_vResultPos.z;

			return MR_TRUE;
		}
	};

	return MR_FALSE;
}

void XModuleMovable::OnSubscribeEvent()
{
	XModule::OnSubscribeEvent();

	Subscribe(XEVTL_ON_CHANGE_ANIMATION);
	Subscribe(XEVTL_ON_CHANGE_MOTION);
	Subscribe(XEVTL_ON_CHANGED_MOTION_TO_IDLE);
	Subscribe(XEVTD_MESH_LOADING_COMPLETE);
	Subscribe(XEVTL_THROW);
}

void XModuleMovable::_PostUpdateJump()
{
	if (m_MovableFactor.GENERAL_STATE.m_bJumping && m_MovableFactor.GENERAL_STATE.m_bJumpAscending)
	{
		if (m_MovableFactor.VELOCITY.m_vVelocity.z <= 0)
		{
			m_MovableFactor.GENERAL_STATE.m_bJumpAscending = false;
			m_MovableFactor.GENERAL_STATE.m_bFallingStartRecording = true;
			m_MovableFactor.GENERAL_STATE.m_fFallingStartHeight = m_MovableFactor.m_vResultPos.z;

			XEvent evt;
			evt.m_nID = XEVTL_ON_MOTION_EVENT; 
			evt.m_pData = MOTION_EVENT_GRAVITY_DOWN;
			m_pOwner->Event(evt);
		}
	}

	if(m_MovableFactor.GENERAL_STATE.m_bJumping)
	{
		// 카메라를 위한 위치 계산... 실제 점프 높이의 1/2만큼만 올라간다.
		float fHeightforCamera = (m_MovableFactor.m_vResultPos.z - m_pOwner->GetPosition().z);
		
		if(fHeightforCamera != 0.0f)
			fHeightforCamera = (fHeightforCamera * 2.0f) / 3.0f;

		m_MovableFactor.GENERAL_STATE.m_vJumpPosForCamera.x = m_MovableFactor.m_vResultPos.x;
		m_MovableFactor.GENERAL_STATE.m_vJumpPosForCamera.y = m_MovableFactor.m_vResultPos.y;
		m_MovableFactor.GENERAL_STATE.m_vJumpPosForCamera.z += fHeightforCamera;

		m_MovableFactor.GENERAL_STATE.m_fCalJumpHeight += fHeightforCamera;
		if(m_MovableFactor.GENERAL_STATE.m_fCalJumpHeight <= 0.0f)
			m_MovableFactor.GENERAL_STATE.m_vJumpPosForCamera.z = m_MovableFactor.m_vResultPos.z;
	}
	else
	{
		m_MovableFactor.GENERAL_STATE.m_fCalJumpHeight = 0.0f;
	}
}

void XModuleMovable::TriggerKnockback( const vec3& dir, const vec3& pos, const vec3& target, float fPower, float fVelocity, float fKBEventDist )
{
	XMovableFactor::SKnockBack& KNOCK_BACK = m_MovableFactor.KNOCK_BACK;

	// 넉백 재계산, 맞는 위치가 다를 수 있다.
	// 서버의 최종 위치에 맞춘다.
	vec3 vArrivePos = target;
	vec3 vArriveDir = vArrivePos - m_pOwner->GetPosition();

	float fReCalPower = vArriveDir.Length();
	if(fReCalPower < 0.0f)
		fReCalPower = 0.0f;

	// 데이터 셋팅
	KNOCK_BACK.m_vKnockbackDir.Set(vArriveDir);
	KNOCK_BACK.m_vKnockbackDir.Normalize();
	KNOCK_BACK.m_vKnockBackCalcPos.Set(m_pOwner->GetPosition());

	KNOCK_BACK.m_fKBFullPower = fReCalPower;
	KNOCK_BACK.m_fKBPower = fReCalPower;
	KNOCK_BACK.m_fKBVel = fVelocity;
	KNOCK_BACK.m_fKBFullTime = XMotionFactorHelper::CalcKnockbackTime(fPower);
	KNOCK_BACK.m_fKBElapsedTime = KNOCK_BACK.m_fKBFullTime;


	KNOCK_BACK.m_fKBUpVelocity = 0.0f;
	KNOCK_BACK.m_fUPDownTime = 0.0f;

	KNOCK_BACK.m_bKnockback = true;
	KNOCK_BACK.m_bKBSentEvent = false;
	KNOCK_BACK.m_fKBEventDistance = fKBEventDist;

	m_MovableFactor.INTERPOLATION.m_bWorking = false;

	m_MovableFactor.VELOCITY.m_vVelocity.Set(vec3::ZERO);

	//vec3 bottom = pos + vec3(0, 0, 10);
	//vec3 top	= bottom + vec3(0, 0, 100);
	//REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugCapsule(MCapsule(bottom, top, 10), 5, D3DCOLOR_XRGB(0, 255, 255));

	//bottom = m_pOwner->GetPosition() + vec3(0, 0, 10);
	//top	= bottom + vec3(0, 0, 100);
	//REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugCapsule(MCapsule(bottom, top, 10), 5, D3DCOLOR_XRGB(255, 0, 255));

	//bottom = vArrivePos + vec3(0, 0, 10);
	//top = bottom + vec3(0, 0, 100);
	//REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugCapsule(MCapsule(bottom, top, 10), 5, D3DCOLOR_XRGB(255, 255, 255));
}

void XModuleMovable::TriggerUppered( const vec3& dir, const vec3& pos, const vec3& target, float fPower, float fUPDownTime, float fUpVelocity )
{
	XMovableFactor::SKnockBack& KNOCK_BACK = m_MovableFactor.KNOCK_BACK;
	TriggerKnockback(dir, pos, target, fPower, UPPERED_MAX_VELOCITY,  0.0f);

	KNOCK_BACK.m_fKBUpVelocity = fUpVelocity;
	KNOCK_BACK.m_fUPDownTime = fUPDownTime;

	m_MovableFactor.GENERAL_STATE.m_bLanding = false;		// Uppered는 무조건 위로 뜬다고 하자. 그래야 모션에서 이벤트 처리가 수월하다.
}

void XModuleMovable::TriggerDrag( const vec3& dir, const vec3& pos, const vec3& target, float fPower, float fKBEventDist )
{
	XMovableFactor::SKnockBack& KNOCK_BACK = m_MovableFactor.KNOCK_BACK;

	// 넉백 재계산, 맞는 위치가 다를 수 있다.
	// 서버의 최종 위치에 맞춘다.
	vec3 vArrivePos = target;
	vec3 vArriveDir = vArrivePos - m_pOwner->GetPosition();

	float fReCalPower = vArriveDir.Length();
	if(fReCalPower < 0.0f)
		fReCalPower = 0.0f;

	// 데이터 셋팅
	KNOCK_BACK.m_vKnockbackDir.Set(vArriveDir);
	KNOCK_BACK.m_vKnockbackDir.Normalize();
	KNOCK_BACK.m_vKnockBackCalcPos.Set(m_pOwner->GetPosition());

	KNOCK_BACK.m_fKBFullPower = fReCalPower;
	KNOCK_BACK.m_fKBPower = fReCalPower;
	KNOCK_BACK.m_fKBVel = DRAG_MAX_VELOCITY;
	KNOCK_BACK.m_fKBFullTime = (fPower / 100.0f) * DRAG_TIME;
	KNOCK_BACK.m_fKBElapsedTime = KNOCK_BACK.m_fKBFullTime;

	KNOCK_BACK.m_bKnockback = true;
	KNOCK_BACK.m_bKBSentEvent = false;
	KNOCK_BACK.m_fKBEventDistance = fKBEventDist;

	m_MovableFactor.INTERPOLATION.m_bWorking = false;

	m_MovableFactor.VELOCITY.m_vVelocity.Set(vec3::ZERO);
}

void XModuleMovable::_PostUpdateReleaseJumpAndFall()
{
	bool bMoveStop = false;
	if ((m_MovableFactor.GENERAL_STATE.m_bColTerrain/* || m_MovableFactor.GENERAL_STATE.m_bColObject*/) &&
		(m_MovableFactor.GENERAL_STATE.m_bJumping || m_MovableFactor.GENERAL_STATE.m_bFallingButNotJump || m_MovableFactor.GENERAL_STATE.m_bLanding == false))
	{
		XEvent msg;

		msg.m_nID = XEVTL_ON_MOTION_EVENT; 
		msg.m_pData = MOTION_EVENT_LANDED;
		m_pOwner->Event(msg);

		m_MovableFactor.GENERAL_STATE.m_fFallingEndHeight = m_MovableFactor.m_vResultPos.z;
		m_MovableFactor.GENERAL_STATE.m_bJumping = false;
		m_MovableFactor.GENERAL_STATE.m_bFallingButNotJump = false;
		m_MovableFactor.GENERAL_STATE.m_bLanding = true;
		m_MovableFactor.GENERAL_STATE.m_bFallingStartRecording = false;

		bMoveStop = true;
	}

	if(m_MovableFactor.GENERAL_STATE.m_bColObject || bMoveStop)
	{
		// 마우스 이동중에 점프했으면... 마우스 이동을 종료하자. 
		if(m_MovableFactor.MOUSE_MOVE.m_bWorking && m_MovableFactor.GENERAL_STATE.m_bJumping)
		{
			m_MovableFactor.MOUSE_MOVE.m_bWorking = false;
			m_MovableFactor.INTERPOLATION.m_bWorking = false;
		}
		else
		{
			m_MovableFactor.VELOCITY.m_vVelocity.x = 0.0f;
			m_MovableFactor.VELOCITY.m_vVelocity.y = 0.0f;
		}
	}
}

void XModuleMovable::TriggerSwim( bool bSwim )
{
	// 수영이라면 수영에 방해되는 부분 초기화
	if(bSwim)
	{
		m_MovableFactor.GENERAL_STATE.m_bJumping			= false;
		m_MovableFactor.GENERAL_STATE.m_bFallingButNotJump	= false;
		m_MovableFactor.GENERAL_STATE.m_bLanding			= true;
	}

	m_MovableFactor.GENERAL_STATE.m_bSwim = bSwim;
}

void XModuleMovable::_PostUpdateFalling()
{
	if (m_MovableFactor.GENERAL_STATE.m_bFallingButNotJumpReserved)
	{
		m_MovableFactor.GENERAL_STATE.m_bFallingButNotJump = true;
		m_MovableFactor.GENERAL_STATE.m_bFallingButNotJumpReserved = false;
	}
}

void XModuleMovable::_PostUpdateThrow()
{
	XMovableFactor::SGrapple& GRAPPLE = m_MovableFactor.GRAPPLE;

	if ( GRAPPLE.m_bGrappled == false)
		return;


	// to do...
}

void XModuleMovable::_PostUpdateSwim()
{
	// 죽은경우는 수영처리를 하면 안된다.
	if(m_MovableFactor.GENERAL_STATE.m_bDie)
		return;

	// 수영 시작
	if(m_MovableFactor.GENERAL_STATE.m_bSwimStart)
	{
		m_MovableFactor.GENERAL_STATE.m_bSwimStart			= false;
		
		// 물에 뛰어들때 떨어지거나 점프로 떨어질때에 대한 처리
		float fFallHeight = m_MovableFactor.GENERAL_STATE.m_fFallingStartHeight - (m_MovableFactor.m_vResultPos.z + SWIMMING_HEIGHT_VALUE) ;
		if(fFallHeight >= m_pModuleCollision->GetRadius() &&		// 자신의 Radius 값보다 크게 떨어져야 한다.
			(m_MovableFactor.GENERAL_STATE.m_bJumping || m_MovableFactor.GENERAL_STATE.m_bFallingButNotJump || m_MovableFactor.GENERAL_STATE.m_bLanding == false))
			m_MovableFactor.GENERAL_STATE.m_bFallingSwimStart = true;

		m_MovableFactor.GENERAL_STATE.m_bJumping			= false;
		m_MovableFactor.GENERAL_STATE.m_bFallingButNotJump	= false;
		m_MovableFactor.GENERAL_STATE.m_bLanding			= true;
		m_MovableFactor.GENERAL_STATE.m_bFallingStartRecording = false;

		m_MovableFactor.GENERAL_STATE.m_bSwim				= true;

		XEvent evt;
		evt.m_nID = XEVTL_SWIM_IN;
		evt.m_pData = (void*)&m_MovableFactor.GENERAL_STATE.m_bFallingSwimStart;
		m_pOwner->Event(evt);
	}
	// 수영 끝
	else if(m_MovableFactor.GENERAL_STATE.m_bSwimEnd)
	{
		XEvent evt;
		evt.m_nID = XEVTL_SWIM_OUT;
		m_pOwner->Event(evt);

		m_MovableFactor.GENERAL_STATE.m_bSwimEnd			= false;
		m_MovableFactor.GENERAL_STATE.m_bSwim				= false;
		m_MovableFactor.GENERAL_STATE.m_bFallingSwimStart	= false;
		m_MovableFactor.GENERAL_STATE.m_bFallingStartRecording = false;
	}
}

void XModuleMovable::_PostUpdateInterpolation()
{
	if(m_MovableFactor.INTERPOLATION.m_bArrived && 
		m_MovableFactor.INTERPOLATION.m_bFinishMoveMsg)
	{
		XEvent evt;
		evt.m_nID = XEVTL_INTERPOLATION_MOVE_ARRIVED;
		m_pOwner->Event(evt);

		m_MovableFactor.INTERPOLATION.m_bFinishMoveMsg = false;
	}

	//// 더미 이동 처리
	//if ( m_MovableFactor.MOVING_DUMMY.m_bHaveDummy == true)
	//{
	//	// 더미가 이동한 만큼 타겟도 이동시켜준다
	//	m_MovableFactor.INTERPOLATION.m_vTargetPos = m_MovableFactor.INTERPOLATION.m_vTargetPos + m_MovableFactor.MOVING_DUMMY.m_vDummyPosElipsed;
	//}
}

void XModuleMovable::_PostUpdateHoldExpception( float fDelta )
{
	// 지형에 끼었을때 처리
	// 낙하상태이거나 슬라이딩 상태일때...
	if(m_MovableFactor.GENERAL_STATE.m_bSliding &&
		(m_MovableFactor.GENERAL_STATE.m_bJumping || 
		m_MovableFactor.GENERAL_STATE.m_bFallingButNotJump || 
		m_MovableFactor.GENERAL_STATE.m_bLanding == false))
	{
		vec3 vCurrent = m_pOwner->GetPosition();

		// 좌표 변화가 없는가?
		if(vCurrent == m_MovableFactor.m_vResultPos)
		{
			m_MovableFactor.GENERAL_STATE.m_fHoldExceptionTime += fDelta;
			m_MovableFactor.GENERAL_STATE.m_nHoldExceptionFrame++;

			// 1. 일정 시간 동안 변화가 없다면... 
			// 2. 일정 프레임이 지나면... 
			// 예외처리한다.
			if(m_MovableFactor.GENERAL_STATE.m_fHoldExceptionTime >= PROCESS_HOLD_EXCEPTION_TIME &&
				m_MovableFactor.GENERAL_STATE.m_nHoldExceptionFrame >= PROCESS_HOLD_EXCEPTION_FRAME)
			{
				m_MovableFactor.GENERAL_STATE.m_bHoldException = true;

				XEvent evt;
				evt.m_nID = XEVTL_EXPCEPTION_HOLD;
				GetOwner()->Event(evt);
			}

			return;
		}
	}

	m_MovableFactor.GENERAL_STATE.m_nHoldExceptionFrame = 0;
	m_MovableFactor.GENERAL_STATE.m_fHoldExceptionTime	= 0.0f;
	m_MovableFactor.GENERAL_STATE.m_bHoldException		= false;
}

void XModuleMovable::Warp( const vec3& vPos, const vec3& vDir )
{
	// 워프일때 Movable에서 필요한 것을 여기서 한다.

	// Movable에서는 다음 보간을 위해서 Interpolation에 있는 위치와 방향값을 새로 넣는다.
	m_MovableFactor.INTERPOLATION.m_vTargetPos = vPos;
	m_MovableFactor.INTERPOLATION.m_vTargetDir = vDir;
}

void XModuleMovable::_PostUpdateKnockback()
{
	XMovableFactor::SKnockBack& KNOCK_BACK = m_MovableFactor.KNOCK_BACK;

	if(KNOCK_BACK.m_bKnockback)
	{
		XActor * pOwnerActor = static_cast<XActor*>(m_pOwner);

		// 죽었고 멈춰있다면... 넉백끝으로 처리한다.
		if(/*pOwnerActor->IsDead() && */KNOCK_BACK.m_fKBPower <= 0.0f)
		{
			// 구르기가 끝났다.
			// 넉백 끝
			m_MovableFactor.KNOCK_BACK.m_bKnockback = false;
			m_MovableFactor.KNOCK_BACK.m_vKnockBackCalcPos.Set(vec3::ZERO);
			ForceStop();
		}
	}
}

void XModuleMovable::UpdateShove( vec3& vTo, float fDelta )
{
	// 밀기에서 내 캐릭터에 대해 필요한 업데이트 작성
	// run processor
	const vec3 vCurrentPos( m_pModuleEntity->GetPosition() );
	const vec3 vCurrentDir( m_pModuleEntity->GetDirection() );

	vTo += m_MovableFactor.VELOCITY.m_vAccel * fDelta * fDelta;
	vec3 vDir = vCurrentDir;
	vec3 vUpVec = vec3::AXISZ;

	//if(m_pModuleCollision->GetOwner()->GetUID() == XGetMyUID())
	//{
	//	mlog("밀기시작 %f %f %f\n", vTo.x, vTo.y, vTo.z);
	//}
#define SHOVE_MOVABLE_PROCESSOR_COUNT 4
	int nMovableProcessorIndex[SHOVE_MOVABLE_PROCESSOR_COUNT] = {E_MOVABLE_PROCESSOR_THROW, E_MOVABLE_PROCESSOR_OBJECT, E_MOVABLE_PROCESSOR_TERRAIN, E_MOVABLE_PROCESSOR_WATER};
	XMovableFactor MovableFactor = m_MovableFactor;
	MovableFactor.GENERAL_STATE.m_bTalentActive = false;

	for (int i = 0; i < SHOVE_MOVABLE_PROCESSOR_COUNT; ++i)
	{
		XMovableProcessor* pMovableProcessor = m_MovableProcessors[nMovableProcessorIndex[i]];

		// 디버깅 정보
		_ASSERT(pMovableProcessor != NULL);
#ifdef _DEBUG_SHOW_MOVABLE_PROC_PASSEED
		bPassed.push_back(pMovableProcessor->IsProcessable(MovableFactor));
#endif

		if (pMovableProcessor->IsProcessable(MovableFactor) == true)
			pMovableProcessor->Run(MovableFactor, vCurrentPos, vCurrentDir, vTo, vDir, vUpVec, fDelta);
	}

	//mlog("헉 %f\n", vTo.Length());
	//_RunMovableProcessor(fDelta);
	//mlog("밀기 끝\n");

	m_pModuleEntity->SetPosition(vCurrentPos + vTo, this);
	m_pModuleEntity->SetDirection(vDir, this);
	m_pModuleEntity->SetUpVector(vUpVec, this);
}

void XModuleMovable::DebugCheckRunProc( XMovableProcessor* pMovableProcessor, float fDelta, vec3 vCurrentPos, vec3 vTo )
{
#ifdef _PUBLISH
	return;
#endif

	if (XMovableDebugReporter::GetInstancePtr() && XMovableDebugReporter::GetInstance().CheckReportTime())
	{
		MUID uidObject = m_pOwner->GetUID();
		XMovableDebugReporter::GetInstancePtr()->CheckProc(uidObject,
														   pMovableProcessor->GetDebugName(), 
														   fDelta, 
														   vCurrentPos, 
														   vTo);
	}
}

void XModuleMovable::_PostUpdateDie()
{
	if(m_MovableFactor.KNOCK_BACK.m_bKnockback == false &&
		m_MovableFactor.GENERAL_STATE.m_bJumping == false &&
		m_MovableFactor.GENERAL_STATE.m_bFallingButNotJump == false &&
		m_MovableFactor.GRAPPLE.m_bGrappled == false &&
		m_MovableFactor.GENERAL_STATE.m_bDie == true)
	{
		// 움직이지 않는다.
		vec3 vOut;
		m_pModuleCollision->CheckXYTerrainCollision(m_pOwner->GetPosition(), vec3(0,0,0), vOut);
		m_MovableFactor.m_vResultPos.Set( vOut );
	}
}

void XModuleMovable::InitMovableFactorFalling()
{
	m_MovableFactor.GENERAL_STATE.m_fFallingStartHeight = 0.0f;
	m_MovableFactor.GENERAL_STATE.m_fFallingEndHeight = 0.0f;
}

// TODO : 리소스 밸리데이터로 옮기기
bool XModuleMovable::CheckUsableDummyLoc()
{
	// 클라이언트에서 탤런트 사용일 때 가능
	if(m_MovableFactor.IsTalentActive() == true)
		return true;

	wstring strCurMotion;
	// 방어가 성공시... dummy_loc가 있으면 한다.
	bool bUsableDummyLocMotion = false;
	XModuleMotion* pModuleMotion = m_pOwner->GetModuleMotion();
	if(pModuleMotion)
	{
		strCurMotion = pModuleMotion->GetCurrMotion();
		if( strCurMotion == MOTION_NAME_SHIELD_GUARD			||
			strCurMotion == MOTION_NAME_SHIELD_GUARD_BLOCK		||
			strCurMotion == MOTION_NAME_SHIELD_GUARD_KNOCKBACK	||
			strCurMotion == MOTION_NAME_GUARD					||
			strCurMotion == MOTION_NAME_GUARD_BLOCK				||
			strCurMotion == MOTION_NAME_MF_BEATEN_BACK			||
			strCurMotion == MOTION_NAME_GUARD_KNOCKBACK)
			return true;
	}

	mlog("%s모션에 필요없는 loc가 존재합니다.\n", MLocale::ConvUTF16ToAnsi(strCurMotion.c_str()).c_str());

	return false;
}

void XModuleMovable::_PostUpdateMouseMove( float fDelta )
{
	// 플레이어 마우스 이동이라면...
	if(m_MovableFactor.MOUSE_MOVE.m_bWorking &&
		(m_MovableFactor.GENERAL_STATE.m_bColTerrain_Front || m_MovableFactor.GENERAL_STATE.m_bColObject))
	{
		if(m_MovableFactor.MOUSE_MOVE.m_fColFrontCheckTime >= 0.0f)
			m_MovableFactor.MOUSE_MOVE.m_fColFrontCheckTime += fDelta;

		if(m_MovableFactor.MOUSE_MOVE.m_fColFrontCheckTime >= 0.5f)
			m_MovableFactor.MOUSE_MOVE.m_bColEnd = true;
	}
	else
	{
		m_MovableFactor.MOUSE_MOVE.m_fColFrontCheckTime = 0.0f;
	}

	if(m_MovableFactor.MOUSE_MOVE.m_bWorking == false)
	{
		m_MovableFactor.MOUSE_MOVE.m_fColFrontCheckTime = 0.0f;
		m_MovableFactor.MOUSE_MOVE.m_bColEnd = false;
	}
}

void XModuleMovable::SetInitialFactors( bool bUsingGravity )
{
	m_MovableFactor.VELOCITY.m_bUsingGravity = bUsingGravity;
}

void XModuleMovable::TriggerRotate( XMovableGoParam* pParam )
{
	if(pParam->m_nType == XMovableGoParam::E_GO_INTERPOLATION)
	{
		m_MovableFactor.INTERPOLATION.SetRoate(pParam->m_vTargetDir, pParam->m_fRotateSpeed);
	}
}

void XModuleMovable::TriggerGrappledBeThrown()
{
	XObject* pAttacker = gg.omgr->Find( m_MovableFactor.GRAPPLE.m_uidGrappledAttacker);
	if ( pAttacker == NULL)
	{
		_ASSERT( 0);
		return;
	}

	RMatrix mat;
	if (NULL == pAttacker->GetActor()->GetAnimatedActorNodeTransform( GRAPPLED_NPC_THROW_BONE_NAME, mat, E_TS_WORLD))
	{
		_ASSERT( 0);
		return;
	}

	m_MovableFactor.GRAPPLE.m_vGrappledThrownStartPos.Set(mat._41, mat._42, mat._43);
	m_MovableFactor.GRAPPLE.m_vGrappledThrownStartDir = Normalize( vec3(-mat._21, -mat._22, -mat._23) );
	m_MovableFactor.GRAPPLE.m_vGrappledVelocity *= m_MovableFactor.GRAPPLE.m_vGrappledThrownStartDir;

	m_MovableFactor.GENERAL_STATE.m_bColTerrain = false;
	m_MovableFactor.GRAPPLE.m_bGrappled = true;
	m_MovableFactor.GRAPPLE.m_bGrappledBeThrown = true;
	m_MovableFactor.GRAPPLE.m_fGrappledElapsed = 0.0f;


	// 박히게 하는 잡기인가?
	float fTerrainZ = gg.currentgamestate->GetWorld()->GetGroundZ( m_MovableFactor.GRAPPLE.m_vGrappledThrownStartPos);

	vec3 vRight = vec3::AXISX;
	if(mat._43 - fTerrainZ <= m_pOwner->GetRadius() + 10.0f)
	{
		// 박힌거면... 지형충돌된걸루 처리하고...
		m_MovableFactor.GRAPPLE.m_bGrappledStuck = true;
		m_MovableFactor.GENERAL_STATE.m_bColTerrain = true;

		m_MovableFactor.GRAPPLE.m_vGrappledThrownStartPos.Set(mat._41, mat._42, fTerrainZ);
		vRight = CrossProduct(vec3(mat._21, mat._22, mat._23), vec3(mat._31, mat._32, mat._33));
		m_MovableFactor.GRAPPLE.m_vGrappledEndDir = CrossProduct(vec3::AXISZ, vRight);
	}

	//mlog("\n던진 방향 %f %f %f\n", m_MovableFactor.GRAPPLE.m_vGrappledEndDir.x, m_MovableFactor.GRAPPLE.m_vGrappledEndDir.y, m_MovableFactor.GRAPPLE.m_vGrappledEndDir.z);
}
