#include "StdAfx.h"
#include "GJob_MoveToPos_Air.h"
#include "GEntityNPC.h"

GJob_MoveToPos_Air::GJob_MoveToPos_Air(GModuleAI* pmAI, vec3 vTarPos, float fSpeed, bool bRun, bool bNowRetreat, float fLimitSumOfPathLength)
: GJob_Movement(pmAI)
, m_vFinalTarPos(vTarPos)
, m_fSpeed(fSpeed)
, m_fLimitSumOfPathLength(fLimitSumOfPathLength)
, m_bRun(bRun)
, m_bPathFindFailed(false)
, m_bNowRetreat(bNowRetreat)
{
	if (true == bNowRetreat && ETID_NPC == GetOwnerNPC()->GetTypeID())
	{
		GetOwnerNPC()->GetNPCStatus().bNowRetreat = true;
	}
}

GJob_MoveToPos_Air::~GJob_MoveToPos_Air(void)
{
	if (ETID_NPC == GetOwnerNPC()->GetTypeID())
	{
		GetOwnerNPC()->GetNPCStatus().bNowRetreat = false;
	}
}

bool GJob_MoveToPos_Air::OnStart()
{
	GNPCInfo* pNPCInfo = GetOwnerNPC()->GetNPCInfo();
	if (NULL == pNPCInfo)
	{
		return false;
		_ASSERT(0);
	}

	int nNPCOffSetTalentID = 200000;
	int nTakeOffTalentID = nNPCOffSetTalentID + (pNPCInfo->nID * 10) + 9;
	int nLandingTalentID = nNPCOffSetTalentID + (pNPCInfo->nID * 10) + 10;
	
	GTalentInfo* pTakeOffTalent = GTalentInfoMgr::GetInstance().Get(nTakeOffTalentID);
	GTalentInfo* pLandingTalent = GTalentInfoMgr::GetInstance().Get(nLandingTalentID);

	m_TaskBroker.PushTask_UseTalent(GetOwnerNPC(), pTakeOffTalent);
	m_TaskBroker.PushTask_Move(GetOwnerNPC(), m_vFinalTarPos, m_fSpeed, m_bRun);
	m_TaskBroker.PushTask_UseTalent(GetOwnerNPC(), pLandingTalent);

	return true;
}

GJobResult GJob_MoveToPos_Air::OnRun(float fDelta)
{
	return JR_RUNNING;
}

bool GJob_MoveToPos_Air::OnComplete()
{
	return true;
}

bool GJob_MoveToPos_Air::OnCancel()
{
	return true;
}

void GJob_MoveToPos_Air::OnSuspend()
{

}

void GJob_MoveToPos_Air::OnResume()
{

}


