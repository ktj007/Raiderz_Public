#include "stdafx.h"
#include "XMovableFactor.h"
#include "XMotion.h"


XMovableFactor::SInterpolation::SInterpolation()
: m_bWorking(false)
, m_vStartPos(vec3::ZERO)
, m_vStartDir(-vec3::AXISY)
, m_vTargetPos(vec3::ZERO)
, m_vTargetDir(-vec3::AXISY)
, m_fMoveSpeed(DEFAULT_MOVE_SPEED)
, m_fRotateSpeed(DEFAULT_ROTATE_SPEED)
, m_nBoostFlags(BOOST_NONE)
, m_nMovementFlags(MT_DEFAULT)
, m_bRunMotion(false)
, m_bFinishMove(true)
, m_bFinishRotate(true)
, m_fLimitTime(0.0f)
, m_fElipsedTime(0.0f)
, m_bUpdated(false)
, m_bForceInterpolation(false)
, m_bArrived(false)
, m_bFinishMoveMsg(false)
, m_vMovePos(vec3::ZERO)
, m_vStartPosForDummyMove(vec3::ZERO)
, m_fJumpSpeedRatio(1.0f)
{
}

XMovableFactor::SInterpolation::~SInterpolation()
{
}

void XMovableFactor::SInterpolation::Set( const vec3& vTargetPos, const vec3& vTargetDir, float fMoveSpeed, float fRotateSpeed, unsigned int nBoostFlags, unsigned int nMovementFlags, bool bRunMotion, float fLimitTime, bool bForce, MOVE_CHECK_TYPE eMoveType)
{
	m_bWorking			= true;

	m_vTargetPos		= vTargetPos;
	m_vTargetDir		= vTargetDir;

	m_fMoveSpeed		= fMoveSpeed;
	m_fRotateSpeed		= fRotateSpeed;
	m_nBoostFlags		= nBoostFlags;

	m_nMovementFlags	= nMovementFlags;
	m_bRunMotion		= bRunMotion;

	m_bFinishMove		= false;
	m_bFinishRotate		= false;
	m_bFinishMoveMsg	= false;

	m_fLimitTime		= fLimitTime;
	m_fElipsedTime		= 0.0f;

	m_bUpdated			= false;

	m_bArrived			= false;
	m_bForceInterpolation = bForce;

	m_eMoveCheckType	= eMoveType;

	 m_vMovePos			= vec3::ZERO;
	 m_vStartPosForDummyMove = vec3::ZERO;
}

void XMovableFactor::SInterpolation::SetRoate( const vec3& vTargetDir, float fRotateSpeed )
{
	m_vTargetDir		= vTargetDir;
	m_fRotateSpeed		= fRotateSpeed;
	m_bFinishRotate		= false;
}

bool XMovableFactor::IsMoveMF() const
{
	// 여기서 모션팩터 중에서 이동에 관련된 체크를 여기서 한다.
	if( KNOCK_BACK.m_bKnockback)
	{
		return true;
	}

	return false;
}

bool XMovableFactor::IsInterpolationStopped() const
{
	return INTERPOLATION.m_bFinishMove;
}

bool XMovableFactor::IsInterpolationRunning() const
{
	if ( ! IsInterpolationStopped() && (INTERPOLATION.m_bRunMotion == true))
		return true;

	return false;
}

bool XMovableFactor::IsInterpolationIgnoringState() const
{
	//if ( IsMoveMF() || IsStance() || IsTalentActive() || IsJumping() || IsFalling() || IsStun())
	if ( IsMoveMF() || /*IsStance() || IsMoveDummyNode() ||*/ IsJumping() || IsFalling() || /*IsStun() ||*/ IsGrappled())
		return true;
	return false;
}
