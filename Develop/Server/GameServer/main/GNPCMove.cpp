#include "StdAfx.h"
#include "GNPCMove.h"
#include "GEntityNPC.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GConst.h"

GNPCMove::GNPCMove(GEntityNPC* pOwner)
: m_pOwner(pOwner)
, m_bAlwaysRun(false)
, m_bNowRetreat(false)
, m_bNowReturnToHome(false)
{
}

GNPCMove::~GNPCMove(void)
{
}

void GNPCMove::SetAlwaysRun(bool bRun)
{
	m_bAlwaysRun = bRun;
}

bool GNPCMove::IsRun() const
{
	if (m_bAlwaysRun)
		return true;

	if (m_bNowReturnToHome)
		return true;

	return m_pOwner->IsNowCombat();
}

float GNPCMove::GetRunSpeed() const
{
	return m_pOwner->GetNPCInfo()->fRunSpeed + (m_pOwner->GetNPCInfo()->fRunSpeed * m_pOwner->GetChrStatus()->ActorModifier.fMoveSpeed);
}

float GNPCMove::GetFleeSpeed() const
{
	return GetRunSpeed() * m_pOwner->GetNPCInfo()->fFleeSpeedAmp;
}

float GNPCMove::GetSpeed() const
{
	if (m_bNowRetreat)
	{
		return GetFleeSpeed();
	}

	return (IsRun())?GetRunSpeed():m_pOwner->GetWalkSpeed();
}

void GNPCMove::RouteMove(vec3& tarPos)
{
	// 주위에 메시지를 보낸다.
	TD_NPC_MOVE td_npc_move;
	td_npc_move.nUIID = m_pOwner->GetUIID();
	td_npc_move.vTarPos = tarPos;

	_ASSERT(GetSpeed() == 0.0f || ( GetSpeed() > 0.99f && GetSpeed() < USHRT_MAX) );

	td_npc_move.svDir = m_pOwner->GetDir();
	td_npc_move.nSpeed = (uint16)GetSpeed();

	if (IsRunForTest())
	{
		SYSTEMTIME stCurTime;
		GetLocalTime(&stCurTime);
		char szTime[256] = "";
		sprintf_s(szTime, "[%02d:%02d:%02d.%03d]:"
			, stCurTime.wHour
			, stCurTime.wMinute
			, stCurTime.wSecond
			, stCurTime.wMilliseconds);

		mmlog("[%s] GEntityNPC::RouteMove\n", szTime);
	}

	// 플레이어에게 이동 정보를 보내준다.
	MCommand* pNewCommand = 
		MakeNewCommand(IsRun()?MC_NPC_RUN:MC_NPC_MOVE, 1, NEW_SINGLE_BLOB(&td_npc_move, sizeof(TD_NPC_MOVE)));

	m_pOwner->RouteToThisCell(pNewCommand);
}

void GNPCMove::RouteMoveStop()
{
	if (IsRunForTest())
	{
		SYSTEMTIME stCurTime;
		GetLocalTime(&stCurTime);
		char szTime[256] = "";
		sprintf_s(szTime, "[%02d:%02d:%02d.%03d]:"
			, stCurTime.wHour
			, stCurTime.wMinute
			, stCurTime.wSecond
			, stCurTime.wMilliseconds);
		mmlog("[%s] GEntityNPC::RouteMoveStop\n", szTime);
	}

	MCommand* pNewCommand = 
		MakeNewCommand(MC_NPC_MOVE_STOP, 2, NEW_USHORT(m_pOwner->GetUIID()), NEW_VEC(m_pOwner->GetPos()));

	m_pOwner->RouteToThisCell(pNewCommand);
}

void GNPCMove::MakeTD( TD_UPDATE_CACHE_NPC& out ) const
{
	if (IsRun())
	{
		SetBitSet(out.nStatusFlag, UNS_RUN);
	}
	else
	{
		SetBitSet(out.nStatusFlag, UNS_WALK);
	}
}

void GNPCMove::SetRetreat( bool var )
{
	m_bNowRetreat = var;
}

void GNPCMove::SetReturnToMove( bool var )
{
	m_bNowReturnToHome = var;
}