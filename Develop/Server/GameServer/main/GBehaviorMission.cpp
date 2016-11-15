#include "stdafx.h"
#include "GBehaviorMission.h"
#include "GModuleAI.h"
#include "GEntityNPC.h"
#include "GJob_Patrol.h"
#include "GMessageID.h"
#include "GAIProfiler.h"
#include "GJobMgr.h"
#include "GScriptManager.h"
#include "GScriptGroup_NPC.h"
#include "GNPCAssist.h"
#include "GConst.h"

GBehaviorMission::GBehaviorMission( GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM ) 
: GBehaviorState(pOwner, pFSM)
, m_bJobComplete(false)
{
	// do nothing
}

GBehaviorMission::~GBehaviorMission()
{
	ClearMissionJob();
}

void GBehaviorMission::OnRegisterTrainstionRule()
{
	if (true == m_pOwnerNPC->GetNPCInfo()->bCombatable)
	{
		AddTransitionRule(GMSG_AI_EVT_TRY_ATTACKED,		AI_BEHAVIOR_COMBAT);
		AddTransitionRule(GMSG_AI_EVT_ATTACK,			AI_BEHAVIOR_COMBAT);
		AddTransitionRule(GMSG_AI_EVT_ATTACKED,			AI_BEHAVIOR_COMBAT);
		AddTransitionRule(GMSG_AI_EVT_FIND_TARGET,		AI_BEHAVIOR_COMBAT);
		AddTransitionRule(GMSG_AI_EVT_STARTCOMBAT,		AI_BEHAVIOR_COMBAT);
		AddTransitionRule(GMSGL_HIT_TALENT,				AI_BEHAVIOR_COMBAT);
	}

	AddTransitionRule(GMSG_AI_EVT_FLEE,				AI_COMBAT_FLEE);
	AddTransitionRule(GMSG_AI_EVT_DEAD,				AI_BEHAVIOR_DEAD);
	AddTransitionRule(GMSG_AI_EVT_PEACE,			AI_BEHAVIOR_PEACE);
	AddTransitionRule(GMSG_AI_EVT_SESSION,			AI_BEHAVIOR_SESSION);
	AddTransitionRule(GMSG_AI_EVT_SESSION_PUSHJOB,	AI_BEHAVIOR_SESSION);
}

void GBehaviorMission::Enter()
{
}

void GBehaviorMission::Exit(int nNextStateID)
{
}

void GBehaviorMission::PushMissionJob(GJob* pJob)
{
	pJob->Attr.bAutoDeletable = false;
	pJob->Attr.bClearJobListWhenCanceled = false;
	pJob->Attr.bClearJobListWhenTimedout = false;
	m_qScheduledJob.push_back(pJob);
}

void GBehaviorMission::ClearMissionJob()
{
	for (size_t i=0; i<m_qScheduledJob.size(); i++) // 이중 삭제되는 것 같습니다.
	{
		delete m_qScheduledJob[i];
	}

	m_qScheduledJob.clear();
}

GJob* GBehaviorMission::GetCurMissionJob()	
{
	if (m_qScheduledJob.empty())
		return NULL;

	return m_qScheduledJob.front();
}

bool					
GBehaviorMission::AdvanceJob()
{
	if (m_qScheduledJob.empty())
	{
		return false;
	}

	delete m_qScheduledJob.front();
	m_qScheduledJob.pop_front();

	return true;
}

void					
GBehaviorMission::OnJobFinished(const GJob* pLastJob)
{
	if (GetCurMissionJob() == pLastJob)
	{
		m_bJobComplete = true;
	}
}

GMessageResult GBehaviorMission::Message( GMessage& msg )
{
	GBehaviorState::Message(msg);

	switch (msg.m_nID)
	{
	case GMSG_AI_EVT_YELL:
		{
			EVENT_YELL_INFO info = *(EVENT_YELL_INFO*)(msg.m_pData);

			GEntityNPC* pSenderNPC = static_cast<GEntityNPC*>(m_pOwnerNPC->FindActor(info.uidSender));
			GEntityActor* pTargetActor = m_pOwnerNPC->FindActor(info.uidTarget);
			GetScriptNPC()->OnYell(m_pOwnerNPC->GetDelegator(), pSenderNPC->GetDelegator(), pTargetActor->GetDelegator());
			if (m_pOwnerNPC->GetNPCAssist())
			{
				m_pOwnerNPC->GetNPCAssist()->OnYell(pSenderNPC);
			}
		}break;
	case GMSG_AI_EVT_JOB_FINISHED:
		{
			OnJobFinished((GJob*)msg.m_pData);
		}break;
	}
	
	return MR_FALSE;
}

void GBehaviorMission::Update( float fDelta )
{
	PFI_BLOCK_THISFUNC(9062);
	AI_PFI_BLOCK(APT_ID_BEHAVIOR_MISSION, L"BehaviorMission");

	if (m_pOwnerNPC->HasSession())
	{
		GMessage msg(GMSG_AI_EVT_SESSION);
		m_pOwner->Message(msg);
		return; // 세션 진행중
	}

	if (m_bJobComplete)
	{
		m_bJobComplete = false;
		AdvanceJob();
	}

	GJob* pCurJob = GetCurMissionJob();
	if (!pCurJob)
	{
		// 수행할 미션잡이 없음
		GMessage msg(GMSG_AI_EVT_PEACE);
		m_pOwner->Message(msg);
		return;
	}

	GJobMgr* pJobMgr = m_pOwner->GetJobMgr();
	if (!pJobMgr->IsEmpty())
	{
		if (GetMissionJobCount() >= 2 &&
			pCurJob->GetID() == GJOB_ROAM)
		{
			// 미션잡이 있을 때는 로밍 무시
			pJobMgr->CancelCurrJob();
			AdvanceJob();
		}

		return; // 잡이 진행중
	}

	
	if (GConst::LOG_JOB)
	{
		mlog("[JobLog] <Run_MissionJob -> %d remain> Instant: %d, Owner:%s(%I64d), Job:%s\n", 
			GetMissionJobCount(), pCurJob->IsInstant(), MLocale::ConvUTF16ToAnsi(m_pOwnerNPC->GetName()).c_str(), m_pOwnerNPC->GetUID(), MLocale::ConvUTF16ToAnsi(pCurJob->GetJobName()).c_str());
	}

	// 미션잡 실행
	pJobMgr->Push(pCurJob);
}

int GBehaviorMission::GetMissionJobCount() const
{
	return (int)m_qScheduledJob.size();
}

deque<GJob*> GBehaviorMission::GetMissionJobs()
{
	return m_qScheduledJob;
}

bool GBehaviorMission::HasMissionJob() const
{
	return (GetMissionJobCount() > 0);
}
