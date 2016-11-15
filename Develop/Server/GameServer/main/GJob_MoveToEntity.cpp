#include "stdafx.h"
#include "GJob_MoveToEntity.h"
#include "GField.h"
#include "GEntityNPC.h"
#include "GModuleAI.h"
#include "GPathFinder.h"
#include "GConst.h"
#include "GTask_Move.h"
#include "GMath.h"
#include "GTargetObserveRotater.h"
#include "GNPCMove.h"

GJob_MoveToEntity::GJob_MoveToEntity(GModuleAI* pmAI, MUID uidTargetEntity, float fLeastGap,bool bSetInifity )
: GJob(pmAI)
, m_uidTargetEntity(uidTargetEntity)
, m_bInifity(bSetInifity)
, m_fLeastGap(fLeastGap)
{
	const float FIND_PATH_INITIAL_TICK = 1.0f;
	m_FindPathRegulator.SetTickTime(FIND_PATH_INITIAL_TICK);	//	기본은 1초
	m_FindPathRegulator.Reset(true);
}

GJob_MoveToEntity::~GJob_MoveToEntity()
{

}

bool GJob_MoveToEntity::OnStart()
{
	PFI_BLOCK_THISFUNC(1904);
	vec3 vCurTarEntityPos;
	if (GetTargetEntityPos(vCurTarEntityPos) == false) return false;

	if (IsArriveTarEntity(vCurTarEntityPos, DEFAULT_GAME_TICK) && false == m_bInifity)
	{
		return false;
	}

	if (!MoveToEntity())
	{
		return false;
	}

	return true;
}


GJobResult GJob_MoveToEntity::OnRun( float fDelta )
{
	PFI_BLOCK_THISFUNC(1701);

	// delta 제한.
	const float MAX_MOVING_DELTATIME = 1.0f;
	if (fDelta > MAX_MOVING_DELTATIME) fDelta = MAX_MOVING_DELTATIME;

	if (m_pRunningTask &&
		m_pRunningTask->GetID() != GTASK_MOVE)
	{
		return JR_RUNNING;
	}

	// 매틱 호출되야 함 (목표가 죽었는지 체크)
	vec3 vCurTarEntityPos;
	if (GetTargetEntityPos(vCurTarEntityPos) == false) 
		return JR_CANCELED;

	if (!m_bInifity && m_pRunningTask == NULL)
	{
		return JR_COMPLETED;
	}

	if (CheckFindPathTick(vCurTarEntityPos, fDelta))
	{
		if (!IsTargetMoved(vCurTarEntityPos))
		{
			return JR_RUNNING;
		}

		if (MoveToEntity() == false)
		{
			return JR_CANCELED;
		}
	}

	if (m_bInifity && m_pRunningTask == NULL)
	{
		MoveToEntity();
		return JR_RUNNING;
	}

	if (m_pRunningTask == NULL)
	{
		return JR_COMPLETED;
	}

	return JR_RUNNING;
}

void GJob_MoveToEntity::OnTaskComplete(GTaskID nTaskID)
{
	PFI_BLOCK_THISFUNC(9074);

	if (m_bInifity ||				// 무한 추적중이라면 반복
		nTaskID == GTASK_TALENT)	// 회전중이라면 반복
	{
		MoveToEntity();
	}
}

bool GJob_MoveToEntity::MoveToEntity()
{
	if (GetTargetEntityPos(m_vLastTarEntityPos) == false) return false;
	if (GetTargetEntityDir(m_vLastTarEntityDir) == false) return false;
	GEntityNPC* pOwnerNPC = GetOwnerNPC();
	GModuleAI* pmAI = m_pOwner;
	if (NULL == pmAI) return false;

	// 회전 탤런트 사용이 필요한 경우
	GTargetObserveRotater rotater;
	if (rotater.IsNeedRotate(pOwnerNPC))
	{
		GTask* pMoveTask = rotater.GetRotateTask(pOwnerNPC);
		if (pMoveTask)
		{
			SetRunningTask(pMoveTask);
			return true;
		}
	}

	// 이동 태스크 설정
	if (AT_SKY == pOwnerNPC->GetNPCInfo()->nAltitude)
	{
		SetRunningTask(m_TaskBroker.New_MoveStraight(pOwnerNPC, m_vLastTarEntityPos, m_fLeastGap));
	}
	else
	{
		SetRunningTask(m_TaskBroker.New_MoveDefinite(pOwnerNPC, m_vLastTarEntityPos, m_fLeastGap));
	}

	return true;
}

GEntityActor* GJob_MoveToEntity::GetTargetEntity()
{
	GEntityNPC* pOwnerNPC = GetOwnerNPC();
	GField* pField = pOwnerNPC->GetField();
	if (!pField)			return NULL;

	return pField->FindActor(m_uidTargetEntity);
}


bool GJob_MoveToEntity::GetTargetEntityPos(vec3& voutTarPos)
{
	GEntityActor* pTargetEntity = GetTargetEntity();
	if (!pTargetEntity)		
		return false;
	if (pTargetEntity->IsDie())	
		return false;
	if (pTargetEntity->IsDead())	
		return false;

	voutTarPos = pTargetEntity->GetPos();

	return true;
}

bool GJob_MoveToEntity::GetTargetEntityDir(vec3& voutTarDir)
{
	GEntityActor* pTargetEntity = GetTargetEntity();
	if (!pTargetEntity)		return false;

	voutTarDir = pTargetEntity->GetPos() - m_pOwner->GetOwnerNPC()->GetPos();
	GMath::NormalizeZ0(voutTarDir);

	return true;
}

void GJob_MoveToEntity::OnCancel()
{
//	GetOwnerNPC()->RouteMoveStop();
}

void GJob_MoveToEntity::OnResume()
{
	__super::OnResume();

	MoveToEntity();
}

bool GJob_MoveToEntity::CheckFindPathTick(vec3 vCurTarEntityPos, float fDelta)
{
	if (m_FindPathRegulator.IsReady(fDelta) == false) 
		return false;

	GEntityNPC* pOwnerNPC = GetOwnerNPC();
	float fDistance = pOwnerNPC->GetPos().DistanceTo(vCurTarEntityPos);
	float fTick = 1.0f;
	if (fDistance < GConst::NPC_FINDPATH_RADIUS_LEVEL1)
	{
		fTick = GConst::NPC_FINDPATH_TICK_LEVEL1;
	}
	else if (fDistance < GConst::NPC_FINDPATH_RADIUS_LEVEL2)
	{
		fTick = GConst::NPC_FINDPATH_TICK_LEVEL2;
	}
	else if (fDistance < GConst::NPC_FINDPATH_RADIUS_LEVEL3)
	{
		fTick = GConst::NPC_FINDPATH_TICK_LEVEL3;
	}
	else
	{
		fTick = GConst::NPC_FINDPATH_TICK_OTHER;
	}

	float fMoveSpeed = GetOwnerNPC()->GetNPCMove().GetSpeed();
	if (fMoveSpeed > 0.0f)
		m_FindPathRegulator.SetTickTime(fTick * (fMoveSpeed/GConst::NPC_FINDPATH_MOVESPEED_FACTOR));
	else
		m_FindPathRegulator.SetTickTime(fTick);

	return true;
}

bool GJob_MoveToEntity::IsArriveTarEntity(const vec3& vTarEntityPos, float fDelta)
{	
	GEntityNPC* pOwnerNPC = GetOwnerNPC();	
	
	vec3 vTarDir = vTarEntityPos - pOwnerNPC->GetPos();
	GMath::NormalizeZ0(vTarDir);

	//if (abs(pOwnerNPC->GetPos().z - vTarEntityPos.z) > 120.0f)
	//{
	//	return false;
	//}

	if (!GMath::IsEqual2DPos(pOwnerNPC->GetPos(), vTarEntityPos, m_fLeastGap))
		return false;

	float fTolerRad = pOwnerNPC->GetRotateSpeed()==0?360:pOwnerNPC->GetRotateSpeed(); // 회전값이 0이면 즉시회전
	if (!GMath::IsEqualDirection(pOwnerNPC->GetDir(), vTarDir, fDelta * fTolerRad))
		return false;

	return true;
}

void GJob_MoveToEntity::OnComplete()
{
//	GetOwnerNPC()->RouteMoveStop();
}

void GJob_MoveToEntity::OnTimeout()
{
//	GetOwnerNPC()->RouteMoveStop();
}

bool GJob_MoveToEntity::IsTargetMoved( vec3 vCurTarEntityPos )
{
#define TARGETPOS_GAP	100.0f
	return m_vLastTarEntityPos.DistanceTo(vCurTarEntityPos) > TARGETPOS_GAP;
}

string GJob_MoveToEntity::GetParamString() const
{
	string strTargetName = "unknown";
	GEntityActor* pTarget = GetOwnerNPC()->FindActor(m_uidTargetEntity);
	if (pTarget)	
		strTargetName = MLocale::ConvUTF16ToAnsi(pTarget->GetName());

	char buff[512];
	sprintf(buff, "Target: %s, Infinity: %s, LeastGap: %.1f, LastTarPos: %.1f %.1f %.1f", 
		strTargetName.c_str(),
		m_bInifity?"true":"false",
		m_fLeastGap,
		m_vLastTarEntityPos.x, m_vLastTarEntityPos.y, m_vLastTarEntityPos.z);
	return buff;
}
