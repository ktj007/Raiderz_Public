#pragma once;

#include "XMovableFactor.h"


// Movable Go - Interpolation Go & Direction Go - 와 관련된 상수
#define CONST_MOVABLE_INTERPOLATION_CHANGE_MOTION_MARGIN_SQ_DIST	5.f			/* 이것보다 작으면 모션을 변경하지 않고 보정 */
#define CONST_MOVABLE_INTERPOLATION_IGNORE_SPECIAL_STATE_SQ_SIDT	10000000.f	/* 이것보다 크면 특수상황을 무시하고 무조건 보간 */
#define CONST_MOVABLE_INTERPOLATION_VIRTUAL_TARGET_MIN_SQ_DIST		0.5f		/* 이것보다 작으면 가상타겟을 수신한 좌표로 설정 */
#define CONST_MOVABLE_INTERPOLATION_VIRTUAL_TARGET_MAX_SQ_DIST		1000000.f	/* 이것보다 크면 가상타겟을 수신한 좌표로 설정, 10미터 */
#define CONST_MOVABLE_INTERPOLATION_VIRTUAL_TARGET_DETERM_FACTOR	500.f		/* 이 값이 클수록 먼곳에 가상타겟 설정, 5미터 앞 */
#define CONST_MOVABLE_INTERPOLATION_DEFAULT_ROTATION_SPEED			200.f		/* OnRotaion 콜백을 받을때 기본 회전 속도 */
#define CONST_MOVABLE_INTERPOLATION_IGNORE_MOVE_ON_TALENT_SQ_DIST	40000.f		/* 탈랜트 사용시 짧을 거리를 움직이면 무시한다. 2미터 */


// TriggerGo 을 제어하기 위한 파라미터
class XMovableGoParam : public MMemPool< XMovableGoParam >
{
public:
	enum E_GO_TYPE
	{
		E_GO_DIRECTION = 0,
		E_GO_INTERPOLATION,
		E_GO_MOUSE_MOVE
	};


	E_GO_TYPE			m_nType;

	vec3				m_vTargetPos;
	vec3				m_vTargetDir;

	float				m_fMoveSpeed;
	float				m_fRotateSpeed;
	unsigned int		m_nBoostFlags;

	unsigned int		m_nMovementFlags;
	bool				m_bRunMotion;

	float				m_fLimitTime;

	MOVE_CHECK_TYPE		m_eMoveCheckType;

	union
	{
		bool			m_bKeyPressed;
		bool			m_bForceInterpolation;
	};



public:
	XMovableGoParam(E_GO_TYPE _eGoType)
		: m_nType(_eGoType)
		, m_vTargetPos(vec3::ZERO)
		, m_vTargetDir(vec3::ZERO)
		, m_fMoveSpeed(0.0f)
		, m_fRotateSpeed(DEFAULT_ROTATE_SPEED)
		, m_nBoostFlags( BOOST_NONE)
		, m_nMovementFlags(0)
		, m_fLimitTime( NO_LIMIT_TIME)
		, m_bRunMotion(true)
		, m_bKeyPressed(false)
		, m_eMoveCheckType(MCT2_NONE)
	{
	}

	virtual ~XMovableGoParam()
	{
	}


	inline void			Set( const vec3& vTargetPos, const vec3& vTargetDir, float fMoveSpeed, float fRotateSpeed, unsigned int nBoostFlags, unsigned int nMovementFlags, bool bRunMotion, float fLimitTime =NO_LIMIT_TIME);
	inline void			SetRotate( const vec3& vTargetDir, float fRotateSpeed);

	inline void			SetKeyPress(bool bKeyPressed);
 	inline void			SetGoInterpolationForce(bool bForce);
 	inline void			SetGoInterpolationMotion(unsigned int nMovementFlags, bool bRun);
	inline void			SetMoveCheckType(MOVE_CHECK_TYPE eType) { m_eMoveCheckType = eType; } 

	inline E_GO_TYPE	GetParamType()			{ return m_nType; }

	inline bool			IsKeyPressed()			{ return m_bKeyPressed; }
	inline bool			IsForceInterpolation()	{ return m_bForceInterpolation; }
};



//----------------------------------------------------------------------------------------------------
inline void XMovableGoParam::Set(const vec3& vTargetPos, const vec3& vTargetDir, float fMoveSpeed, float fRotateSpeed, unsigned int nBoostFlags, unsigned int nMovementFlags, bool bRunMotion, float fLimitTime)
{
	m_vTargetPos.Set(vTargetPos);
	m_vTargetDir.Set(vTargetDir);

	m_fMoveSpeed = fMoveSpeed;
	m_fRotateSpeed = fRotateSpeed;
	m_nBoostFlags = nBoostFlags;

	m_nMovementFlags = nMovementFlags;
	m_bRunMotion = bRunMotion;

	m_fLimitTime = fLimitTime;

	m_bForceInterpolation = false;
}
//----------------------------------------------------------------------------------------------------
inline void XMovableGoParam::SetKeyPress(bool bKeyPressed)
{
	_ASSERT( m_nType == E_GO_DIRECTION );
	m_bKeyPressed = bKeyPressed;
}
//----------------------------------------------------------------------------------------------------
inline void XMovableGoParam::SetGoInterpolationForce(bool bForce)
{
	// 무조건 Interpolation 되도록 설정
	m_bForceInterpolation = bForce;
}
//----------------------------------------------------------------------------------------------------
inline void XMovableGoParam::SetGoInterpolationMotion(unsigned int nMovementFlags, bool bRun)
{
	_ASSERT( m_nType == E_GO_INTERPOLATION );

	// 속도가 모두 0이면 Interpolation 의미가 없다.
	// 외부에서 _ASSERT가 발생하지 않도록 SetGoInterpolation을 설정해주어야한다.
	_ASSERT( (m_fMoveSpeed > FLT_EPSILON) || (m_fRotateSpeed > FLT_EPSILON) );

	// 움직임 관련 플래그 설정
	m_nMovementFlags = nMovementFlags;
	m_bRunMotion = bRun;
}

void XMovableGoParam::SetRotate( const vec3& vTargetDir, float fRotateSpeed )
{
	m_vTargetDir.Set(vTargetDir);
	m_fRotateSpeed = fRotateSpeed;
}

//----------------------------------------------------------------------------------------------------
