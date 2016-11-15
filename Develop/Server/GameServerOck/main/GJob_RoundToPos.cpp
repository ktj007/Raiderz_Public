#include "StdAfx.h"
#include "GJob_RoundToPos.h"
//#include "GEntityNPC.h"
//
//GJob_RoundToPos::GJob_RoundToPos(GModuleAI* pmAI, vec3 vStartPos, vec3 vCenterPos, float fRadian)
//: GJob(pmAI)
//, m_vStartPos(vStartPos)
//, m_vCenterPos(vCenterPos)
//, m_fRadian(fRadian)
//{
//}
//
//GJob_RoundToPos::~GJob_RoundToPos(void)
//{
//}
//
//bool GJob_RoundToPos::OnStart()
//{
//	GNPCInfo* pNPCInfo = GetOwnerNPC()->GetNPCInfo();
//	if (NULL == pNPCInfo)
//	{
//		return false;
//		_ASSERT(0);
//	}
//
//	{
//		SetRunningTask(m_TaskBroker.New_MoveStraight(GetOwnerNPC(), m_vStartPos, 0.0f));
//		SetRunningTask(m_TaskBroker.New_Round(GetOwnerNPC(), m_vCenterPos, m_fRadian));
//		return true;
//
//	}
//
//	if (AT_SKY == GetOwnerNPC()->GetNPCInfo()->nAltitude)
//	{
//		SetRunningTask(m_TaskBroker.New_MoveStraight(GetOwnerNPC(), m_vStartPos, 0.0f));
//		SetRunningTask(m_TaskBroker.New_Round(GetOwnerNPC(), m_vCenterPos, m_fRadian));
//	}
//	else
//	{
//		if (!GetOwnerNPC()->GetPos().IsEqual(m_vStartPos))
//			SetRunningTask(m_TaskBroker.New_MoveGradual(GetOwnerNPC(), m_vStartPos, 0.0f));
//
//		SetRunningTask(m_TaskBroker.New_Round(GetOwnerNPC(), m_vCenterPos, m_fRadian));
//		// TODO: 현제 날때와 같은 길찾기를 하지 않는 경우만 사용 가능
//	}
//
//	return true;
//}
//
//GJobResult GJob_RoundToPos::OnRun( float fDelta )
//{
//	return JR_RUNNING;
//}
//
//void GJob_RoundToPos::OnComplete()
//{
//	GetOwnerNPC()->RouteMoveStop();
//}
//
//void GJob_RoundToPos::OnCancel()
//{
//	GetOwnerNPC()->RouteMoveStop();
//}
