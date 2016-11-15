#include "stdafx.h"
#include "XNPCAligner.h"
#include "XNonPlayer.h"
#include "XMyPlayer.h"
#include "XModuleMotion.h"
#include "XModuleEntity.h"
#include "XModuleCollision.h"
#include "XModuleAction.h"
#include "XModuleBuff.h"
#include "XModuleTalent.h"

vec3 XNPCAligner::XAlignCalculator::CalcEnemyDir( vec3& vOwnerPosition )
{
	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	vec3 vEnemyDir = pMyPlayer->GetPosition() - vOwnerPosition;
	vEnemyDir.z = 0.0f;
	vEnemyDir.Normalize();

	return vEnemyDir;
}

float XNPCAligner::XAlignCalculator::CalcDistance( vec3& vOwnerPosition )
{
	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;
	return (pMyPlayer->GetPosition().DistanceTo(vOwnerPosition));

}

vec3 XNPCAligner::XAlignCalculator::CalcMoveDir( vec3& vOwnerPosition, vec3& vTarPos )
{
	vec3 vMoveDir = vTarPos - vOwnerPosition;
	vMoveDir.z = 0;
	vMoveDir.Normalize();

	return vMoveDir;
}

vec3 XNPCAligner::XAlignCalculator::CalcThisNPCDir( XNonPlayer* pOwner )
{
	vec3 vRenderDir = pOwner->GetModuleEntity()->GetRenderDirection();
	if (vRenderDir != vec3::ZERO)
	{
		return vRenderDir;
	}

	return pOwner->GetDirection();
}

//////////////////////////////////////////////////////////////////////////

bool XNPCAligner::XAlignChecker::IsEnemy( XNonPlayer* pOwner )
{
	TD_ENEMY_INFO* pEnemyInfo = gvar.Game.EnemyInfo.GetEnemyInfoByUIID(pOwner->GetUIID());

	if(pEnemyInfo == NULL)
	{
		return false;
	}

	return true;
}

bool XNPCAligner::XAlignChecker::CheckMotion( XModuleMotion* pModuleMotion )
{
	if (pModuleMotion->IsCurrentMotion(MOTION_NAME_NPC_IDLE) || 
		pModuleMotion->IsCurrentMotion(MOTION_NAME_NPC_IDLE_COMBAT) || 
		pModuleMotion->IsCurrentMotion(MOTION_NAME_NPC_WALK) ||
		pModuleMotion->IsCurrentMotion(MOTION_NAME_NPC_RUN))
	{
		return true;
	}
	return false;
}


bool XNPCAligner::XAlignChecker::CheckAngleEnabled( vec3 vDir1, vec3 vDir2, float fEnemyDistance, bool bMoving )
{
	vDir1.z = 0.0f;	vDir1.Normalize();
	vDir2.z = 0.0f;	vDir2.Normalize();

	float fAngleRadian = abs(vDir1.AngleBetween(vDir2));

	float fTolerAngleRadian;

	if (bMoving)
	{
		fTolerAngleRadian = (XMath::HALFPI * 0.5f);
	}
	else
	{
		fTolerAngleRadian = XMath::PI;
	}

	if (fAngleRadian < fTolerAngleRadian)
	{
		return true;
	}

	return false;
}

bool XNPCAligner::XAlignChecker::CheckNPCInfo( XNonPlayer* pOwner )
{
	if (pOwner == NULL) return false;
	if (pOwner->GetInfo(true) == NULL) return false;

	return true;
}

bool XNPCAligner::XAlignChecker::CheckStunAndRooted( bool bIsStun, XBuffAttribute& buffAttribute )
{
	if (bIsStun) return false;
	if (buffAttribute.bRooted) return false;

	return true;
}

bool XNPCAligner::XAlignChecker::CheckEnemyList( XNonPlayer* pOwner )
{
	vector<TD_ENEMY_INFO>& enemylist = gvar.Game.EnemyInfo.GetEnemyList();
	for(vector<TD_ENEMY_INFO>::iterator it = enemylist.begin(); it != enemylist.end(); ++it)
	{
		if(it->nUIID == pOwner->GetUIID())
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

XNPCAligner::XNPCAligner() : m_bEnabled(false), m_nActionState(NPC_ACTION_STATE_NONE)
{

}

XNPCAligner::~XNPCAligner()
{

}

void XNPCAligner::SetEnabled( XModuleEntity* pModuleEntity, bool bAlign )
{
	if (m_bEnabled == bAlign) return;

	m_bEnabled = bAlign;

	if (bAlign == false)
	{
		pModuleEntity->StopDirectionInterpolation();
		pModuleEntity->SetRenderDirection(vec3::ZERO);
	}
}

void XNPCAligner::Update( XNonPlayer* pOwner, float fDelta )
{
	if (XAlignChecker::CheckNPCInfo(pOwner) == false)
	{
		return;
	}

	UpdateEnemyAlign(pOwner, fDelta);

	UpdateTerrainAlign(pOwner);
}

void XNPCAligner::OnEnterMoveState( XNonPlayer* pOwner, vec3& vTarPos )
{
	if (XAlignChecker::CheckNPCInfo(pOwner) == false)
	{
		return;
	}
	m_nActionState = NPC_ACTION_STATE_MOVE;

	if (pOwner->GetInfo(true)->bAlignEnemy == false) return;

	if(XAlignChecker::CheckEnemyList(pOwner) == false) return;

	m_bEnabled = true;
	OnMoveUpdate(pOwner, vTarPos);
}

void XNPCAligner::OnEnterState( XNonPlayer* pOwner, NPC_ACTION_STATE nActionState )
{
	if (XAlignChecker::CheckNPCInfo(pOwner) == false ||
		XAlignChecker::CheckEnemyList(pOwner) == false)
	{
		return;
	}

	m_nActionState = nActionState;

	switch (nActionState)
	{
	case NPC_ACTION_STATE_NA:
	case NPC_ACTION_STATE_MOVE:
		{
			if (pOwner->GetInfo(true)->bAlignEnemy)
			{
				m_bEnabled = true;
			}
		}
		break;
	case NPC_ACTION_STATE_TALENT:
		{
			m_bEnabled = true;
		}
		break;
	}


}

void XNPCAligner::OnExitState( XNonPlayer* pOwner )
{
	if (XAlignChecker::CheckNPCInfo(pOwner) == false ||
		XAlignChecker::CheckEnemyList(pOwner) == false)
	{
		return;
	}

	switch (m_nActionState)
	{
	case NPC_ACTION_STATE_NA:
	case NPC_ACTION_STATE_MOVE:
		{
			if (pOwner->GetInfo(true)->bAlignEnemy)
			{
				SetEnabled(pOwner->GetModuleEntity(), false);
			}
		}
		break;
	case NPC_ACTION_STATE_TALENT:
		{
			SetEnabled(pOwner->GetModuleEntity(), false);
		}
		break;
	}

	m_nActionState = NPC_ACTION_STATE_NONE;
}


void XNPCAligner::OnMoveUpdate( XNonPlayer* pOwner, vec3& vTarPos )
{
	if (!m_bEnabled) return;

	if (XAlignChecker::CheckNPCInfo(pOwner) == false)
	{
		return;
	}

	if (pOwner->GetInfo(true)->bAlignEnemy == false) return;

	if (m_nActionState != NPC_ACTION_STATE_MOVE &&
		m_nActionState != NPC_ACTION_STATE_NA)
	{
		return;
	}

	if (XAlignChecker::IsEnemy(pOwner) == false)
	{
		SetEnabled(pOwner->GetModuleEntity(), false);
		return;
	}

	vec3 vEnemyDir = XAlignCalculator::CalcEnemyDir(pOwner->GetPosition());
	vec3 vMoveDir = XAlignCalculator::CalcMoveDir(pOwner->GetPosition(), vTarPos);
	float fEnemyDistance = XAlignCalculator::CalcDistance(pOwner->GetPosition());

	if (XAlignChecker::CheckAngleEnabled(vMoveDir, vEnemyDir, fEnemyDistance, true) == false)
	{
		if (pOwner->GetModuleEntity()->GetRenderDirection() != vec3::ZERO)
		{
			SetRenderDirection(pOwner, vMoveDir);
		}

		SetEnabled(pOwner->GetModuleEntity(), false);
	}
}




//bool XNPCAligner::UpdateInterpolation( XNonPlayer* pOwner, float fDelta )
//{
//	if (m_bInterpolation == false) return false;
//
//	bool bInterpoRet = m_DirInterpo.Interpolate(fDelta);
//
//	if (bInterpoRet)
//	{
//		m_bInterpolation = false;
//	}
//
//	vec3 vInterpoDir = m_DirInterpo.GetValue();
//
//	SetRenderDirection(pOwner, vInterpoDir);
//
//
//	return true;
//}

void XNPCAligner::StartInterpolation( XNonPlayer* pOwner, vec3 vToDir )
{
	// 한바퀴 도는 시간
	const float ROTATE_SPEED = 0.5f;

	pOwner->GetModuleEntity()->SetDirectionSmoothly(vToDir, ROTATE_SPEED);
}

void XNPCAligner::SetRenderDirection( XNonPlayer* pOwner, vec3 vDir )
{
	pOwner->SetDirection(vDir);
	pOwner->GetModuleEntity()->SetRenderDirection(vDir);
}

void XNPCAligner::UpdateTerrainAlign( XNonPlayer* pOwner )
{
	if(pOwner->GetInfo(true)->bAlignTerrain == false) return;

	bool bAligningEnemyNow = (pOwner->GetInfo(true)->bAlignEnemy && m_bEnabled);

	m_TerrainAligner.Update(pOwner, bAligningEnemyNow);
}

void XNPCAligner::_UpdateEnemyAlignOnMoveState( XNonPlayer* pOwner, float fDelta )
{
	if (pOwner->GetInfo(true)->bAlignEnemy == false) return;

	if (XAlignChecker::IsEnemy(pOwner) == false)
	{
		m_bEnabled = false;
		return;
	}

	if (XAlignChecker::CheckMotion(pOwner->GetModuleMotion()) == false)
	{
		return;
	}

	if (pOwner->GetModuleEntity()->IsDirectionInterpolatingNow() == true)
	{
		return;
	}

	if (XAlignChecker::CheckStunAndRooted(pOwner->GetModuleAction()->IsStun(),
			pOwner->GetModuleBuff()->GetBuffAttribute()) == false)
	{
		return;
	}

	DoAlignEnemy(pOwner);
}

void XNPCAligner::_UpdateEnemyAlignOnTalentState( XNonPlayer* pOwner, float fDelta )
{
	if (XAlignChecker::IsEnemy(pOwner) == false)
	{
		m_bEnabled = false;
		return;
	}

	if (pOwner->GetModuleEntity()->IsDirectionInterpolatingNow() == true)
	{
		return;
	}

	XModuleTalent* pModuleTalent = pOwner->GetModuleTalent();
	if (pModuleTalent)
	{
		if (pModuleTalent->IsActive() && 
			pModuleTalent->GetInfo())
		{
			if (pModuleTalent->GetPhase() == TALENT_PHASE_PREPARE)
			{
				if (pModuleTalent->GetInfo()->m_bUseAiming &&
					!pModuleTalent->GetInfo()->m_bSetDirection)
				{
					DoAlignEnemy(pOwner);
				}
			}
			else
			{
				SetEnabled(pOwner->GetModuleEntity(), false);
			}
		}
	}
}

void XNPCAligner::DoAlignEnemy( XNonPlayer* pOwner )
{
	vec3 vEnemyDir = XAlignCalculator::CalcEnemyDir(pOwner->GetPosition());
	vec3 vThisNPCDir = XAlignCalculator::CalcThisNPCDir(pOwner);
	float fEnemyDistance = XAlignCalculator::CalcDistance(pOwner->GetPosition());

	if (XAlignChecker::CheckAngleEnabled(vEnemyDir, vThisNPCDir, fEnemyDistance, false))
	{
		vec3 vToDir = vEnemyDir;
		vToDir.z = vThisNPCDir.z;
		vToDir.Normalize();

		StartInterpolation(pOwner, vToDir);
	}
}

void XNPCAligner::UpdateEnemyAlign( XNonPlayer* pOwner, float fDelta )
{
	if (!m_bEnabled) return;

	switch (m_nActionState)
	{
	case NPC_ACTION_STATE_NA:
	case NPC_ACTION_STATE_MOVE:
		{
			_UpdateEnemyAlignOnMoveState(pOwner, fDelta);
		}
		break;
	case NPC_ACTION_STATE_TALENT:
		{
			_UpdateEnemyAlignOnTalentState(pOwner, fDelta);
		}
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
void XNPCAligner::XTerrainAligner::Update( XNonPlayer* pOwner, bool bAligningEnemyNow )
{
	if(pOwner->GetInfo()->bAlignTerrain == false) return;

	XModuleCollision * pModuleCollision = pOwner->GetModuleCollision();
	if(pModuleCollision)
	{
		vec3 vPos = pOwner->GetPosition();
		vec3 vDir = pOwner->GetDirection();

		if (bAligningEnemyNow)
		{
			if (pOwner->GetModuleEntity()->GetRenderDirection() != vec3::ZERO)
			{
				vDir = pOwner->GetModuleEntity()->GetRenderDirection();
			}
		}

		vec3 vStart	= vPos + RVector(0,0,100);
		vec3 vEnd	= vPos + RVector(0,0,-1000);
		vec3 vOutPos;
		vec3 vRenderDir;
		vec3 vRenderUp;

		if(pModuleCollision->PickRayToTerrain(vStart, vEnd, vOutPos, vRenderUp))
		{
			// 업벡터 셋팅
			vRenderUp.Normalize();

			// 실제 방향 구하기
			vec3 vCross		= vDir.CrossProduct(vRenderUp);
			vRenderDir		= vRenderUp.CrossProduct(vCross);
		}
		else
		{
			// 지형이 없다면...
			vRenderUp.Set(0.0f, 0.0f, 1.0f);
			vRenderDir.Set(vDir);
		}

		pOwner->GetModuleEntity()->SetRenderUpDir(vRenderUp, vRenderDir);
	}
}