#include "stdafx.h"
#include "XMovableProcObject.h"
#include "XModuleCollision.h"
#include "XModuleMovable.h"
#include "XModuleTalent.h"
#include "XMovableProcObjectChecker.h"

void XMovableProcObject::Init( XModuleMovable* pProcOwner )
{
	m_pMovableProcOwner	= pProcOwner;
	m_pModuleCollision	= pProcOwner->m_pModuleCollision;

}

bool XMovableProcObject::IsProcessable( const XMovableFactor& _rMovableFactor )
{
	if (_rMovableFactor.GRAPPLE.m_bGrappled)
		return false;

	return true;
}

void XMovableProcObject::Run( XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos, const vec3& _rVecCurrentDir, vec3& _rVecTo, vec3& _rVecDir, vec3& _rUpVec, float fDelta )
{
	_ASSERT(m_pModuleCollision);

	//----------------------------------------------------------------------//
	// Factor들 초기화
	_rMovableFactor.GENERAL_STATE.m_bColObject = false;

	//----------------------------------------------------------------------//
	// 상대 캐릭터가 특수한 상황중에서는 오브젝트 충돌을 허용한다.
	XMovableProcObjectChecker objectChecker;
	bool bObjectCol = objectChecker.CheckObjectCollision(_rMovableFactor, m_pModuleCollision->GetOwner());

	if(bObjectCol == false)
		return;

	//----------------------------------------------------------------------//
	// 오브젝트 충돌 검사
	bool bSliding = (_rMovableFactor.MOVING_DUMMY.m_vDummyPosElipsed == vec3::ZERO);
	

	float fDist = 0.0f;
	vec3 vImpactPos;
	vec3 vImpactDir;
	int nCurrentTalentID = 0;
	
	bool bRub = objectChecker.CheckCanRub(_rMovableFactor, m_pModuleCollision->GetOwner(), _rMovableFactor.GENERAL_STATE.m_nTalentActiveID);
	bool bOffset = objectChecker.CheckCapsuleRadiusOffset(_rMovableFactor, m_pModuleCollision->GetOwner());

	//if(m_pModuleCollision->GetOwner()->GetUID() == XGetMyUID())
	//{
	//	mlog("오브젝트 %f %f %f\n", _rVecTo.x, _rVecTo.y, _rVecTo.z);
	//}
	//if(m_pModuleCollision->GetOwner()->GetType() == XOBJ_NET_PLAYER &&
	//	_rMovableFactor.GENERAL_STATE.m_bJumping)
	//{
	//	mlog("벨로시티 %f %f\n", _rVecTo.x, _rVecTo.y);
	//}

	//----------------------------------------------------------------------//
	//

	_rMovableFactor.GENERAL_STATE.m_vecTerrainColObejct.clear();

	if(m_pModuleCollision->ColObject(_rVecTo, _rVecCurrentPos, vImpactPos, vImpactDir, fDist, _rMovableFactor.GENERAL_STATE.m_vecTerrainColObejct, bRub, bOffset))
	{
		vImpactDir.z = 0.0f;
		vImpactDir.Normalize();
		_rMovableFactor.GENERAL_STATE.m_bColObject = true;

		vec3 vMoveDir = vImpactDir * fDist;

		// 서로 반대 방향이면서 vMoveDir값이 더 크면 뒤로 밀려나게 되는 것이므로 
		// 이렇게 되면 적용하지 않는다.
		if (_rMovableFactor.KNOCK_BACK.m_bKnockback)
		{
			float _rVecToLenSq = _rVecTo.LengthSq();
			float vMoveDirLensq = vMoveDir.LengthSq();

			if (_rVecToLenSq > 0 && vMoveDirLensq > 0)
			{
				vec3 vToNormal = _rVecTo;
				vToNormal.Normalize();

				float fDot = vToNormal.DotProduct(vImpactDir);
				if (fDot < 0.0f && vMoveDirLensq > _rVecToLenSq)
				{
					return;
				}
			}
		}

		_rVecTo += vMoveDir;
	}
}
