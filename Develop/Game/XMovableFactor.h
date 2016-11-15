#ifndef _XMOVABLE_FACTOR_H
#define _XMOVABLE_FACTOR_H

const float DEFAULT_MOVE_SPEED		= 100.0f;
const float DEFAULT_ROTATE_SPEED	= 100.0f;


// flags of boost speed
#define BOOST_NONE		0x00
#define BOOST_MOVE		0x01
#define BOOST_ROTATE	0x02

#define NO_LIMIT_TIME	0.0f

enum MOVE_CHECK_TYPE
{
	MCT2_NONE,
	MCT2_TARGET_HEGITH,			// 타겟 높이로 충돌 체크
	MCT2_DIRECT,				// 지형 검사를 하지 않고 허공답보를 합니다.
	MCT2_MOUSE_MOVE
};

class XMovableFactor
{
private:

	friend class XModuleMovable;

	vec3	m_vResultDir;
	vec3	m_vResultPos;
	vec3	m_vResultUpVec;

public:

	XMovableFactor()
		: m_vResultPos(vec3::ZERO)
		, m_vResultDir(vec3::ZERO)
		, m_vResultUpVec(vec3::AXISZ)
	{
	}

	~XMovableFactor()
	{
	}


	//////////////////////////////////////////////////////////////////////////
	// 속도
	//
	struct SVelocity
	{
		SVelocity()
			: m_vAccel(vec3::ZERO)
			, m_vVelocity(vec3::ZERO)
			, m_bUsingAccel(true)
			, m_bUsingGravity(true)
		{
		}
		vec3					m_vVelocity;				// 움직임 속도값
		vec3					m_vAccel;					// 움직임의 가속값
		bool					m_bUsingAccel;				// 가속도 사용여부
		bool					m_bUsingGravity;			// 중력 사용여부

	} VELOCITY;


	//////////////////////////////////////////////////////////////////////////
	// 상태
	//
	struct SState
	{
		SState()
			: m_bFallingButNotJump(false)
			, m_bFallingButNotJumpReserved(false)
			, m_bFallingStartRecording(false)
			, m_nFallingButNotJumpStartTime(0)
			, m_fFallingStartHeight(0.0f)
			, m_fFallingEndHeight(0.0f)

			, m_bJumping(false)
			, m_bJumpAscending(false)
			, m_vJumpPosForCamera(vec3::ZERO)
			, m_fCalJumpHeight(0.0f)

			, m_bLanding(true)

			, m_bColTerrain(false)
			, m_bColObject(false)
			, m_bColTerrain_Front(false)

			, m_bSliding(false)

			, m_bSwimStart(false)
			, m_bFallingSwimStart(false)
			, m_bSwim(false)
			, m_bSwimEnd(false)

			, m_bGoingDirection(false)

			, m_bTalentActive(false)
			, m_nTalentActiveID(-1)

			, m_bStance(false)
			, m_bStun(false)

			, m_bHoldException(false)
			, m_fHoldExceptionTime(0.0f)
			, m_nHoldExceptionFrame(0)

			, m_bDie(false)
			
			, m_bGuardDefense(false)
		{
		}

		// falling
		bool					m_bFallingButNotJump;
		bool					m_bFallingButNotJumpReserved;	// 떨어지는순간 falling이 아니라 잠깐 텀을 둔다.
		bool					m_bFallingStartRecording;
		int						m_nFallingButNotJumpStartTime;
		float					m_fFallingStartHeight;
		float					m_fFallingEndHeight;

		// jump
		bool					m_bJumping;			//점프
		bool					m_bJumpAscending;	//점프시 오르는.
		vec3					m_vJumpPosForCamera;// 카메라를 위한 점프 위치 계산
		float					m_fCalJumpHeight;

		// 착지
		bool					m_bLanding;

		// 충돌
		bool					m_bColTerrain;		// 지형
		bool					m_bColObject;		// 오브젝트
		bool					m_bColTerrain_Front;// 지형 정면
		vector<MUID>			m_vecTerrainColObejct;	// 지형 충돌로 쓰일 오브젝트(npc)

		// 미끄러짐
		bool					m_bSliding;	

		// 수영
		bool					m_bSwimStart;
		bool					m_bFallingSwimStart;
		bool					m_bSwim;
		bool					m_bSwimEnd;

		// 전진(타겟에 의한 인터폴레이션이 아닌 키 입력에 의한 전진)
		bool					m_bGoingDirection;

		// 탈랜트 사용중
		bool					m_bTalentActive;
		int						m_nTalentActiveID;

		// 스탠스
		bool					m_bStance;

		// 스턴
		bool					m_bStun;

		// 끼었을때 예외처리
		bool					m_bHoldException;
		float					m_fHoldExceptionTime;
		int						m_nHoldExceptionFrame;

		// 죽음
		bool					m_bDie;

		// 가드
		bool					m_bGuardDefense;

	} GENERAL_STATE;


	//////////////////////////////////////////////////////////////////////////
	// 이동 보간 관련
	//
	struct SInterpolation
	{
		SInterpolation();
		~SInterpolation();

		bool					m_bWorking;				///< 작동여부

		vec3					m_vStartPos;			///< Position of start
		vec3					m_vStartDir;			///< Direction of start

		vec3					m_vTargetPos;			///< Position of target
		vec3					m_vTargetDir;			///< Direction of target

		vec3					m_vStartPosForDummyMove;///< Dummy에 의한 이동에 필요한 시작값
		vec3					m_vMovePos;				///< 이동한값

		float					m_fMoveSpeed;			///< Speed of movement
		float					m_fRotateSpeed;			///< Speed of rotation
		unsigned int			m_nBoostFlags;			///< Flag of boost speed of movement and rotation
		float					m_fJumpSpeedRatio;		///< 점프 스피드 비율

		unsigned int			m_nMovementFlags;		///< Flags of movement

		bool					m_bRunMotion;			///< Run animation

		bool					m_bFinishMove;			///< Flag of finishing move
		bool					m_bFinishRotate;		///< Flag of finishing rotate
		bool					m_bFinishMoveMsg;		///< 이동 종료 메세지

		float					m_fLimitTime;			///< Maximum time to complete
		float					m_fElipsedTime;			///< ElipsedTime;

		bool					m_bUpdated;				///< 새 데이터가 입력 된 이후에 업데이트가 되었는지 여부
		bool					m_bArrived;				///< 인터폴레이션해서 목표 위치에 도착했을때
		bool					m_bForceInterpolation;	///< 보간을 사용하지 않는 기본정책을 강제로 무시하게 할 경우

		MOVE_CHECK_TYPE			m_eMoveCheckType;		///< 이동 검사 방법


		void					Set( const vec3& vTargetPos, const vec3& vTargetDir, float fMoveSpeed, float fRotateSpeed, unsigned int nBoostFlags, unsigned int nMovementFlags, bool bRunMotion, float fLimitTime, bool bForce, MOVE_CHECK_TYPE eMoveType);
		void					SetRoate(const vec3& vTargetDir, float fRotateSpeed);

	} INTERPOLATION;


	//////////////////////////////////////////////////////////////////////////
	// 자연스러운 움직임을 위한 도움 더미노드
	//
	struct SMovingDummy
	{
		SMovingDummy() :
			m_bHaveDummy( false),
			m_vDummyPosition( vec3::ZERO),
			m_vDummyPosElipsed( vec3::ZERO),
			m_vDummyDirection( -vec3::AXISY),
			m_vDummyDirElipsed( 0.0f)
			{
		}

		bool					m_bHaveDummy;					// 더미 노드를 가지고 있는지 여부

		vec3					m_vDummyPosition;				// 더미노드 위치값
		vec3					m_vDummyPosElipsed;				// 더미노드 위치 변화값
		vec3					m_vDummyDirection;				// 더미노드 방향값
		float					m_vDummyDirElipsed;				// 더미노드 방향 변화값

	} MOVING_DUMMY;


	//////////////////////////////////////////////////////////////////////////
	// KnockBack
	//
	struct SKnockBack
	{
		SKnockBack()
			: m_fKBEventDistance(0.f)
			, m_fKBFullPower(0.0f)
			, m_fKBPower(0.f)
			, m_fKBVel(0.f)
			, m_fKBUpVelocity(0.0f)
			, m_fKBFullTime(0.0f)
			, m_fKBElapsedTime(0.0f)
			, m_vKnockbackDir(vec3::ZERO)
			, m_vKnockBackDelta(vec3::ZERO)
			, m_bKnockback(false)
			, m_bKBSentEvent(false)
			, m_vKnockBackCalcPos(vec3::ZERO)
			, m_fUPDownTime(0.0f)
		{

		}


		float					m_fKBEventDistance;	// 넉백 이벤트가 나와야 하는 남은 거리
		float					m_fKBFullPower;
		float					m_fKBPower;
		float					m_fKBVel;

		float					m_fKBFullTime;
		float					m_fKBElapsedTime;

		float					m_fKBUpVelocity;
		float					m_fUPDownTime;

		vec3					m_vKnockbackDir;
		vec3					m_vKnockBackDelta;	// 넉백 델타
		vec3					m_vKnockBackCalcPos; // 넉백 시작 위치
		bool					m_bKnockback;
		
		bool					m_bKBSentEvent;

	} KNOCK_BACK;


	//////////////////////////////////////////////////////////////////////////
	// 잡기
	//
	struct SGrapple
	{
		SGrapple()
			: m_bGrappled( false)
			, m_uidGrappledAttacker( MUID::ZERO)
			, m_bGrappledAttach(false)
			, m_bGrappledBeThrown( false)
			, m_bGrappledStuck(false)
			, m_vGrappledThrownStartPos( vec3::ZERO)
			, m_vGrappledThrownStartDir( -vec3::AXISY)
			, m_vGrappledVelocity( vec3::ZERO)
			, m_vGrappledStartDir( vec3::AXISY )
			, m_vGrappledStartPos( vec3::ZERO )
			, m_vGrappledEndDir( vec3::AXISY )
			, m_vGrappledReadyPos( vec3::ZERO )
			, m_fGrappledElapsed( 0.0f)
			, m_fGrappledStartInterpolationTime(0.0f)
			, m_fGrappledRemainStartInterpolationTime(0.0f)
		{
		} 

		bool					m_bGrappled;				// 잡힌 상태 여부
		MUID					m_uidGrappledAttacker;		// 나를 잡은 상대 캐릭터 UID
		bool					m_bGrappledAttach;			// 잡혀진 상태
		bool					m_bGrappledBeThrown;		// 잡혀서 던져진 상태 여부
		bool					m_bGrappledStuck;			// 잡혀서 날아가지 않고 박히는 잡기이다.
		vec3					m_vGrappledThrownStartPos;	// 날아갈 시작하는 위치
		vec3					m_vGrappledThrownStartDir;	// 날아갈 시작하는 방향
		vec3					m_vGrappledVelocity;		// 날아갈 때 속도 벡터
		vec3					m_vGrappledStartDir;		// 잡기 시작한 방향
		vec3					m_vGrappledStartPos;		// 잡기 시작한 위치
		vec3					m_vGrappledEndDir;			// 잡기 끝나는 방향
		vec3					m_vGrappledReadyPos;		// 잡기 준비 위치
		float					m_fGrappledElapsed;			// 날아가는 시간
		float					m_fGrappledStartInterpolationTime; // 시작시 보간처리 할 시간...
		float					m_fGrappledRemainStartInterpolationTime; // 시작시 보간처리 할 시간...

	} GRAPPLE;

	//////////////////////////////////////////////////////////////////////////
	// 마우스 이동
	//
	struct SMouseMove
	{
		SMouseMove()
			: m_bWorking(false)
			, m_bColEnd(false)
			, m_fColFrontCheckTime(0.0f)
		{
		}

		float					m_fColFrontCheckTime;

		bool					m_bWorking;				///< 작동여부
		bool					m_bColEnd;				///< 충돌로 종료

	} MOUSE_MOVE;

public:
	
	//////////////////////////////////////////////////////////////////////////
	//
	// 움직임과 관련된 상태 쿼리(질의) 함수
	//
	//////////////////////////////////////////////////////////////////////////

	bool					IsMoveMF() const;
	bool					IsJumping()	const		{ return GENERAL_STATE.m_bJumping; }
	bool					IsFalling()	const		{ return GENERAL_STATE.m_bFallingButNotJump; }
	bool					IsSliding()	const		{ return GENERAL_STATE.m_bSliding; }
	bool					IsGoingDirection() const{ return GENERAL_STATE.m_bGoingDirection; }

	bool					IsTalentActive() const	{ return GENERAL_STATE.m_bTalentActive; }
	bool					IsStance() const		{ return GENERAL_STATE.m_bStance; }
	bool					IsStun() const			{ return GENERAL_STATE.m_bStun; }

	bool					IsInterpolationIgnoringState() const;
	bool					IsInterpolationStopped()	const;
	bool					IsInterpolationRunning()	const;

	bool					IsMoveDummyNode() const	{ return MOVING_DUMMY.m_bHaveDummy; }

	bool					IsGrappled() const { return GRAPPLE.m_bGrappled;}

	bool					IsGuardDefense() const	{ return GENERAL_STATE.m_bGuardDefense; }
};


#endif // _XMOVABLE_FACTOR_H