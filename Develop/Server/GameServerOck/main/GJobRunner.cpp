#include "StdAfx.h"
#include "GJobRunner.h"
#include "GJob_Broker.h"
#include "GModuleAI.h"
#include "GNPCInfo.h"
#include "GEntityNPC.h"
#include "GTalentInfoMgr.h"
#include "GJob_Talent.h"
#include "GMath.h"
#include "GStrings.h"
#include "GGlobal.h"
#include "GConst.h"
#include "GJobMgr.h"
#include "GBehaviorStateMachine.h"
#include "GAIRoomMgr.h"
#include "GAIMonitorPort.h"
#include "GAIState.h"
#include "GField.h"
#include "GFieldNPCSessionMgr.h"
#include "GFieldNPCSession.h"

GJobRunner::GJobRunner()
{

}

void GJobRunner::PushJob( GModuleAI* pmAI, GJob* pJob )
{
	_ASSERT(pmAI != NULL);
	
	GJobMgr* pJobMgr = pmAI->GetJobMgr();
	pJobMgr->Push(pJob);

	if (GConst::LOG_JOB)
	{
		int nJobCount = pJobMgr->GetCount()+(pJobMgr->IsEmpty()?0:1);
		mlog("[JobLog] <Push_Job -> Queued: %d> IsMissionMode: %d, Owner:%s(%I64d), Job:%s\n", 
			nJobCount, IsNowCombat(pmAI), MLocale::ConvUTF16ToAnsi(pmAI->GetOwnerNPC()->GetName()).c_str(), pmAI->GetOwnerUID(), MLocale::ConvUTF16ToAnsi(pJob->GetJobName()).c_str());
	}
}

void GJobRunner::PushMissionJob( GModuleAI* pmAI, GJob* pJob )
{
	GBehaviorStateMachine* pFSM = pmAI->GetBehaviorFSM();
	pFSM->PushMissionJob(pJob);

	GMessage msg(GMSG_AI_EVT_MISSION);
	pmAI->GetOwner()->Message(msg);

	if (GConst::LOG_JOB)
	{
		int nJobCount = pFSM->GetMissionJobCounter();
		mlog("[JobLog] <Push_MissionJob -> Queued: %d> IsMissionMode: %d, Owner:%s(%I64d), Job:%s\n", 
			nJobCount, IsNowCombat(pmAI), MLocale::ConvUTF16ToAnsi(pmAI->GetOwnerNPC()->GetName()).c_str(), pmAI->GetOwnerUID(), MLocale::ConvUTF16ToAnsi(pJob->GetJobName()).c_str());
	}
}

void GJobRunner::PushJobByMode( GModuleAI* pmAI, GJob* pJob )
{
	VALID(pJob);

	GEntityNPC* pNPC = pmAI->GetOwnerNPC();
	if (!pNPC)
		return;

	if (IsSessionMode(pNPC))
	{
		GFieldNPCSession* pNPCSession = 
			pNPC->GetSession();

		if (pNPCSession)
			pNPCSession->PushJob(pJob);
		
		return;
	}


	if (IsNowCombat(pmAI))
		PushJob( pmAI, pJob );
	else
		PushMissionJob( pmAI, pJob );
		

	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(pmAI->GetOwnerUID()))
	{
		GAIMonitorPort::Inst().OnJobEnqueue(IsNowCombat(pmAI), pJob);
	}
}

void GJobRunner::Move(GModuleAI* pmAI, vec3 vTarPos, vec3 vTarDir, float fLimitSumOfPathLength, unsigned long nMarkerID)
{
	GJob* pJob = GJobBroker::NewJob_MoveToPos(pmAI, vTarPos, vTarDir, fLimitSumOfPathLength, nMarkerID);
	PushJobByMode(pmAI, pJob);
}

void GJobRunner::MoveToMarker(GModuleAI* pmAI, const MARKER_INFO& Marker, float fLimitSumOfPathLength)
{
	Move(pmAI, Marker.vPoint, Marker.vDir, fLimitSumOfPathLength, Marker.nID);
}

void GJobRunner::MoveToSensor(GModuleAI* pmAI, const SENSOR_INFO& Sensor, float fLimitSumOfPathLength)
{
	Move(pmAI, Sensor.vPoint, vec3::ZERO, fLimitSumOfPathLength);
}

bool GJobRunner::UseTalent(GEntityNPC* pThisNPC, GEntityActor* pTargetActor, int nTalentID, USETALENT_PARAM param)
{
	VALID_RET(pThisNPC, false);
	GModuleAI* pmAI = pThisNPC->GetModuleAI();
	VALID_RET(pmAI, false);

	GNPCInfo* pNPCInfo = pmAI->GetOwnerNPC()->GetNPCInfo();
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
	if (pTalentInfo == NULL)	
	{
		dlog("%s - invalid talent (npcid: %d, talentid: %d)\n", __FUNCTION__, pThisNPC->GetID(), nTalentID);
		return	false;
	}
	if (pTalentInfo->m_setNPCID.find(pNPCInfo->nID) == pTalentInfo->m_setNPCID.end()) 
	{
		if (IsRunForTest())
		{
			dlog("NPC가 사용할 수 없는 탤런트입니다. (NPCID: %d, TalentID: %d)\n", pNPCInfo->nID, nTalentID);
		}

		return false;
	}

	GJob_Talent* pJobTalent = static_cast<GJob_Talent*>(GJobBroker::NewJob_UseTalent(pmAI, pTalentInfo, pTargetActor?pTargetActor->GetUID():MUID::Invalid()));
	pJobTalent->Attr.bClearJobListWhenCanceled = true;
		
	if (pJobTalent->GetID() == GJOB_TALENT)
	{
		GJob_UseTalent* pJobUseTalent = static_cast<GJob_UseTalent*>(pJobTalent);

		// 회전할지 여부 설정
		if (pTalentInfo->IsProjectileType())
			pJobUseTalent->SetRotatable(true);
		else
			pJobUseTalent->SetRotatable(param.bRotatable);
	}
	
	if (pTargetActor)
	{
		if (param.bCheckValidate)
		{
			if (!pThisNPC->CheckTalentRange(pTargetActor, nTalentID))
			{
				MoveToTargetCloser(pmAI, pTargetActor, pJobTalent->GetTalentID(), param.fChaseLimitTime);
			}
		}
		else
		{
			// 목표로부터 원격 공격이 가능한지 체크
			if (pTalentInfo->IsProjectileType())
			{
				vec3 vEndPos = pTargetActor->GetPos();
				vEndPos.z += pTargetActor->GetColRadius(); // 충돌구 만큼 높이를 올려줌
				if (!gmgr.pAIRoomMgr->IsValid(pThisNPC->GetGroupID(), vEndPos))
				{
					return false;
				}
			}
		}
	}

	pJobTalent->SetGainStress(param.bGainStress);
	PushJobByMode(pmAI, pJobTalent);

	return true;
}

void GJobRunner::MoveToEntity( GModuleAI* pmAI, GEntityActor* pActor, float fLeastDistance, bool bInfinity, float fDurationTime )
{
	_ASSERT(pActor);
	if (!pActor)	return;

	GJob* pJob = GJobBroker::NewJob_MoveToEntity(pmAI, pActor->GetUID(), fLeastDistance, bInfinity);
	pJob->SetDurationTime(fDurationTime);

	PushJobByMode(pmAI, pJob);
}

void GJobRunner::Follow( GModuleAI* pmAI, GEntityActor* pActor, float fLeastDistance, float fDurationTime )
{
	_ASSERT(pActor);
	if (!pActor)	return;

	fLeastDistance += pActor->GetColRadius();

	GJob* pNewJob = GJobBroker::NewJob_MoveToEntity(pmAI, pActor->GetUID(), fLeastDistance, false);
	pNewJob->SetDurationTime(fDurationTime);

	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::Idle( GModuleAI* pmAI, float fIdleTime )
{
	GJob* pNewJob = GJobBroker::NewJob_Idle(pmAI, fIdleTime);
	PushJobByMode(pmAI, pNewJob);
}

bool GJobRunner::MoveToTargetCloser( GModuleAI* pmAI, GEntityActor* pTargetActor, int nTalentID, float fChaseLimitTime )
{
	if (!pTargetActor)		return false;

	GNPCInfo* pNPCInfo = pmAI->GetOwnerNPC()->GetNPCInfo();
	if (pNPCInfo->bRooted) return false;

	float fThisNPCRadius = pmAI->GetOwnerNPC()->GetColRadius();
	float fLeastGap = 0;
	if (INVALID_TALENT_ID != nTalentID)
	{
		GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
		if (NULL == pTalentInfo)
		{
			_ASSERT(pTalentInfo);	
			return false;
		}

		if (pTalentInfo->m_nSkillType == ST_ARCHERY ||
			pTalentInfo->m_nSkillType == ST_MAGIC)
		{
			fLeastGap  = pTalentInfo->m_fRange;
		}
		else
		{
			// 딱 사정거리 만큼만 멀리 떨어져서 싸우면 이상해서 좀더 붙인다.
			float fAttackRange = pTalentInfo->GetDistanceToLongestPoint();
			float fMinRadiusRange = (pTargetActor->GetColRadius()+fThisNPCRadius) * 0.8f;
			
			fLeastGap = max(fAttackRange, fMinRadiusRange);
			fLeastGap = max(0.0f, fLeastGap);
		}
	}
	else
	{
		fLeastGap = pTargetActor->GetColRadius()+fThisNPCRadius;
	}


	GJob* pNewJob = GJobBroker::NewJob_MoveToEntity(pmAI, pTargetActor->GetUID(), fLeastGap, false);
	pNewJob->Attr.bClearJobListWhenTimedout = true;
	pNewJob->SetDurationTime(fChaseLimitTime);

	PushJobByMode(pmAI, pNewJob);
	return true;
}

void GJobRunner::FleeFromEntity( GModuleAI* pmAI, MUID uidTarget )
{
	GJob* pNewJob = GJobBroker::NewJob_FleeFromEntity(pmAI, uidTarget);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::FleeToHome( GModuleAI* pmAI )
{
	GJob* pNewJob = GJobBroker::NewJob_FleeToHome(pmAI);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::Roam( GModuleAI* pmAI, float fRadius, float fDurationTime/*=0.0f*/ )
{
	PFI_BLOCK_THISFUNC(9070);
	GJob* pNewJob = GJobBroker::NewJob_Roam(pmAI, fRadius, fDurationTime);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::Patrol( GModuleAI* pmAI, MARKER_LIST MarkerList, int nPatrolType )
{
	GJob* pNewJob = GJobBroker::NewJob_Patrol(pmAI, MarkerList, (PATROL_TYPE)nPatrolType);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::Patrol( GModuleAI* pmAI, const vector<MARKER_INFO>& MarkerList, int nPatrolType )
{
	GJob* pNewJob = GJobBroker::NewJob_Patrol(pmAI, MarkerList, (PATROL_TYPE)nPatrolType);
	PushJobByMode(pmAI, pNewJob);
}


void GJobRunner::Speak( GModuleAI* pmAI, const wchar_t* pszSpeakType, wstring strText, float fMaintainTime)
{	
	if (fMaintainTime == -1.0f)
	{
		// 말하는 시간이 명시적으로 지정되지 않은 경우
		fMaintainTime = GStrings::CalcSpeakMaintainTime(strText.c_str());
	}
	
	GJob* pNewJob = GJobBroker::NewJob_Speak(pmAI, pszSpeakType, strText, fMaintainTime);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::RunScript( GModuleAI* pmAI, MUID uidOpponent, wstring strTable, wstring strFunc )
{
	GJob* pNewJob = GJobBroker::NewJob_Script(pmAI, uidOpponent, strTable, strFunc);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::Guard( GModuleAI* pmAI, int nGuardTalentID, float fGuardingTime )
{
	GJob* pNewJob = GJobBroker::NewJob_Guard(pmAI, nGuardTalentID, fGuardingTime);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::Flee( GModuleAI* pmAI, FleeType nFleeType, float fDurationTime )
{
	GJob* pNewJob = GJobBroker::NewJob_Flee(pmAI, nFleeType, fDurationTime);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::Yell( GModuleAI* pmAI, int nRange )
{
	GJob* pNewJob = GJobBroker::NewJob_Yell(pmAI, nRange);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::ClearJob(GModuleAI* pmAI)
{
	VALID(pmAI);

	GJobMgr* pJobMgr = pmAI->GetJobMgr();
	pJobMgr->Clear(true);

	// INFO(Pyo): ClearMissionJob이 필요하면 따로 만들기
}

void GJobRunner::Warp(GModuleAI* pmAI, vec3 vecPos, vec3 vecDir, bool bRouteToMe)
{
	GJob* pNewJob = GJobBroker::NewJob_Warp(pmAI, vecPos, vecDir, bRouteToMe);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::GainBuff(GModuleAI* pmAI, int nBuffID)
{
	GJob* pNewJob = GJobBroker::NewJob_GainBuff(pmAI, nBuffID);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::Despawn(GModuleAI* pmAI, bool bSpawnEnable)
{
	GJob* pNewJob = GJobBroker::NewJob_Despawn(pmAI, bSpawnEnable);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::Die(GModuleAI* pmAI, MUID uidTarget)
{
	GJob* pNewJob = GJobBroker::NewJob_Die(pmAI, uidTarget);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::Assist(GModuleAI* pmAI, MUID uidTarget)
{
	GJob* pNewJob = GJobBroker::NewJob_Assist(pmAI, uidTarget);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::Attack(GModuleAI* pmAI, MUID uidTarget, int nDamage)
{
	GJob* pNewJob = GJobBroker::NewJob_Attack(pmAI, uidTarget, nDamage);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::BreakPart(GModuleAI* pmAI, uint8 nPartID, MUID uidOpponent)
{
	GJob* pNewJob = GJobBroker::NewJob_BreakPart(pmAI, nPartID, uidOpponent);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::FaceTo(GModuleAI* pmAI, MUID uidTarget)
{
	GJob* pNewJob = GJobBroker::NewJob_FaceTo(pmAI, uidTarget);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::WaitFor(GModuleAI* pmAI, GEntityNPC* pTarget)
{
	VALID(pTarget);
	GJob* pNewJob = GJobBroker::NewJob_WaitFor(pmAI, pTarget->GetUID());
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::NextAction( GModuleAI* pmAI )
{
	GJob* pNewJob = GJobBroker::NewJob_NextAction(pmAI);
	PushJobByMode(pmAI, pNewJob);
}

void GJobRunner::Summon( GModuleAI* pmAI, int nNPCID, vec3 vStartPos )
{
	GJob* pNewJob = GJobBroker::NewJob_Summon(pmAI, nNPCID, vStartPos);
	PushJobByMode(pmAI, pNewJob);
}

bool GJobRunner::IsNowCombat( GModuleAI* pmAI )
{
	VALID_RET(pmAI, false);
	VALID_RET(pmAI->GetOwnerNPC(), false);

	return pmAI->GetOwnerNPC()->IsNowCombat();
}

bool GJobRunner::IsPeaceBehaviorMode( GModuleAI* pmAI )
{
	VALID_RET(pmAI, false);
	return pmAI->GetBehaviorFSM()->GetCurrentStateID() == AI_BEHAVIOR_PEACE;
}

bool GJobRunner::IsSessionMode( GEntityNPC* pNPC )
{
	if (!pNPC)
		return false;
	if (!pNPC->HasSession())
		return false;

	if (pNPC->IsNowCombat())
		return false;

	return true;
}
