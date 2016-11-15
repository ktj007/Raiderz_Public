#include "stdafx.h"
#include "GTask_Move.h"
#include "GEntityNPC.h"
#include "GModuleMovable.h"

GTask_Move::GTask_Move(GEntityNPC* pEntity, vec3 vTargetPos, float fLeastGap, GTaskMoveStrategy nStrategyType) 
: GTask(pEntity)
, m_vGoalPos(vTargetPos)
, m_fLeastGap(fLeastGap)
, m_nWay(MW_STRAIGHT)
, m_nChangeWayCounter(0)
, m_nStrategyType(nStrategyType)
, m_vFinishDir(vec3::ZERO)
{
}

GTask_Move::~GTask_Move()
{
}

bool GTask_Move::OnStart()
{
	GetMovable()->Move(m_vGoalPos, m_fLeastGap, m_nStrategyType);
	GetMovable()->SetFinishDir(m_vFinishDir);
	return GetMovable()->Start();
}

GTaskResult GTask_Move::OnRun(float fDelta)
{	
	// ¿Ï·á
	if (GetMovable()->IsMoveFinish())
	{
		//mlog("complete - %.1f %.1f %.1f\n", m_vGoalPos.x, m_vGoalPos.y, m_vGoalPos.z);
		return TR_COMPLETED;		
	}

	return TR_RUNNING;
}

void GTask_Move::OnComplete()
{
	GetMovable()->Complete();
}

void GTask_Move::OnCancel()
{
	GetMovable()->Cancel();
}

void GTask_Move::OnSuspend()
{
	GetMovable()->Suspend();
}

void GTask_Move::OnResume()
{
	GetMovable()->Resume();
}

bool GTask_Move::Reset( GTask* pNewTask )
{
	GTask_Move* pNewMoveTask = static_cast<GTask_Move*>(pNewTask);
	m_vGoalPos = pNewMoveTask->m_vGoalPos;
	m_fLeastGap = pNewMoveTask->m_fLeastGap;
	m_nWay = pNewMoveTask->m_nWay;
	m_nChangeWayCounter = pNewMoveTask->m_nChangeWayCounter;
//	mlog("GTask_Move::Reset - %.1f %.1f %.1f\n", m_vGoalPos.x, m_vGoalPos.y, m_vGoalPos.z);
	GetMovable()->Reset(pNewTask);
	
	return GTask::Reset(pNewTask);
}

const wchar_t* GTask_Move::GetTaskName()
{
	return GetMovable()->GetTaskName();
}

GModuleMovable* GTask_Move::GetMovable() const
{
	return m_pOwner->GetModuleMovable();
}

void GTask_Move::SetFinishDir( vec3 vFinishDir )
{
	m_vFinishDir = vFinishDir;
}