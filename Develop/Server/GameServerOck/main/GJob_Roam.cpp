#include "stdafx.h"
#include "GJob_Roam.h"
#include "GEntitySync.h"
#include "GEntityNPC.h"
#include "GModuleAI.h"
#include "GField.h"
#include "GTask_Move.h"
#include "GNPCPathFinder.h"
#include "GColtMgr.h"
#include "GConst.h"
#include "GTestSystem.h"
#include "GGlobal.h"

GJob_Roam::GJob_Roam(GModuleAI* pmAI, float fRadius, float fDurationTime)
: GJob(pmAI)
, m_fRadius(fRadius)
, m_fPickDepth(PICKING_ADDED_VALUE_Z)
, m_nPhase(PHASE_ROAM)
{
	SetDurationTime(fDurationTime);
}

GJob_Roam::~GJob_Roam()
{

}

bool GJob_Roam::OnStart()
{
	PFI_BLOCK_THISFUNC(1907);
	if (m_fRadius <= 0.0f) return false;
		
	const SPAWN_INFO* pSpawnInfo = GetOwnerNPC()->GetSpawnInfo();
	
	if (pSpawnInfo)
	{
		m_vCenter = pSpawnInfo->vPoint;
		m_fPickDepth = pSpawnInfo->fRoamDepth;		
	}
	else
	{		
		// Lua, GM command를 통한 spawn시, pSpawnInfo는 NULL일 수 있다 - praptor
		m_vCenter = GetOwnerNPC()->GetSpawnPos();
	}
	
	OnRoaming();

	return true;
}

GJobResult GJob_Roam::OnRun(float fDelta)
{
	return JR_RUNNING;
}

void GJob_Roam::OnComplete()
{
	GetOwnerNPC()->RouteMoveStop();
}

void GJob_Roam::OnTaskComplete( GTaskID nTaskID )
{
	PFI_BLOCK_THISFUNC(9077);

	switch (m_nPhase)
	{
	case PHASE_ROAM:		{ OnIdleColt(); }break;
	case PHASE_IDLE_COLT:	{ OnIdle(); 	}break;
	case PHASE_IDLE_TASK:	{ OnRoaming();	}break;
	}
}


void GJob_Roam::OnCancel()
{

}

void GJob_Roam::OnIdleColt()
{
	PFI_BLOCK_THISFUNC(9082);
	m_nPhase = PHASE_IDLE_COLT;

	GModuleAI* pModuleAI = m_pOwner;
	GTask* pNextTask = pModuleAI->GetColt()->RunColtIdleForJob(GetOwnerNPC());

	if (pNextTask)
	{
		SetRunningTask(pNextTask);
	}
	else
	{
		OnIdle();
	}
}

void GJob_Roam::OnIdle()
{
	PFI_BLOCK_THISFUNC(9083);
	m_nPhase = PHASE_IDLE_TASK;

	GTaskBroker task_broker;
	GTask* pNextTask = task_broker.New_Idle(GetOwnerNPC(), GConst::ROAM_DEFAULT_IDLE_TIME);
	SetRunningTask(pNextTask);
}

void GJob_Roam::OnRoaming()
{
	PFI_BLOCK_THISFUNC(9084);

	m_nPhase = PHASE_ROAM;

	GEntityNPC* pOwnerNPC = GetOwnerNPC();

	VALID(pOwnerNPC);
	VALID(pOwnerNPC->GetNPCInfo());
	VALID(pOwnerNPC->GetField());
	VALID(pOwnerNPC->GetField()->GetInfo());

	const GFieldInfo* pFieldInfo = pOwnerNPC->GetField()->GetInfo();

	const int MAX_TRY_COUNT = 3;
	int nTriedCounter = MAX_TRY_COUNT;

	vec3 vDestPos;
	float fRadius;
	float fDistance;
	MVector2 vAdjDestPos;

	PFI_B(9088, "GJob_Roam::PickingZ");
	do 
	{
		fRadius = RandomNumber(0.0f, MMath::PI*2.0f);
		fDistance = RandomNumber(0.0f, m_fRadius);
		vAdjDestPos = MMath::GetArcPosition(fRadius, fDistance);

		vDestPos.x = m_vCenter.x + vAdjDestPos.x;
		vDestPos.y = m_vCenter.y + vAdjDestPos.y;
		vDestPos.z = pOwnerNPC->GetPos().z;

		if (pOwnerNPC->GetField()->PickingZ(vDestPos, vDestPos, max(PICKING_ADDED_VALUE_Z, m_fPickDepth)))
		{
			break;
		}

		gsys.pTestSystem->PickingLog(GetOwnerNPC()->GetUID(), L"pick_fail: GJob_Roam::OnRoaming");

	} while (nTriedCounter-- > 0);
	PFI_E(9088);

	if (nTriedCounter <= 0)
	{
		return;
	}

	PFI_B(9089, "GJob_Roam::OnRoaming-GTaskBroker::New_MoveStraight");
	GTask* pNewTask = m_TaskBroker.New_MoveStraight(GetOwnerNPC(), vDestPos, 0.0f);
	PFI_E(9089);

	PFI_B(9093, "GJob_Roam::OnRoaming-GJob::SetRunningTask");
	SetRunningTask(pNewTask);	
	PFI_E(9093);
}

string GJob_Roam::GetParamString() const
{
	char buff[512];
	sprintf(buff, "Center: %.1f %.1f %.1f, Radius = %.1f, Depth = %.1f, Phase = %d", 
		m_vCenter.x, m_vCenter.y, m_vCenter.z, m_fRadius, m_fPickDepth, m_nPhase);
	return buff;
}

