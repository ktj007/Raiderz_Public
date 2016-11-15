#include "stdafx.h"
#include "GMoveStrategy.h"
#include "GMath.h"
#include "GEntityNPC.h"
#include "GTaskUtil.h"
#include "GConst.h"
#include "GNPCPathFinder.h"
#include "GCommandTable.h"
#include "GField.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GTestSystem.h"
#include "GAIProfiler.h"
#include "GNPCMove.h"
#include "GPathFinder.h"

//////////////////////////////////////////////////////////////////////////
//
//		GMoveStrategy
//

GMoveStrategy::GMoveStrategy(GEntityNPC* pOwner, float fLeastGap)
: m_pOwner(pOwner)
, m_fLeastGap(fLeastGap)
, m_vGoalPos(vec3::ZERO)
, m_vGoalDir(-vec3::AXISY)
, m_vOldPos(vec3::ZERO)
{

}

GMoveStrategy::~GMoveStrategy()
{

}

bool GMoveStrategy::IsReachPos(const vec3& vDestPos, float fDelta)
{
	if (m_vOldPos == vec3::ZERO)
		return false; // 아직 이동하지 않았음

	// 이동 방향 검사
	vec3 vCurPos = GetOwnerNPC()->GetPos();
	vec3 vOldPos = m_vOldPos;

	// z축 고려하지 않고 검사
	vCurPos.z = vDestPos.z;
	vOldPos.z = vDestPos.z;

	vec3 vCurMovingDir = vDestPos - vCurPos;
	vCurMovingDir.Normalize();

	vec3 vOldMovingDir = vDestPos - vOldPos;
	vOldMovingDir.Normalize();

	if (vCurMovingDir.DotProduct(vOldMovingDir) < 0.0f)
		return true;	// 이미 지나쳤음

	// 위치 검사
	if (!GMath::IsEqual2DPosSQ(vDestPos, vCurPos, 10000.0f * fDelta/*fLeastGap*/))
		return false; // 평면위치 도착 않음

	if (abs(vDestPos.z - vCurPos.z) > 150.0f) 
		return false; // 층간 높이보다 높음

	return true;
}

bool GMoveStrategy::IsReachGoalPos(float fDelta)
{	
	return (IsReachPos(GetGoalPos(), fDelta));
}

bool GMoveStrategy::IsReachGoalPos()
{
	return IsReachGoalPos(1.0f/DEFAULT_GAME_TICK);
}

bool GMoveStrategy::SetValidateZPos(vec3& vResultPos)
{
	PFI_BLOCK_THISFUNC(9087);

	GField* pField = GetOwnerNPC()->GetField();
	if (!pField)	return false;

	if (!GetOwnerNPC()->SetPosToGroundVertical(&vResultPos))
		return false;

	return true;
}

GEntityNPC* GMoveStrategy::GetOwnerNPC()
{
	return m_pOwner;
}

vec3 GMoveStrategy::GetGoalPos()
{
	return m_vGoalPos;
}

vec3 GMoveStrategy::GetGoalDir()
{
	return m_vGoalDir;
}

void GMoveStrategy::RouteMoveStop()
{
	GetOwnerNPC()->RouteMoveStop();
}

void GMoveStrategy::RouteMove(vec3 vDestPos)
{	
	GetOwnerNPC()->RouteMove(vDestPos);
}

bool GMoveStrategy::Start(vec3 vGoalPos, vec3 vGoalDir, float fLeastGap)
{	
	VALID_RET(GetOwnerNPC(), false);
	GField* pField = GetOwnerNPC()->GetField();
	VALID_RET(pField, false);

	// 목적지 z축 좌표 보정
	pField->PickingZ(vGoalPos, vGoalPos);
	m_vGoalPos = vGoalPos;
	m_vGoalDir = vGoalDir;
	m_fLeastGap = fLeastGap;

	return true;
}




//////////////////////////////////////////////////////////////////////////
//
//		GDefiniteMoveStrategy
//

GDefiniteMoveStrategy::GDefiniteMoveStrategy( GEntityNPC* pOwner, float fLeastGap)
: GMoveStrategy(pOwner, fLeastGap)
, m_vNextDir(-vec3::AXISY)
, m_nMoveIndex(0)
, m_vWaypoint(vec3::ZERO)
{
}

GDefiniteMoveStrategy::~GDefiniteMoveStrategy()
{

}

bool GDefiniteMoveStrategy::Start(vec3 vGoalPos, vec3 vGoalDir, float fLeastGap)
{
	AI_PFI_BLOCK(APT_ID_DEFINITE_MOVE, L"DefiniteMove");

	if ( !FindPath( GetOwnerNPC()->GetPos(), vGoalPos))
		return false;

	if ( !__super::Start( vGoalPos, vGoalDir, fLeastGap))
		return false;

	SetNextWaypoint();
	RouteMoveNextWayPoint();

	return true;
}

void GDefiniteMoveStrategy::Update( float fDelta )
{
	AI_PFI_BLOCK(APT_ID_DEFINITE_MOVE, L"DefiniteMove");
	PFI_BLOCK_THISFUNC(7702);

	GNPCPathFinder* path_finder = m_pOwner->GetPathFinder();
	SetNextWaypointDir();

	vec3 vPos = GetOwnerNPC()->GetPos();
	m_vOldPos = vPos;
	vec3 vResultPos = GTaskUtil::CalcPos(fDelta, GetOwnerNPC()->GetNPCMove().GetSpeed(), vPos, m_vWaypoint, m_vNextDir);
	SetValidateZPos(vResultPos);

	if ( IsReachWaypoint( fDelta) == true)
	{
		if ( IsEndOfWaypoint() == false)
		{
			++m_nMoveIndex;

			SetNextWaypoint();
			RouteMoveNextWayPoint();
		}
	}
}


#define DISTANCE_LIMIT_MIN			50.0f


bool GDefiniteMoveStrategy::FindPath( vec3 vStartPos, vec3& vDestPos)
{
	AI_PFI_BLOCK(APT_ID_PATH_FIND, L"FindPath");	
	PFI_BLOCK_THISFUNC(7703);

	GField* pField = m_pOwner->GetField();
	if (!pField)
		return false;

	// way point 리스트 갱신
	m_vecWaypoints.clear();
	m_nMoveIndex = 0;

	// 패쓰 파인딩
	GNPCPathFinder* path_finder = m_pOwner->GetPathFinder();
	vector<vec3> vecWaypoint;
	if ( !path_finder->FindPath( vStartPos, vDestPos, vecWaypoint))
	{
		if (GConst::LOG_PATHFIND)
		{
			mlog("\t길찾기 실패\n");
		}
		return false;
	}

	vecWaypoint.push_back(vDestPos);

	// 근접한 거리 필터링
	vec3 _pos = vStartPos;
	vector<vec3>::iterator it = vecWaypoint.begin();
	for ( it;  it != vecWaypoint.end();)
	{
		// 거리 비교해서 가까우면 패스
		vec3 diff =  *it - _pos;
		if ( abs(diff.x) < DISTANCE_LIMIT_MIN  &&  abs(diff.y) < DISTANCE_LIMIT_MIN  &&  abs(diff.z) < DISTANCE_LIMIT_MIN)
		{
			// 가까움
			it = vecWaypoint.erase(it);
		}
		else
		{
			_pos = *it;
			++it;
		}
		
	}

	// way point가 비어있으면 최종 좌표로 이동
	if ( vecWaypoint.empty())
	{
		if (GConst::LOG_PATHFIND)
		{
			mlog("\t실패: 이동 경로가 비어있음\n");
		}
		return false;
	}

	// 마지막 way point는 least gap을 적용해서 계산한다
	if ( m_fLeastGap > 1.0f)
	{
		vec3 _dir = vecWaypoint[ vecWaypoint.size() - 1] - ( (vecWaypoint.size() == 1) ? vStartPos : vecWaypoint[ vecWaypoint.size() - 2]);
		float _dist = sqrt( _dir.x * _dir.x + _dir.y * _dir.y + _dir.z * _dir.z);
		GMath::NormalizeZ0( _dir);

		// 거리차가 least gap보다 크면 least gap만큼 빼준다
		if ( _dist > m_fLeastGap)
			vecWaypoint[ vecWaypoint.size() - 1] -= _dir * m_fLeastGap;

		// 거리차가 least gap보다도 작으면 아예 마지막 노드를 빼버린다
		else
		{
			vecWaypoint.pop_back();

			if ( vecWaypoint.empty() == true)
			{
				if (GConst::LOG_PATHFIND)
				{
					mlog("\t길찾기 보정후, 이동할 필요가 없음\n");
				}
				return false;
			}
		}
	}
	
	m_vecWaypoints = vecWaypoint;

	for (size_t i=0; i<m_vecWaypoints.size(); ++i)
	{
		float fHeight = 0.0f;
		m_vecWaypoints[i].z += PICKING_ADDED_VALUE_Z;
		if (PATH_SUCCESS != gsys.pPathFinder->GetHeightAtPosition(&fHeight, pField->GetPathMesh(), m_vecWaypoints[i]))
			continue;

		m_vecWaypoints[i].z = fHeight;
	}

	vDestPos = m_vecWaypoints.back();

	RouteDebugPathInfo();

	return true;
}

bool GDefiniteMoveStrategy::IsReachWaypoint(float fDelta)
{
	return IsReachPos(m_vWaypoint, fDelta);
}

bool GDefiniteMoveStrategy::IsEndOfWaypoint()
{
	return (m_nMoveIndex >= (int)m_vecWaypoints.size()-1);
}

void GDefiniteMoveStrategy::SetNextWaypoint()
{
	m_vWaypoint = m_vecWaypoints[m_nMoveIndex];

	SetNextWaypointDir();

	m_pOwner->SetDir( m_vNextDir);
	m_pOwner->SetFacingDir( m_vNextDir);
	m_vOldPos = vec3::ZERO;
}

void GDefiniteMoveStrategy::RouteDebugPathInfo()
{
	if (!IsRunForTest())
	{
		return;
	}

	if (!GConst::TEST_SHOW_CLIENT_NPC_MOVE_PATH)	return;
	if (!GetOwnerNPC()->GetField())					return;

	vec3* pPathList = new vec3[m_vecWaypoints.size()+1];
	pPathList[0] = GetOwnerNPC()->GetPos();

	for (size_t i=0; i<m_vecWaypoints.size(); i++)
	{
		pPathList[i+1] = m_vecWaypoints[i];
	}

	MUID uidTarget = GetOwnerNPC()->GetUID();

	wchar_t buff[128];
	swprintf_s(buff, 128, L"%u", uidTarget.High);

	MCommand* pNewCommand = MakeNewCommand(MC_DEBUG_STRING, 4, 
		NEW_WSTR(L"MovePathTrace"), 
		NEW_INT(uidTarget.Low), 
		NEW_WSTR(buff), 
		NEW_BLOB(pPathList, sizeof(vec3), static_cast<unsigned short>(m_vecWaypoints.size()+1)));
	GetOwnerNPC()->RouteToField(pNewCommand);

	delete [] pPathList;
}

void GDefiniteMoveStrategy::RouteMoveNextWayPoint()
{
	RouteMove(m_vWaypoint);
}

void GDefiniteMoveStrategy::SetNextWaypointDir()
{
	if ( m_vWaypoint.IsEqual(GetOwnerNPC()->GetPos()))
	{
		m_vNextDir = GetOwnerNPC()->GetDir();
		return;
	}

	vec3 vWayPoint = m_vWaypoint;
	vec3 vOwnerPos = GetOwnerNPC()->GetPos();

	vWayPoint.z = 0.0f;
	vOwnerPos.z = 0.0f;

	if (vWayPoint.DistanceToSq(vOwnerPos) > 0.01f)
	{
		m_vNextDir = m_vWaypoint - GetOwnerNPC()->GetPos();
	}
	else
	{
		m_vNextDir = GetOwnerNPC()->GetDir();
	}

	GMath::NormalizeZ0(m_vNextDir);
}

//////////////////////////////////////////////////////////////////////////
//
//		GStraightMoveStrategy
//

GStraightMoveStrategy::GStraightMoveStrategy( GEntityNPC* pOwner, float fLeastGap )
: GMoveStrategy(pOwner, fLeastGap)
{

}

GStraightMoveStrategy::~GStraightMoveStrategy()
{

}


bool GStraightMoveStrategy::Start(vec3 vGoalPos, vec3 vGoalDir, float fLeastGap)
{
	AI_PFI_BLOCK(APT_ID_STRAIGHT_MOVE, L"StraightMove");

	// 목표 좌표에서 LeastGap만큼 빼줌
	float fDistance = m_pOwner->GetPos().DistanceTo(vGoalPos);
	vGoalPos += min(fDistance,fLeastGap) * -vGoalDir;

	if (!__super::Start(vGoalPos, vGoalDir, fLeastGap))
		return false;

	m_vGoalDir = m_vGoalPos - m_pOwner->GetPos();
	GMath::NormalizeZ0(m_vGoalDir);

	m_pOwner->SetDir( GetGoalDir());
	m_pOwner->SetFacingDir( GetGoalDir());

	RouteMove( GetGoalPos());

	return true;
}

void GStraightMoveStrategy::Update( float fDelta )
{
	PFI_BLOCK_THISFUNC(9085);
	AI_PFI_BLOCK(APT_ID_STRAIGHT_MOVE, L"StraightMove");

	vec3 vOwnerPos, vResultPos;

	vOwnerPos = GetOwnerNPC()->GetPos();

	m_vOldPos = vOwnerPos;

	PFI_B(9086, "GStraightMoveStrategy::Update-GTaskUtil::CalcPos");
	vResultPos = GTaskUtil::CalcPos(fDelta, 
		GetOwnerNPC()->GetNPCMove().GetSpeed(), 
		vOwnerPos, 
		m_vGoalPos, 
		m_vGoalDir);

	PFI_E(9086);

	SetValidateZPos(vResultPos);
}

