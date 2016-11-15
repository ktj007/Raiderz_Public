#include "stdafx.h"
#include "XMovableProcThrow.h"
#include "XModuleCollision.h"
#include "XModuleEntity.h"
#include "XModuleMovable.h"
#include "XModuleTalent.h"
#include "XModuleMotion.h"


#ifdef _DEBUG
//#define GRAPLLED_DEBUG_LINE
//#include "RDebugPassRenderer.h"
#endif

XMovableProcThrow::XMovableProcThrow()
{
	m_bGrip = false;
	m_bGripStart = false;
}


XMovableProcThrow::~XMovableProcThrow()
{

}


void XMovableProcThrow::Init( XModuleMovable* pProcOwner)
{
	m_pMovableProcOwner	= pProcOwner;
	m_pModuleCollision	= pProcOwner->m_pModuleCollision;

	_ASSERT( m_pModuleCollision);
}


void XMovableProcThrow::Fini()
{

}


bool XMovableProcThrow::IsProcessable( const XMovableFactor& _rMovableFactor)
{
	if ( _rMovableFactor.GRAPPLE.m_bGrappled == false)
		return false;

	return true;
}


void XMovableProcThrow::Run( XMovableFactor& rMovFactor, const vec3& vPosCurr, const vec3& vDirCurr, vec3& vPosTo, vec3& vDirTo, vec3& vUpVec, float fDelta)
{
	XMovableFactor::SGrapple& GRAPPLE = rMovFactor.GRAPPLE;

	if ( GRAPPLE.m_bGrappled == false)
		return;

	// 잡기전 보간 처리...
	if( GRAPPLE.m_fGrappledRemainStartInterpolationTime > 0.0f)
		_UpdateGrapplStart(rMovFactor, vPosCurr, vPosTo, vDirTo, vUpVec, fDelta);

	// 
	if( GRAPPLE.m_fGrappledRemainStartInterpolationTime == 0.0f)
	{
	// 잡히기만 한 상태이면...
		if ( GRAPPLE.m_bGrappledBeThrown == false)
			_UpdateGrappled( rMovFactor, vPosCurr, vPosTo, vDirTo, vUpVec, fDelta);

		// 날아가는 상태이면...
		else
			_UpdateThrow( rMovFactor, vPosCurr, vPosTo, vDirTo, fDelta);
	}
}


void XMovableProcThrow::_UpdateGrappled( XMovableFactor& rMovFactor, const vec3& vPosCurr, vec3& vPosTo, vec3& vDirTo, vec3& vUpVec, float fDelta)
{
	XMovableFactor::SGrapple& GRAPPLE = rMovFactor.GRAPPLE;
	GRAPPLE.m_bGrappledAttach = false;

	// Grip 더미의 위치, 방향을 구한다
	vec3 _GripPos, _GripDir, _GripUpVec;
	if ( _GetGripPosDir( GRAPPLE.m_uidGrappledAttacker, _GripPos, _GripDir, _GripUpVec) == false)
		return;


	// 잡힌 위치 설정
	vPosTo = _GripPos - vPosCurr;
	vDirTo = _GripDir;
	vUpVec = _GripUpVec;

	GRAPPLE.m_bGrappledAttach = true;

	//RMatrix mat;
	//mat.SetTranslationMatrix(_GripPos);
	//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 10, 3, D3DCOLOR_XRGB(255, 255, 255));
#ifdef GRAPLLED_DEBUG_LINE
	RMatrix mat;
	mat.MakeIdentity();
	mat.SetTranslation( vPosCurr);
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 2, 20, D3DCOLOR_XRGB(255, 0, 255));
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugLine( vPosCurr, vPosCurr + vPosTo, 20, D3DCOLOR_XRGB(255, 0, 255));
#endif
}


void XMovableProcThrow::_UpdateThrow( XMovableFactor& rMovFactor, const vec3& vPosCurr, vec3& vPosTo, vec3& vDirTo, float fDelta)
{
	XMovableFactor::SState& GENERAL_STATE = rMovFactor.GENERAL_STATE;
	XMovableFactor::SGrapple& GRAPPLE = rMovFactor.GRAPPLE;


	if ( GRAPPLE.m_vGrappledVelocity == 0.0f)
	{
		vDirTo = GRAPPLE.m_vGrappledEndDir;
		XMath::NormalizeZ0( vDirTo);
		return;
	}


	// Update time
	GRAPPLE.m_fGrappledElapsed += fDelta;


	// Get next position
	vec3 vNextPos = GRAPPLE.m_vGrappledThrownStartPos + GRAPPLE.m_vGrappledVelocity * GRAPPLE.m_fGrappledElapsed;
	vNextPos.z += 0.5f * GRAVITY * GRAPPLE.m_fGrappledElapsed * GRAPPLE.m_fGrappledElapsed;

	
	// Update position
	vPosTo = vNextPos - vPosCurr;

	// Update direction
	vDirTo = Normalize( vPosTo);

	if ( GENERAL_STATE.m_bColTerrain == true)
	{
		vDirTo = GRAPPLE.m_vGrappledEndDir;
		XMath::NormalizeZ0( vDirTo);

		if(GRAPPLE.m_bGrappledStuck)
			GRAPPLE.m_vGrappledVelocity = vec3::ZERO;
	}
	else
	{
		vDirTo = -vDirTo;

		// 날아갈때 방향이 위쪽이면... 이쁘지 않아서 잡기 끝났을때 모델 방향으로 설정(이때 up이 (0,0,1) 방향으로 해서 dir이 계산이 되어 있기 때문이다.)
		float fDot = DotProduct(vDirTo,vec3::AXISZ);
		if(fDot > 0.97f)
		{
			vDirTo = GRAPPLE.m_vGrappledEndDir;
		}
	}


#ifdef GRAPLLED_DEBUG_LINE
	RMatrix mat;
	mat.MakeIdentity();
	mat.SetTranslation( vPosCurr);
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 2, 3, D3DCOLOR_XRGB(255, 255, 255));
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugLine( vPosCurr, vPosCurr + vPosTo, 3, D3DCOLOR_XRGB(255, 255, 255));
#endif
}


bool XMovableProcThrow::_GetGripPosDir( const MUID& uidAttacker, vec3& vPos, vec3& vDir, vec3& vUpVec)
{
	XObject* pAttacker = gg.omgr->Find( uidAttacker);
	if ( pAttacker == NULL)
		return false;

	XCharacter* pCharacter = pAttacker->GetActor();
	if ( pCharacter == NULL)
		return false;

	RMatrix mat;
	if(pCharacter->GetActorNodeTransform(GRAPPLED_NPC_GRIP_BONE_NAME, mat, E_TS_WORLD))
	{
		vPos.Set(mat._41, mat._42, mat._43);
		vDir.Set(-mat._21, -mat._22, -mat._23);
		vUpVec.Set(mat._31, mat._32, mat._33);

		//RMatrix mat;
		//mat.MakeIdentity();
		//mat.SetTranslation( vPos);
		//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBoxAndAxis( mat, 2, 20, D3DCOLOR_XRGB(0, 0, 255));

		return true;
	}

	return false;
}

void XMovableProcThrow::_UpdateGrapplStart( XMovableFactor& rMovFactor, const vec3& vPosCurr, vec3& vPosTo, vec3& vDirTo, vec3& vUpVec, float fDelta )
{
	rMovFactor.GRAPPLE.m_fGrappledRemainStartInterpolationTime -= fDelta;

	if(m_bGripStart == false)
	{
		m_bGripStart = true;
	}

	if(rMovFactor.GRAPPLE.m_fGrappledRemainStartInterpolationTime <= 0.0f)
	{
		rMovFactor.GRAPPLE.m_fGrappledRemainStartInterpolationTime = 0.0f;
		m_bGripStart = false;
	}


	float fd = rMovFactor.GRAPPLE.m_fGrappledRemainStartInterpolationTime / rMovFactor.GRAPPLE.m_fGrappledStartInterpolationTime;
	vec3 _GripPos = rMovFactor.GRAPPLE.m_vGrappledReadyPos.GetInterpolated(rMovFactor.GRAPPLE.m_vGrappledStartPos, fd);

	vPosTo = _GripPos - vPosCurr;

	//// Grip 더미의 위치, 방향을 구한다
	//vec3 _GripPos, _GripDir, _GripUpVec;
	//if ( _GetGripPosDir( rMovFactor.GRAPPLE.m_uidGrappledAttacker, _GripPos, _GripDir, _GripUpVec) == false)
	//	return;

#ifdef GRAPLLED_DEBUG_LINE
	RMatrix mat;
	mat.MakeIdentity();
	mat.SetTranslation( vPosCurr);
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox( mat, 2, 20, D3DCOLOR_XRGB(255, 255, 0));
	REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugLine( vPosCurr, vPosCurr + vPosTo, 20, D3DCOLOR_XRGB(255, 255, 0));
#endif
}
