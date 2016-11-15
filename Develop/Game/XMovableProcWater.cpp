#include "stdafx.h"
#include "XMovableProcWater.h"
#include "XModuleCollision.h"
#include "XModuleMovable.h"

#include "RDebugPassRenderer.h"
#include "RCameraSceneNode.h"
void XMovableProcWater::Init( XModuleMovable* pProcOwner )
{
	m_pMovableProcOwner	= pProcOwner;
	m_pModuleCollision	= pProcOwner->m_pModuleCollision;

	_ASSERT(m_pModuleCollision);
}

bool XMovableProcWater::IsProcessable( const XMovableFactor& _rMovableFactor )
{
	if (_rMovableFactor.GENERAL_STATE.m_bSwim == false ||
		_rMovableFactor.GRAPPLE.m_bGrappled == true ||
		m_pModuleCollision->GetOwner()->GetEntityType() == ETID_NPC )
	{
		return false;
	}

	return true;
}

void XMovableProcWater::Run( XMovableFactor& _rMovableFactor, const vec3& _rVecCurrentPos, const vec3& _rVecCurrentDir, vec3& _rVecTo, vec3& _rVecDir, vec3& _rUpVec, float fDelta )
{
	vec3 vWaterPos;
	vec3 now_pos		= _rVecCurrentPos;
	vec3 target			= now_pos + _rVecTo;
	vec3 vOut			= now_pos;
	float fWaterLimit	= SWIMMING_HEIGHT_VALUE;

	if(m_pModuleCollision->PickTerrainWater(target, vWaterPos))
	{
		// 물이 있다면...
		// 지형충돌 체크
		vec3 vNor;
		target			= now_pos + vec3(_rVecTo.x, _rVecTo.y, 0.0f);

		//맵에 닿으면
		if(m_pModuleCollision->WaterMoveTestMap(now_pos, target, fWaterLimit, vOut, vNor, &_rMovableFactor.GENERAL_STATE.m_vecTerrainColObejct))
		{
			float fDot = vNor.DotProduct(RVector::AXISZ);

			if(vWaterPos.z - vOut.z <= fWaterLimit)
			{
				vOut.z = vWaterPos.z - fWaterLimit;

				if(fDot >= 0.76f)
				{
					// 지형으로 나갈수 있는 조건이 된다.
					_rMovableFactor.GENERAL_STATE.m_bSwimEnd = true;
				}

				_rVecTo = vOut - _rVecCurrentPos;
				return;
			}

			vWaterPos.x = vOut.x;
			vWaterPos.y = vOut.y;
		}
		else
		{
			// 맵에 대한 예외처리 - 지형 밑에 들어가버렸다면...
			vec3 vOutTerrain;
			m_pModuleCollision->CheckXYTerrainCollision(target, vec3(0, 0, 0), vOutTerrain);
			if(vOutTerrain.z > vWaterPos.z)
			{
				_rMovableFactor.GENERAL_STATE.m_bSwimEnd = true;
				_rVecTo = vOutTerrain - _rVecCurrentPos;
				return;
			}
		}

		// 여기서 좌표 높이값은 수면 높이다. 
		// 원점이 밑에 있으므로 좌표를 조금 내린다.
		vWaterPos.z	-= fWaterLimit;
		vOut	= vWaterPos;
		_rVecTo = vOut - _rVecCurrentPos;
		_rMovableFactor.VELOCITY.m_vVelocity.z = 0.0f;
	}
	else
	{
		_rMovableFactor.GENERAL_STATE.m_bSwimEnd = true;
	}
}