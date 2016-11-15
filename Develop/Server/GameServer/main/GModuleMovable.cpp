#include "StdAfx.h"
#include "GModuleMovable.h"
#include "stdafx.h"
#include "GTask_Move.h"
#include "GConst.h"
#include "CCommandTable.h"
#include "GPathFinder.h"
#include "GEntityNPC.h"
#include "GField.h"
#include "GTaskUtil.h"
#include "GNPCPathFinder.h"
#include "GMath.h"
#include "GTestSystem.h"
#include "GGlobal.h"


GModuleMovable::GModuleMovable( GEntity* pOwner )
: GModule(pOwner)
, m_vGoalPos(vec3::ZERO)
, m_vFinishDir(vec3::ZERO)
, m_fLeastGap(0.0f)
, m_nStrategyType(MS_STRAIGHT)
, m_nWay(MW_STRAIGHT)
, m_nChangeWayCounter(0)
//, m_nNextChangeWay(-1)
, m_pStrategy(NULL)
, m_bMoving(false)
, m_bSuspended(false)
{
}
GModuleMovable::~GModuleMovable(void)
{
	SAFE_DELETE(m_pStrategy);
}

void GModuleMovable::Move( vec3 vTargetPos, float fLeastGap, GTaskMoveStrategy nStrategyType )
{
	//	mlog("move - %.1f %.1f %.1f\n", vTargetPos.x, vTargetPos.y, vTargetPos.z);

	m_bSuspended = false;
	m_vFinishDir = vec3::ZERO;
	m_vGoalPos = vTargetPos;
	m_nStrategyType = nStrategyType;
	m_fLeastGap = fLeastGap;

	if (m_pStrategy)
	{
		SAFE_DELETE(m_pStrategy);
	}

	m_pStrategy = NewStrategy(m_nStrategyType, m_fLeastGap);
}

bool GModuleMovable::Start()
{
	PFI_BLOCK_THISFUNC(1950);

	CalcGoalDir();
	CalcRotateGoalDir();

	if (m_vGoalDir.LengthSq() <= 0.1f)
		return false;	// 이동할 수 없는 목표 방향

	if (!m_pStrategy) 
		return false;

	if (!GetOwnerNPC()->IsMovable())
		return false;

	vec3 vStartPos, vFinalGoalPos;

	vStartPos = m_pOwner->GetPos();
	vFinalGoalPos = m_vGoalPos;

	// 확정적 이동일때만 flexible한 이동처리
	//if (m_pStrategy->GetType() == MS_DEFINITE)
	//{
	//	vec3 vFlexibleGoalPos = CalcFlexibleDestPos(GetOwnerNPC(), vStartPos, m_vGoalPos, m_fLeastGap);

	//	if (CheckValidation(vFlexibleGoalPos))
	//	{
	//		vFinalGoalPos = vFlexibleGoalPos;
	//	}
	//}


	if (!m_pStrategy->Start(vFinalGoalPos, m_vGoalDir, m_fLeastGap))
	{
		return false;
	}

	m_vStartDir = m_vGoalDir;
	m_vStartGoalPos = m_vGoalPos;

	// 	if (!IsNeedRotation())
	// 	{
	// 		if (vec3::ZERO != m_vRotateGoalDir)
	// 		{
	// 			m_pOwner->SetDir(m_vRotateGoalDir);
	// 			m_pOwner->SetFacingDir(m_vRotateGoalDir);			
	// 		}
	// 	}	

	m_bMoving = true;

	return true;
}

bool GModuleMovable::IsMoveFinish()
{
	return !m_bMoving; 
}

void GModuleMovable::Complete()
{
	if (m_vFinishDir != vec3::ZERO)
	{
//		mmlog("%s - %.1f %.1f\n", __FUNCTION__, m_vFinishDir.x, m_vFinishDir.y);
		// 마지막 각도 지정
		m_pOwner->SetDir(m_vFinishDir);
		m_pOwner->SetFacingDir(m_vFinishDir);
		GetOwnerNPC()->RouteFaceTo(m_vFinishDir);
	}
	MoveFinish();
}

void GModuleMovable::Cancel()
{
	GetOwnerNPC()->RouteMoveStop();
	MoveFinish();
	m_bSuspended = false;
}

void GModuleMovable::Suspend()
{
	GetOwnerNPC()->RouteMoveStop();
	m_bSuspended = true;
}

void GModuleMovable::Resume()
{
	GetOwnerNPC()->RouteMove(m_vGoalPos);
	m_bSuspended = false;
}

void GModuleMovable::Reset( GTask* pNewTask )
{
	GTask_Move* pNewMoveTask = static_cast<GTask_Move*>(pNewTask);

	m_vGoalPos = pNewMoveTask->m_vGoalPos;
	m_fLeastGap = pNewMoveTask->m_fLeastGap;
	m_nWay = pNewMoveTask->m_nWay;
	m_nChangeWayCounter = pNewMoveTask->m_nChangeWayCounter;

	CalcRotateGoalDir();
	CalcGoalDir();
}

bool GModuleMovable::IsNeedRotation()
{
	return (GetOwnerNPC()->GetRotateSpeed() > 0.0f);
}

bool GModuleMovable::IsFinishRotate( float fTolerAngle )
{
	return GMath::IsEqualDirection(m_vRotateGoalDir, m_pOwner->GetDir(), fTolerAngle);
}
//
//vec3 GModuleMovable::CalcFlexibleDestPos( GEntityNPC* pAgentNPC, vec3& vStartPos, vec3& vDestPos, float fLeastGap )
//{
//	GNPCPathFinder* path_finder = pAgentNPC->GetPathFinder();
//
//	int nAgentNum = min(path_finder->GetGroupAgentCount(), 10);
//	float fDist = vStartPos.DistanceTo(vDestPos);
//
//	// 일정 시간마다 way를 바꾼다.
//	ChangeWay(nAgentNum, fDist);
//
//	if (m_nWay == MW_STRAIGHT) return vDestPos;
//
//	m_fLeastGap = 0.0f;
//
//	float fToler = 0.0f;
//	const float toler_factors[10] =
//	{
//		0.0f,
//		0.0f,
//		0.5f,
//		0.5f,
//		0.5f,
//		0.5f,
//		1.0f,
//		1.0f,
//		1.2f,
//		1.2f
//	};
//
//	float fTolerFactor = toler_factors[nAgentNum-1];
//
//
//	if (fDist < 500.0f)
//	{
//		fToler = 0.0f;
//	}
//	else if (fDist < 900.0f)
//	{
//		fToler = 100.0f * fTolerFactor;
//	}
//	else if (fDist < 1500.0f)
//	{
//		fToler = 200.0f * fTolerFactor;
//	}
//
//	fLeastGap = max(0.0f, fLeastGap - 10.0f);
//
//	const float angle_bounds[10] =
//	{
//		1.57f,
//		1.4f,
//		1.1f,
//		0.9f,
//		0.6f,
//		0.4f,
//		0.4f,
//		0.2f,
//		0.2f,
//		0.0f
//	};
//
//	float fAngleBound = angle_bounds[nAgentNum-1];
//
//
//	float lBound, rBound;
//
//	lBound = fAngleBound;
//	rBound = MMath::PI - fAngleBound;
//
//	if (m_nWay == MW_LEFT) lBound = 1.57f;
//	else if (m_nWay == MW_RIGHT) rBound = 1.57f;
//
//	float fRadius = RandomNumber(lBound, rBound);
//	float fDistance = fLeastGap + fToler;
//
//	MVector2 vTarPos = MMath::GetArcPosition(fRadius, fDistance);
//
//	vec3 vAgentDir = vStartPos - vDestPos;
//	vAgentDir.z = 0.0f;
//	vAgentDir.Normalize();
//
//	mat4 matTM;
//	matTM.SetLocalMatrix(vec3::ZERO, vAgentDir, vec3::AXISZ);
//
//	vec3 vToler;
//
//	vToler.x = vTarPos.x;
//	vToler.y = vTarPos.y;
//	vToler.z = 0.0f;
//	vToler = vToler * matTM;
//
//	return (vDestPos + vToler);
//}


GMoveStrategy* GModuleMovable::NewStrategy(GTaskMoveStrategy nStrategy, float fLeastGap)
{
	switch (nStrategy)
	{
	case MS_STRAIGHT: 
		return (new GStraightMoveStrategy(GetOwnerNPC(), fLeastGap));
	case MS_DEFINITE:
		{
			return (new GDefiniteMoveStrategy(GetOwnerNPC(), fLeastGap));
		}
	default:
		_ASSERT(0);
	}

	return (new GDefiniteMoveStrategy(GetOwnerNPC(), fLeastGap));
}
//
//void GModuleMovable::ChangeWay(int nAgentNum, float fDistanceFromEnemy)
//{
//	m_nChangeWayCounter++;
//	if (m_nChangeWayCounter > m_nNextChangeWay)
//	{
//		m_nChangeWayCounter = 0;
//		m_nNextChangeWay = GMath::RandomNumber(3, 8);
//	}
//
//	if (nAgentNum <= 1) 
//	{
//		m_nWay = MW_STRAIGHT;
//		return;
//	}
//
//	if (nAgentNum == 2)
//	{
//		int nRand = GMath::RandomNumber(0, 10);
//		if (nRand <= 4) m_nWay = MW_STRAIGHT;
//		else if (nRand <= 7) m_nWay = MW_LEFT;
//		else m_nWay = MW_RIGHT;
//
//		return;
//	}
//
//	if (nAgentNum == 3)
//	{
//		int nRand = GMath::RandomNumber(0, 10);
//		if (nRand <= 3) m_nWay = MW_STRAIGHT;
//		else if (nRand <= 7) m_nWay = MW_LEFT;
//		else m_nWay = MW_RIGHT;
//
//		return;
//	}
//
//	int nRand = GMath::RandomNumber(0, 10);
//	if (nRand <= 2) m_nWay = MW_STRAIGHT;
//	else if (nRand <= 6) m_nWay = MW_LEFT;
//	else m_nWay = MW_RIGHT;
//}

void GModuleMovable::CalcGoalDir()
{
	m_vGoalDir = m_vGoalPos - m_pOwner->GetPos();
	GMath::NormalizeZ0(m_vGoalDir);
}

void GModuleMovable::CalcRotateGoalDir()
{
	m_vRotateGoalDir = m_vGoalPos - m_pOwner->GetPos();
	GMath::NormalizeZ0(m_vRotateGoalDir);
}

bool GModuleMovable::CheckValidation(vec3 vTestGoalPos)
{
	VALID_RET(m_pOwner->GetField(), false);
	gsys.pTestSystem->CheckNaN(__FUNCTIONW__, vTestGoalPos);

	return (gsys.pPathFinder->IsValidPosition(m_pOwner->GetField()->GetPathMesh(), vTestGoalPos));
}

void GModuleMovable::SetFinishDir( vec3 vFinishDir )
{
	m_vFinishDir = vFinishDir;
}

GEntityNPC* GModuleMovable::GetOwnerNPC() const
{
	return ToEntityNPC(m_pOwner);
}

const wchar_t* GModuleMovable::GetTaskName()
{
	if (!IsRunForTest())
	{
		return L"Move";
	}

	static wstring strName;

	strName = L"Move (";
	switch (m_nStrategyType)
	{
	case MS_DEFINITE:	strName += L"Definite";		break;
	case MS_STRAIGHT:	strName += L"Straight";		break;
	default:			strName += L"None";

	}
	strName += L")";

	return strName.c_str();
}

void GModuleMovable::OnUpdate( float fDelta )
{
	PFI_BLOCK_THISFUNC(1801);

	if (!m_bMoving)
		return;

	if (m_bSuspended)
		return;

	if (!m_pStrategy)
		return;

	// Update
	PFI_B(9079, "GModuleMovable::StrategyUpdate");
	m_pStrategy->Update( fDelta);
	PFI_E(9079);

	// Move
	PFI_B(9080, "GModuleMovable::CalcGoalDir");
	CalcGoalDir();
	PFI_E(9080);

	// Rotate
	//	fRotateSpeed = m_pOwner->GetRotateSpeed();
	//	bool bRotateFinish = _CalcDir( &vFinalDir, m_pOwner->GetDir(), m_vRotateGoalDir, fRotateSpeed, fDelta);

	if (m_pStrategy->IsReachGoalPos())
	{
		MoveFinish();
	}
}

void GModuleMovable::MoveFinish()
{
	SAFE_DELETE(m_pStrategy);
	m_bMoving = false;
}