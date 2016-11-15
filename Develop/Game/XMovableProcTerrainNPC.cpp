#include "stdafx.h"
#include "XMovableProcTerrainNPC.h"
#include "XModuleCollision.h"
#include "XModuleMovable.h"

void XMovableProcTerrainNPC::Init( XModuleMovable* pProcOwner )
{
	m_pMovableProcOwner	= pProcOwner;
	m_pModuleCollision	= pProcOwner->m_pModuleCollision;

	_ASSERT(m_pModuleCollision);
	_ASSERT(m_pModuleCollision->GetOwner()->GetEntityType() == ETID_NPC || m_pModuleCollision->GetOwner()->GetEntityType() == ETID_CUTSCENE);
}

bool XMovableProcTerrainNPC::IsProcessable( const XMovableFactor& _rMovableFactor )
{
	if (_rMovableFactor.GRAPPLE.m_bGrappled)
		return false;

	return true;
}

void XMovableProcTerrainNPC::Run( XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos, const vec3& _rVecCurrentDir, vec3& _rVecTo, vec3& _rVecDir, vec3& _rUpVec, float fDelta )
{
	_ASSERT(gg.game);

	// 맵 충돌
	//
	vec3 vOut				= _rVecCurrentPos;
	vec3 vOutNor;
	const float fCollRadius = m_pModuleCollision->GetRadius();
	vec3 vTo				= _rVecTo;
	vec3 now_pos			= _rVecCurrentPos;

	if(_rMovableFactor.INTERPOLATION.m_bWorking &&
		_rMovableFactor.INTERPOLATION.m_eMoveCheckType == MCT2_TARGET_HEGITH)
		now_pos.z				= _rMovableFactor.INTERPOLATION.m_vTargetPos.z;

	now_pos.z				+= NPC_PICKING_MAP_HEIGHT;			// 이전에는 radius 만큼 올려서 했지만... NPC에는 넘어갈 수 있는 높이 만큼해서 검사하자.

	// 넉백시는 서버와 같은 충돌 검사를 한다.
	if(_rMovableFactor.KNOCK_BACK.m_bKnockback)
	{
		now_pos.z = _rVecCurrentPos.z + fCollRadius;			// 넉백은 서버와 맞추기 위해서 z값을 radius 만큼한다.
		//----------------------------------------------------------------------------
		// 넉백 충돌 검사
		vTo.z = 0.0f;
		vec3 vNorVecTo		= vTo;
		vNorVecTo.Normalize();
		vec3 start_pos		= now_pos - vNorVecTo;
		vec3 target			= now_pos + vTo + (vNorVecTo * fCollRadius);
		float fDistance		= vTo.Length();

		if(m_pModuleCollision->PickRayToTerrain(start_pos, target, vOut, vOutNor))
		{
			float fPickDist = vOut.DistanceTo(start_pos);
/*			if (fPickDist < fCollRadius)
			{
				fDistance = fPickDist - fCollRadius;
			}
			else */if (fPickDist < fDistance + fCollRadius)
			{
				fDistance = fPickDist - fCollRadius;
			}
		}

		vOut	= _rVecCurrentPos + vNorVecTo * fDistance;
		vTo		= vOut - _rVecCurrentPos;
	}

	// 높이차로 인해 Direct로 판정이 나면 소극적인 Picking을 한다. 중력값이 포함되어있지 않다.
	if(_rMovableFactor.INTERPOLATION.m_eMoveCheckType == MCT2_DIRECT)
	{
		// Direct로 왔다는건 Interpolation에서 이동값을 Direct에 맞추어져있으므로 충돌 vEnd값도 거기에 맞추어야 한다.
		vec3 vEnd = _rVecCurrentPos + vTo + vec3(0.0f, 0.0f, -1.0f);
		if(m_pModuleCollision->PickRayToTerrain(now_pos, vEnd, vOut, vOutNor) == false)
			return;
	}
	else if (_rMovableFactor.VELOCITY.m_bUsingGravity)
	{
		// NPC는 이동시 XY 충돌체크를 하지 않는다.
		// 위에서 fCollRadius만큼 올려서 지형 계산을 해야 제대로 된 높이 값이 나온다. 
		// 그리고 CheckXYTerrainCollision()에서 지형 높을 바로 셋팅 하므로 fCollRadius만큰 다시 뺄 필요가 없다.
		m_pModuleCollision->CheckXYTerrainCollision(now_pos, vTo, vOut, 0.0f);

		//if (REngine::m_strDebugFlags == "a")
		{
			CheckAbnormalObstacle(vOut, vTo, _rVecCurrentPos);
		}
	}

	UpdateMoveCheckType(_rMovableFactor, _rVecCurrentPos);

	_rVecTo = vOut - _rVecCurrentPos;
}

bool XMovableProcTerrainNPC::CheckAbnormalObstacle( vec3& _out_nextPos, const vec3& _nextTo, const vec3& _currentPos )
{
	bool bApplied = false;

	float fGuardHeight = 20.f;
	float fGuardNext = 20.f;

	//static std::list< float > distList;

	float fMovedZ = _out_nextPos.z - _currentPos.z;
	if ( abs(fMovedZ) > fGuardHeight)
	{
		vec3 vNextNextTmp = _out_nextPos - _currentPos;
		vNextNextTmp.z = 0;
		vNextNextTmp = vNextNextTmp.Normalize() * fGuardNext;

		vec3 vNextNextPoint = _out_nextPos + vNextNextTmp;
		vNextNextPoint.z += NPC_PICKING_MAP_HEIGHT;

		vec3 vNextNextTo = _nextTo + vNextNextTmp;

		m_pModuleCollision->CheckXYTerrainCollision(vNextNextPoint, vNextNextTo, vNextNextTmp, 0.0f);
		float fPredicated = vNextNextTmp.z - _out_nextPos.z;
		if ( abs(fPredicated) > fGuardHeight  && (fMovedZ*fPredicated) < 0 )
		{
			_out_nextPos.z = _currentPos.z;
			bApplied = true;
			//static int a = 1;
			//mlog("효효 %d\n", ++a);
		}
	}

	//distList.push_front(fMovedZ);
	//while (distList.size() > 30)
	//{
	//	distList.pop_back();
	//}

	return bApplied;
}

void XMovableProcTerrainNPC::UpdateMoveCheckType( XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos )
{
	vec3 vMoveDir = (_rMovableFactor.INTERPOLATION.m_vTargetPos - _rVecCurrentPos).Normalize();
	vec3 vMoveDirZ0 = vMoveDir;
	XMath::NormalizeZ0(vMoveDirZ0);

	float fHeightGap = _rMovableFactor.INTERPOLATION.m_vTargetPos.z - _rVecCurrentPos.z;
	float fDot = vMoveDir.DotProduct(vMoveDirZ0);
	float fMoveLen = (_rMovableFactor.INTERPOLATION.m_vTargetPos - _rVecCurrentPos).Length();	
	
	if(fDot < SLIDING_LIMIT && fMoveLen <= MOVE_LEVITATION_LEN)		// 10 미터 이내에만 공중부양한다.
	{
		// 경사가 높다.
		if(fHeightGap >= MOVE_DIRECT_CHECK_HEIGHT)
			_rMovableFactor.INTERPOLATION.m_eMoveCheckType = MCT2_DIRECT;
		else if(fHeightGap > MOVE_TARGET_HEIGHT_CHECK_HEIGHT)
			_rMovableFactor.INTERPOLATION.m_eMoveCheckType = MCT2_TARGET_HEGITH;
	}
	else if(_rMovableFactor.INTERPOLATION.m_eMoveCheckType != MCT2_MOUSE_MOVE)
	{
		if(fHeightGap < MOVE_DIRECT_CHECK_HEIGHT && fHeightGap > MOVE_TARGET_HEIGHT_CHECK_HEIGHT)
			_rMovableFactor.INTERPOLATION.m_eMoveCheckType = MCT2_TARGET_HEGITH;
		else 
			_rMovableFactor.INTERPOLATION.m_eMoveCheckType = MCT2_NONE;
	}
}
