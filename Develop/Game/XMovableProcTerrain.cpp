#include "stdafx.h"
#include "XMovableProcTerrain.h"
#include "XModuleCollision.h"
#include "XModuleMovable.h"
#include "XGame.h"
#include "XMovableProcObjectChecker.h"

// 디버그용
//#include "RDebugPassRenderer.h"
//#include "RCameraSceneNode.h"

void XMovableProcTerrain::Init( XModuleMovable* pProcOwner )
{
	m_pMovableProcOwner = pProcOwner;
	m_pModuleCollision	= pProcOwner->m_pModuleCollision;
	
	m_fFallingStartReadyTime		= 0.0f;
}

bool XMovableProcTerrain::IsProcessable( const XMovableFactor& _rMovableFactor )
{
	if ( _rMovableFactor.GENERAL_STATE.m_bSwim == true ||
		(_rMovableFactor.GRAPPLE.m_bGrappled  &&  !_rMovableFactor.GRAPPLE.m_bGrappledBeThrown)  ||
		gg.game == NULL)
	{
		return false;
	}
	return true;
}

void XMovableProcTerrain::Run( XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos, const vec3& _rVecCurrentDir, vec3& _rVecTo, vec3& _rVecDir, vec3& _rUpVec, float fDelta )
{
	// 잡혀서 전져지는 충돌 검사
	if ( CheckThrowColision( _rMovableFactor, _rVecCurrentPos, _rVecTo, _rVecDir, fDelta) == true)
		return;



	// 맵 충돌
	//
	vec3 vOut;
	vec3 vOutNor;
	vec3 now_pos = _rVecCurrentPos;
	// 충돌체크용 구가 바닥에 닿도록, 바닥에서부터 반지름만큼 위로 올려준다. -_-; 
	// 경사면일 경우 정확히 접하진 않겠지만 대충 넘어감. -_-

	vOut = now_pos;	

	bool bSliding = false;
	bool bFrontCol = false;
	vec3 target = now_pos + _rVecTo;	

	// 가려고 하는것고 새로운 포지션이 같고 점프등의 특수한 상황이 아니라면
	// 이동하지 않는것이므로 다음작업을 안해준다.
	if (now_pos.IsEqual(target) && _rMovableFactor.IsJumping() == false && _rMovableFactor.IsFalling() == false )
	{
		#ifdef _DEBUG_SHOW_TERRAIN_PROC
		mlog("지형 - 타겟과 목표가 같아서 MovableProc하지 않고 그냥 넘김\n");
		#endif
		return;
	}

	_rMovableFactor.GENERAL_STATE.m_bColTerrain = false;
	_rMovableFactor.GENERAL_STATE.m_bSliding = false;
	_rMovableFactor.GENERAL_STATE.m_bColTerrain_Front = false;

	//맵에 닿으면
	if(m_pModuleCollision->GroundMoveTestMap(now_pos, target, vOut, vOutNor, _rMovableFactor.GENERAL_STATE.m_bJumping, &bSliding, &bFrontCol, &_rMovableFactor.GENERAL_STATE.m_vecTerrainColObejct))
	{
		_rMovableFactor.GENERAL_STATE.m_bSliding = bSliding;
		_rMovableFactor.GENERAL_STATE.m_bColTerrain_Front = bFrontCol;

		m_fFallingStartReadyTime = 0.0f;

		// 예외처리 상태라면...
		if(_rMovableFactor.GENERAL_STATE.m_bHoldException)
		{
			_rMovableFactor.GENERAL_STATE.m_bSliding = false;
		}

		float fTerrainDot = vOutNor.DotProduct(vec3::AXISZ);

		if (bSliding && !_rMovableFactor.GENERAL_STATE.m_bFallingButNotJump && !_rMovableFactor.GENERAL_STATE.m_bJumping)
		{
			if (vOut.z > now_pos.z)	// 미끄러졌는데 더 위로 갔다면
			{									// 못가는데를 올라갈려고 한다던가 그런 경우
				_rVecTo.Set(vec3::ZERO);

				_rMovableFactor.GENERAL_STATE.m_bColTerrain_Front = true;
				_rMovableFactor.GENERAL_STATE.m_bColTerrain = true;
				return;	// 그냥 이동취소한다. 부드럽게 미끄러지도록 바꾸면 더욱 좋겠다. (사실 지금 움직임이 이상함)
			}

			if (fTerrainDot < FALLING_LIMIT &&						// 경사가 깊으면 Falling
				vOut.z != now_pos.z)
			{
				FallingSetting(_rMovableFactor, _rVecCurrentPos);
			}
		}

		// 미끄러지는 중엔 바닥에 닿았어도 떨어지거나 점프중이면 그냥 떨어지거나 점프중인 상태로 유지.
		// 내려가다가 안정된 바닥이 없는 개미지옥을 만나면 falling/jumping state에서 못빠져나오는 문제가 있다.
		// 근데 어차피 그런데 빠지면 못기어나올테니 그냥 일단 냅뒀음.-_-;
		else 
		{
			if (!_rMovableFactor.GENERAL_STATE.m_bSliding ||
				(_rMovableFactor.GENERAL_STATE.m_bSliding && (now_pos.z == vOut.z)))
			{
				_rMovableFactor.GENERAL_STATE.m_bColTerrain = true;
				_rMovableFactor.VELOCITY.m_vVelocity.z = 0.0f;
			}
		}


		// 한번 더 오브젝트 충돌 검사를 한다. 뚫을 수 있기 때문이다.
		// 막아야 한다.
		// 대신 비비지 않게 해서 멈추게 한다.
		// 오브젝트 충돌로 지형충돌처럼 쓰이는 부분이 있기 때문에 생긴 부분이다.(예: 오염된 화원)
		// 상대 캐릭터가 특수한 상황중에서는 오브젝트 충돌을 허용한다.
		XMovableProcObjectChecker objectChecker;
		bool bObjectCol = objectChecker.CheckObjectCollision(_rMovableFactor, m_pModuleCollision->GetOwner());

		if(bObjectCol && bFrontCol)
		{
			vector<MUID> vecEmpty;
			float fDist = 0.0f;
			vec3 vImpactPos;
			vec3 vImpactDir;
			vec3 tempVecTo = vOut - _rVecCurrentPos;
			if(m_pModuleCollision->ColObject(tempVecTo, _rVecCurrentPos, vImpactPos, vImpactDir, fDist, vecEmpty, false))
			{
				vImpactDir.z = 0.0f;
				vImpactDir.Normalize();

				vOut = _rVecCurrentPos + tempVecTo + (vImpactDir * fDist);
			}
		}

		//#ifdef _DEBUG
		//	RMatrix mworld;s
		//	mworld.MakeIdentity();
		//	mworld.SetTranslation(vOut);
		//	REngine::GetSceneManager().GetPassRenderer< RDebugPassRenderer >()->AddDebugBox(mworld, 5, 5, D3DCOLOR_XRGB(0, 0, 255));
		//#endif
	}
	else 	//맵에 닿지 않으면.
	{
		vec3 vCurrentMoveTo = vOut - _rVecCurrentPos;		// 바닥에 닿지 않아도 정면에 충돌이 될수 있으므로 vOut을 최종으로 생각하고 다시 이동 방향을 계산한다
		vec3 vXYColout;
		m_pModuleCollision->CheckXYTerrainCollision(_rVecCurrentPos, vCurrentMoveTo, vXYColout);
		float fGroundZ = vXYColout.z;


		// 만약 바닥이 없을 경우 나락으로 떨어지지 않게 한다.
		vec3 vNoGroundTestPos	= _rVecCurrentPos + vec3(vCurrentMoveTo.x, vCurrentMoveTo.y, m_pModuleCollision->GetRadius());
		vec3 vNoGroundTestTarget = vNoGroundTestPos;
		//vNoGroundTestPos		+= vec3(_rVecTo.x, _rVecTo.y, m_pModuleCollision->GetRadius());
		vNoGroundTestTarget.z	-= 50000.0f;
		vec3 vNoGroundTestOut, vNoGroundTestNor;
		if(m_pModuleCollision->PickRayToTerrain(vNoGroundTestPos, vNoGroundTestTarget, vNoGroundTestOut, vNoGroundTestNor) == false)
		{
			vOut.z = fGroundZ;

			_rMovableFactor.GENERAL_STATE.m_bColTerrain = true;
			_rMovableFactor.VELOCITY.m_vVelocity.z = 0.0f;
		}
		else
		{
			// Falling 체크
			float fTestDot = vNoGroundTestNor.DotProduct(vec3::AXISZ);

			if(fGroundZ < vOut.z - 200.f ||			// 2미터.
				(fTestDot < FALLING_LIMIT && 
				_rMovableFactor.GENERAL_STATE.m_bFallingStartRecording == false && 
				CheckAllowedHeight(_rVecCurrentPos, _rVecTo) == false))
			{
				if(CheckFallingStart(fDelta))
					FallingSetting(_rMovableFactor, _rVecCurrentPos);
			}
		}
	}

	// 물에 충돌 검사한다.
	if(CheckTerrainWater(vOut))
	{
		_rMovableFactor.GENERAL_STATE.m_bSwimStart = true;
	}

	_rVecTo = vOut - _rVecCurrentPos;
}


bool XMovableProcTerrain::CheckThrowColision( XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos, vec3& _rVecTo, vec3& _rVecDir, float fDelta)
{
	XMovableFactor::SGrapple& GRAPPLE = _rMovableFactor.GRAPPLE;

	// 잡혀서 날라가는 상태에서 충돌
	if ( GRAPPLE.m_bGrappled == false)	return false;
	if ( GRAPPLE.m_bGrappledBeThrown == false)  return true;
	if ( GRAPPLE.m_bGrappledStuck == true) return false;


	//  속도가 없으면 종료
	if ( GRAPPLE.m_vGrappledVelocity == 0.0f)
		return false;

	_rMovableFactor.GENERAL_STATE.m_bColTerrain = false;

	vec3 vPosCurr = _rVecCurrentPos;
	vPosCurr.z = max( vPosCurr.z,  gg.currentgamestate->GetWorld()->GetGroundZ( vPosCurr) + 5.0f);
	vec3 vMoveTo = _rVecTo;

	while ( 1)
	{
		// 변수 설정
		vec3 vPosSt = vPosCurr;
		vec3 vPosEd = vPosSt + vMoveTo;


		// 충돌면의 법선 벡터 구하기
		vec3 vPosCol, vFaceNormal;
		if ( m_pModuleCollision->PickRayToTerrain( vPosSt, vPosEd, vPosCol, vFaceNormal) == false)
		{
			// 물과 충돌했는지 여부를 구함
			if ( CheckTerrainWater( vPosEd) == true)
			{
				_rMovableFactor.GENERAL_STATE.m_bSwimStart = true;
				return true;
			}

			if(_rMovableFactor.GENERAL_STATE.m_bColTerrain == false &&
				_rMovableFactor.GENERAL_STATE.m_bFallingStartRecording == false)
			{
				FallingSetting(_rMovableFactor, _rVecCurrentPos);
			}
			// 볼륨 형태의 충돌 검사를 다시 실시해서 충돌 위치를 다시 찾도록...

			break;
		}

//#ifdef _DEBUG
//		RMatrix mat;
//		mat.MakeIdentity();
//		mat.SetTranslation( vPosCol);
//		REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugSphere( mat, 5, 120, D3DCOLOR_XRGB(255, 0, 0));
//#endif


		// 충돌 후 반사되는 속도 벡터
		vec3 vReflected( 0.0f, 0.0f, 0.0f);
		vec3 vReflectedVel( 0.0f, 0.0f, 0.0f);
		float fTimeCol = ( ( vPosCol - vPosSt).Length() / vMoveTo.Length()) * fDelta;


		// 충돌 시 속도를 구함
		float fElapsed = GRAPPLE.m_fGrappledElapsed - fDelta + fTimeCol;
		vec3 vVelocity = GRAPPLE.m_vGrappledVelocity;
		vVelocity.z += 0.5f * GRAVITY * fElapsed * fElapsed;
		float fVelocity = vVelocity.Length();


		// 충돌 직전의 속도가 충분한지 검사
		if ( fVelocity > 800.0f)
		{
			// 반사되는 방향 구하기 = ( 2 * (-D dot N) ) N + D
			vReflected = ( 2.0f * DotProduct( -vMoveTo, vFaceNormal) ) * vFaceNormal + vMoveTo;
			vReflected = Normalize( vReflected);


			// 충돌 후 속도 구하기
			vReflectedVel = vReflected * ( fVelocity * 0.4f);
			vReflectedVel += -vFaceNormal * (vReflectedVel * 0.7f);
		}


		// 속도 재설정
		GRAPPLE.m_fGrappledElapsed = fDelta - fTimeCol;
		GRAPPLE.m_vGrappledVelocity = vReflectedVel;
		GRAPPLE.m_vGrappledThrownStartPos = vPosCol;

		// 충돌 플래그 설정
		_rMovableFactor.GENERAL_STATE.m_bColTerrain = true;


		//  속도가 없으면 종료
		if ( GRAPPLE.m_vGrappledVelocity == 0.0f)
		{
			vMoveTo = vPosCol - vPosCurr;
			break;
		}

		// 충돌 후 위치 재설정
		vPosCurr = vPosCol;

		vec3 vNextPos = GRAPPLE.m_vGrappledThrownStartPos + GRAPPLE.m_vGrappledVelocity * GRAPPLE.m_fGrappledElapsed;
		vNextPos.z += 0.5f * GRAVITY * GRAPPLE.m_fGrappledElapsed * GRAPPLE.m_fGrappledElapsed;
		vNextPos.z = max( vNextPos.z ,  gg.currentgamestate->GetWorld()->GetGroundZ( vNextPos) + 5.0f);
		vMoveTo = vNextPos - vPosCurr;

		fDelta -= fTimeCol;


		// 충돌 후 위치 재설정
		GRAPPLE.m_vGrappledThrownStartDir = Normalize( vNextPos - vPosCol);
	}


	// 이동한 거리 설정
	_rVecTo = (vPosCurr + vMoveTo) - _rVecCurrentPos;

	return true;
}

bool XMovableProcTerrain::CheckTerrainWater( vec3& vPos )
{
	//------------------------------------------------------------------------
	// 지형인지 물인지 판단
	vec3 vOutTerrain = vPos;
	vec3 vOut = vPos;

	m_pModuleCollision->CheckXYTerrainCollision(vPos, vec3(0, 0, 0), vOutTerrain);
	bool bColWater = m_pModuleCollision->PickTerrainWater(vPos, vOut);

	if(bColWater == true)
	{
		// 천장이 존재하는가?
		vec3 vUpColTestOut;
		vec3 vUpColTestNor;
		vOutTerrain.z += 1.0f;
		if(m_pModuleCollision->PickRayToTerrain(vOutTerrain, vOut, vUpColTestOut, vUpColTestNor) &&
			vUpColTestOut.z < vOut.z)
		{
			// 존재하면 내 현재 위치의 물이 아니다.
			return false;
		}

		// 물에서 지형으로 변경해도 되는지 조건을 본다.
		//if(GetMyPlayerOwner()->IsSwim() == true &&		// 현재 수영중이며
		//	m_bSwimOutPossible == false)				// XModuleMovable에서 지형으로 나갈수 없는 상황이면...
		//{
		//	return true;								// 수영중이라고 판단.
		//}

		// 물 충돌
		if(vOutTerrain.z + SWIMMING_HEIGHT_VALUE < vOut.z &&
			(vOut.z - SWIMMING_HEIGHT_VALUE) >= vPos.z)
		{
			vPos.z = vOut.z - SWIMMING_HEIGHT_VALUE;
			return true;
		}
	}

	return false;
}

void XMovableProcTerrain::FallingSetting( XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos )
{
	// 점프도 아니고 폴링도 아니면
	// 폴링자세로 폴링 속성 true로.
	if (!_rMovableFactor.GENERAL_STATE.m_bJumping && !_rMovableFactor.GENERAL_STATE.m_bFallingButNotJump)
	{
		if (!_rMovableFactor.GENERAL_STATE.m_bFallingButNotJumpReserved)
		{
			if(_rMovableFactor.GENERAL_STATE.m_bFallingStartRecording == false)
			{
				_rMovableFactor.GENERAL_STATE.m_fFallingStartHeight = _rVecCurrentPos.z;
				_rMovableFactor.GENERAL_STATE.m_bFallingStartRecording = true;
			}
			_rMovableFactor.GENERAL_STATE.m_bFallingButNotJumpReserved = true;
			_rMovableFactor.GENERAL_STATE.m_nFallingButNotJumpStartTime = XGetNowTime();
			// 바로 떨어지는 모션이 아니라 살짝 딜레이를 주기 위해 reserved 어쩌구 변수를 만든다. -_-;
		}
	}
}

bool XMovableProcTerrain::CheckAllowedHeight( const vec3& vEntityPosition, const vec3 &vec )
{
	vec3 vMoveDir = vec;
	XMath::NormalizeZ0(vMoveDir);
	float fRadius = m_pModuleCollision->GetRadius();

	vec3 vStart = vEntityPosition + vec3(0, 0, fRadius) + (vMoveDir * fRadius);
	vec3 vEnd  = vStart + vec3(0, 0, -50000.0f);

	vec3 vTestOut = vStart;
	vec3 vTestNor = vec3::AXISZ;

	if(m_pModuleCollision->PickRayToTerrain(vStart, vEnd, vTestOut, vTestNor))
	{
		float fHeight = vEntityPosition.z - vTestOut.z;
		if(fHeight < MOVE_ALLOWED_HEIGHT)
			return true;
	}

	return false;
}

bool XMovableProcTerrain::CheckFallingStart( float fDelta )
{
	// 일정시간 안에는 Falling 처리를 하지 않는다.
	m_fFallingStartReadyTime += fDelta;

	if(m_fFallingStartReadyTime >= XCONST::COLLISION_TERRAIN_FALLING_START_TIME)
	{
		return true;
	}

	return false;
}
