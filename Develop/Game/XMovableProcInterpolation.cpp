#include "stdafx.h"
#include "XMovableProcInterpolation.h"
#include "XModuleMovable.h"

#define BOOST_SPEED_MOVE		800.0f
#define BOOST_SPEED_ROTATE		300.0f



XMovableProcInterpolation::XMovableProcInterpolation()
{
}


XMovableProcInterpolation::~XMovableProcInterpolation()
{
}


void XMovableProcInterpolation::Init( XModuleMovable* pProcOwner )
{
	m_pModuleMovable = pProcOwner;
}


bool XMovableProcInterpolation::IsProcessable( const XMovableFactor& _rMovableFactor)
{
	// 잡기실행시 외부에서 보간처리 막는 것을 하지 말고 여기서 하자.
	if (_rMovableFactor.GRAPPLE.m_bGrappled == true)
	{
		return false;
	}

	return _rMovableFactor.INTERPOLATION.m_bWorking;
}


bool _CalcPos( vec3* ret_pos, const vec3& cur_pos, const vec3& tar_pos, float fSpeed, float fDelta)
{
	// 2008-10-27 billcap 수정
	vec3 diff = tar_pos - cur_pos;
	vec3 dir = diff;
	XMath::NormalizeZ0( dir);

	if ( fSpeed < 1.0f)			// fSpeed = 0
	{
		*ret_pos = diff;
		return true;
	}


	double distance = (double)( diff.x * diff.x + diff.y * diff.y);
	vec3 vToler = dir * ( fSpeed * fDelta);

	if ( (vToler.x * vToler.x + vToler.y * vToler.y) >= distance)
	{
		*ret_pos = diff;
		return true;
	}


	ret_pos->x = vToler.x;
	ret_pos->y = vToler.y;
	ret_pos->z = vToler.z;


	return false;
}

bool _CalcPos_Direct( vec3* ret_pos, const vec3& cur_pos, const vec3& tar_pos, float fSpeed, float fDelta)
{
	// 2008-10-27 billcap 수정
	vec3 diff = tar_pos - cur_pos;
	vec3 dir = diff;
	dir.Normalize();

	if ( fSpeed < 1.0f)			// fSpeed = 0
	{
		*ret_pos = diff;
		return true;
	}


	double distance = (double)( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
	vec3 vToler = dir * ( fSpeed * fDelta);

	if ( (vToler.x * vToler.x + vToler.y * vToler.y+ vToler.z * vToler.z) >= distance)
	{
		*ret_pos = diff;
		return true;
	}


	ret_pos->x = vToler.x;
	ret_pos->y = vToler.y;
	ret_pos->z = vToler.z;


	return false;
}

bool _CalcDir( vec3* ret_dir, const vec3& cur_dir, const vec3& tar_dir, float fSpeed, float fDelta)
{
	if ( fSpeed < 1.0f)		// fSpeed = 0
	{
		ret_dir->x = tar_dir.x;
		ret_dir->y = tar_dir.y;
		XMath::NormalizeZ0( *ret_dir);

		return true;
	}


	float fAngleSpeed = XMath::DegreeToRadian( fSpeed);
	vec3 vCross = cur_dir.CrossProduct( vec3::AXISZ);
	float fDot = vCross.DotProduct( tar_dir);

	if ( fDot > 0.0f)
		fAngleSpeed = -fAngleSpeed;


	float fRotateRadian = fAngleSpeed * fDelta;
	float fCurToTarRadian = cur_dir.AngleToXY( tar_dir);

	if ( abs( fRotateRadian) > abs( fCurToTarRadian))
	{
		ret_dir->x = tar_dir.x;
		ret_dir->y = tar_dir.y;
		XMath::NormalizeZ0( *ret_dir);

		return true;
	}

	ret_dir->x = cur_dir.x * cos( fRotateRadian) - cur_dir.y * sin( fRotateRadian);
	ret_dir->y = cur_dir.y * cos( fRotateRadian) + cur_dir.x * sin( fRotateRadian);
	XMath::NormalizeZ0( *ret_dir);

	return false;
}


bool _CalcRotate( float& ret_rot, const vec3& cur_dir,  const vec3& tar_dir, float fSpeed, float fDelta)
{
	if ( fSpeed < 1.0f)		// fSpeed = 0
	{
		ret_rot = cur_dir.AngleToXY( tar_dir);
		return true;
	}

	float fCurToTarRadian = cur_dir.AngleToXY( tar_dir);
	float fAngleSpeed = XMath::DegreeToRadian( fSpeed);
	if ( fCurToTarRadian < 0.0f)
		fAngleSpeed = -fAngleSpeed;
	float fRotateRadian = fAngleSpeed * fDelta;

	if ( abs( fRotateRadian) > abs( fCurToTarRadian))
	{
		ret_rot = fCurToTarRadian;
		return true;
	}

	ret_rot = fRotateRadian;
	return false;
}


vec3 _Rotate( float fRot, const vec3& vec)
{
	vec3 ret;

	ret.x = vec.x * cos( fRot) - vec.y * sin( fRot);
	ret.y = vec.y * cos( fRot) + vec.x * sin( fRot);
	XMath::NormalizeZ0( ret);

	return ret;
}


void XMovableProcInterpolation::Run( XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos, const vec3& _rVecCurrentDir, vec3& _rVecTo, vec3& _rVecDir, vec3& _rUpVec, float fDelta )
{
	PFC_THISFUNC;

	XMovableFactor::SInterpolation& INTERPOL = _rMovableFactor.INTERPOLATION;
	XMovableFactor::SMovingDummy& MOVING_DUMMY = _rMovableFactor.MOVING_DUMMY;


	// 경과시간 업데이트
	if ( INTERPOL.m_fLimitTime > NO_LIMIT_TIME)
		INTERPOL.m_fElipsedTime += fDelta;


	// 업데이트
	INTERPOL.m_bUpdated = true;


	// TODO : 점프 / 폴링등은 자체처리하지 않고 시뮬레이션 시켜준다. 나중에 점프와 관련된 코드 다시볼것! ( 여기 이 부분이 빠질수 있도록.. )
	if (_rMovableFactor.IsJumping() || _rMovableFactor.IsFalling())
	{
		INTERPOL.m_vStartPos.z = INTERPOL.m_vTargetPos.z = _rVecCurrentPos.z + ((_rVecTo.z < 0.0f) ? _rVecTo.z : 0.0f)/*중력만 영향받도록*/;
		INTERPOL.m_vStartDir = INTERPOL.m_vTargetDir = _rVecDir = _rVecCurrentDir;
		INTERPOL.m_bFinishMove = false;
		INTERPOL.m_bFinishRotate = false;

		if(_rMovableFactor.IsJumping() && _rMovableFactor.GENERAL_STATE.m_bColObject)
		{
			INTERPOL.m_bWorking = false;
			INTERPOL.m_bArrived = true;
			return;
		}
	}

	// 예외 상황일 경우엔 그냥 리턴
	else if ( INTERPOL.m_bWorking == false  ||  _rMovableFactor.IsInterpolationIgnoringState())
	{
		INTERPOL.m_vStartPos = INTERPOL.m_vTargetPos = _rVecCurrentPos;
		INTERPOL.m_vStartDir = INTERPOL.m_vTargetDir = _rVecDir = _rVecCurrentDir;
		INTERPOL.m_bFinishMove = false;
		INTERPOL.m_bFinishRotate = false;
		INTERPOL.m_fLimitTime = NO_LIMIT_TIME;

		_rMovableFactor.VELOCITY.m_vVelocity.x = 0.0f;
		_rMovableFactor.VELOCITY.m_vVelocity.y = 0.0f;

		return;
	}


	// 현재 이동/회전 데이터 생성
	vec3 _cpos = _rVecCurrentPos;
	vec3 _cdir = _rVecCurrentDir;
	XMath::NormalizeZ0( _cdir);


	// 타겟 이동/회전 데이터 생성
	vec3 _tpos = INTERPOL.m_vTargetPos;
	vec3 _tdir = INTERPOL.m_vTargetDir;
	XMath::NormalizeZ0( _tdir);


	// 이동/회전 속도 설정
	float _move_speed	= ( INTERPOL.m_nBoostFlags & BOOST_MOVE)	?  BOOST_SPEED_MOVE : INTERPOL.m_fMoveSpeed;
	float _rotate_speed	= ( INTERPOL.m_nBoostFlags & BOOST_ROTATE)	?  BOOST_SPEED_ROTATE : INTERPOL.m_fRotateSpeed;

	// 더미 이동 처리
	if ( MOVING_DUMMY.m_bHaveDummy == true)
	{
		// 더미가 이동한 만큼 오브젝트 위치도 이동시켜준다
		_rVecTo += MOVING_DUMMY.m_vDummyPosElipsed;

		//if(m_pModuleMovable->GetOwner()->GetType() == XOBJ_NET_PLAYER)
		//	mlog("< 더미 이동 > %f %f\n", MOVING_DUMMY.m_vDummyPosElipsed.x, MOVING_DUMMY.m_vDummyPosElipsed.y);
		if(INTERPOL.m_bArrived == false)
		{
		//	//// 더미가 이동한 만큼 타겟도 이동시켜준다
			//INTERPOL.m_vTargetPos += MOVING_DUMMY.m_vDummyPosElipsed;
			//_tpos = INTERPOL.m_vTargetPos;
			_cpos = INTERPOL.m_vStartPosForDummyMove + INTERPOL.m_vMovePos;
			INTERPOL.m_fLimitTime = NO_LIMIT_TIME;

			//if(m_pModuleMovable->GetOwner()->GetType() == XOBJ_NET_PLAYER)
			//{
			//	mlog("보간 시작 %f %f   보간 이동값 %f %f (목표 %f %f)\n", _cpos.x, _cpos.y, INTERPOL.m_vMovePos.x, INTERPOL.m_vMovePos.y, _tpos.x, _tpos.y);
			//}
		}
	}
	//else
	//{
	//	if(INTERPOL.m_bFinishMove == false && m_pModuleMovable->GetOwner()->GetType() == XOBJ_NET_PLAYER)
	//	{
	//		mlog("-- 그냥 보간 시작 %f %f   (목표 %f %f)\n", _cpos.x, _cpos.y, _tpos.x, _tpos.y);
	//	}
	//}

	// 이동 설정
	if ( INTERPOL.m_bFinishMove == false)
	{
		vec3 _diff_pos;
		if(INTERPOL.m_eMoveCheckType == MCT2_DIRECT)
		{
			INTERPOL.m_bFinishMove = _CalcPos_Direct( &_diff_pos, _cpos, _tpos, _move_speed, fDelta);
			_rVecTo = _diff_pos;
		}
		else
		{
			INTERPOL.m_bFinishMove = _CalcPos( &_diff_pos, _cpos, _tpos, _move_speed, fDelta);

			if(INTERPOL.m_eMoveCheckType == MCT2_MOUSE_MOVE)
				_rVecTo += vec3(_diff_pos.x, _diff_pos.y, 0.0f);
			else
			{
				_rVecTo += _diff_pos;
				INTERPOL.m_vMovePos += _diff_pos;
			}
		}
	


		// 목적지 도착시 설정
		if ( INTERPOL.m_bFinishMove == true)
		{
			// 제한시간 해제
			INTERPOL.m_fLimitTime = NO_LIMIT_TIME;


			// 도착 완료
			INTERPOL.m_bArrived = true;

			INTERPOL.m_bFinishMoveMsg = true;

			INTERPOL.m_vMovePos = vec3::ZERO;
		}

	}


	// 회전 설정
	if ( INTERPOL.m_bFinishRotate == false)
	{
		INTERPOL.m_bFinishRotate = _CalcDir( &_rVecDir, _cdir, _tdir, _rotate_speed, fDelta);
	}

	// 더미 회전 처리
	if ( MOVING_DUMMY.m_bHaveDummy == true)
	{
		// 더미가 회전한 만큼 오브젝트 방향도 회전시켜준다
		_rVecDir = _Rotate( MOVING_DUMMY.m_vDummyDirElipsed, _rVecDir);


		// 더미가 회전한 만큼 타겟도 회전시켜준다
		INTERPOL.m_vTargetDir = _Rotate( MOVING_DUMMY.m_vDummyDirElipsed, INTERPOL.m_vTargetDir);
	}



	// 속도 설정
	if ( INTERPOL.m_bFinishMove == true)
	{
		_rMovableFactor.VELOCITY.m_vVelocity.x = 0.0f;
		_rMovableFactor.VELOCITY.m_vVelocity.y = 0.0f;
	}
}
