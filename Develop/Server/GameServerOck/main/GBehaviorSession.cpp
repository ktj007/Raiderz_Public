#include "stdafx.h"
#include "GBehaviorSession.h"
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
#include "GFieldNPCSession.h"
#include "GField.h"
#include "GFieldNPCSessionMgr.h"

GBehaviorSession::GBehaviorSession( GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM ) 
: GBehaviorState(pOwner, pFSM)
, m_pCurJob(NULL)
{
	// do nothing
}

GBehaviorSession::~GBehaviorSession()
{
	ClearCurJob();
}

void GBehaviorSession::OnRegisterTrainstionRule()
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
}

void GBehaviorSession::Enter()
{
	if (GConst::LOG_NPCSESSION)
	{
		mlog("[SessionLog] %s(%I64d)\n", __FUNCTION__, m_pOwnerNPC->GetUID());
	}

	m_pOwnerNPC->GetJobMgr()->Clear();
}

void GBehaviorSession::Exit(int nNextStateID)
{
	if (GConst::LOG_NPCSESSION)
	{
		mlog("[SessionLog] %s(%I64d)\n", __FUNCTION__, m_pOwnerNPC->GetUID());
	}
}

GMessageResult GBehaviorSession::Message( GMessage& msg )
{
	__super::Message(msg);

	switch (msg.m_nID)
	{
	case GMSG_AI_EVT_SESSION:
		{
			if (msg.m_pData)
			{
				m_uidSession = *(MUID*)msg.m_pData;
			}

			m_pOwnerNPC->ClearJob();
		}break;
	case GMSG_AI_EVT_SESSION_FINISHED:
		{
			m_uidSession.SetInvalid();
		}break;
	case GMSG_AI_EVT_SESSION_PUSHJOB:
		{
			GJob* pSessionJob = (GJob*)msg.m_pData;
			VALID_RET(pSessionJob, MR_FALSE);
			SetJob(pSessionJob);
		}break;
	case GMSG_AI_EVT_SESSION_CHANGE_SCENE:
		{
			ClearCurJob();
			m_pOwnerNPC->ClearJob();
		}break;
	case GMSG_AI_EVT_JOB_COMPLETED:
		{
			OnJobFinished((GJob*)msg.m_pData);
		}break;
	}

	return MR_FALSE;
}

void GBehaviorSession::SetJob(GJob* pJob)
{
	if (GConst::LOG_NPCSESSION)
	{
		mlog("[SessionLog] %s(%I64d) - old_job(%s), new_job(%s)\n", __FUNCTION__, m_pOwnerNPC->GetUID(),
			m_pCurJob?MLocale::ConvUTF16ToAnsi(m_pCurJob->GetJobName()).c_str():"{none}",
			pJob?MLocale::ConvUTF16ToAnsi(pJob->GetJobName()).c_str():"{none}"
			);
	}

	if (!m_pOwnerNPC->GetJobMgr())
		return;

	pJob->Attr.bAutoDeletable = false;
	//if (m_pCurJob)
	//{
	//	m_pOwnerNPC->GetJobMgr()->CancelCurrJob();
	//	GFieldNPCSession* pSession =
	//		m_pOwnerNPC->GetJobMgr()->GetSession()->GetFieldSession();
	//	if (pSession)
	//	{
	//		m_pOwnerNPC->GetJobMgr()->GetSession()->OnJobFinished(m_pCurJob);
	//	}
	//}
	m_pCurJob = pJob;
}

void GBehaviorSession::ClearCurJob()
{
	if (GConst::LOG_NPCSESSION)
	{
		mlog("[SessionLog] %s(%I64d) - old_job(%s)\n", __FUNCTION__, m_pOwnerNPC->GetUID(), 
			m_pCurJob?MLocale::ConvUTF16ToAnsi(m_pCurJob->GetJobName()).c_str():"{none}"
			);
	}

	if (!m_pOwnerNPC->GetJobMgr())
		return;

	//if (m_pCurJob)
	//{
	//	m_pOwnerNPC->GetJobMgr()->CancelCurrJob();
	//}

	m_pCurJob = NULL;
}

void GBehaviorSession::Update( float fDelta )
{
	GJobMgr* pJobMgr = m_pOwner->GetJobMgr();
	if (!pJobMgr->IsEmpty())	
		return; // 잡이 진행중

	if (!m_pCurJob)
		return; // 현재 세션으로부터 할당된 잡 없음

	if (!m_pOwner->HasNPCSession())
	{
		// 세션에 속해있지 않음
		GMessage msg(GMSG_AI_EVT_MISSION);
		m_pOwner->Message(msg);
		return;
	}
	
	if (GConst::LOG_JOB)
	{
		mlog("[JobLog] <Run_SessionJob> Instant: %d, Owner:%s(%I64d), Job:%s\n", 
			m_pCurJob->IsInstant(), MLocale::ConvUTF16ToAnsi(m_pOwnerNPC->GetName()).c_str(), m_pOwnerNPC->GetUID(), MLocale::ConvUTF16ToAnsi(m_pCurJob->GetJobName()).c_str());
	}

	if (GConst::LOG_NPCSESSION)
	{
		mlog("[SessionLog] %s_PushJob(%I64d) - job(%s)\n", __FUNCTION__, m_pOwnerNPC->GetUID(), 
			m_pCurJob?MLocale::ConvUTF16ToAnsi(m_pCurJob->GetJobName()).c_str():"{none}");
	}

	pJobMgr->Push(m_pCurJob);
}

void GBehaviorSession::OnJobFinished( const GJob* pLastJob )
{
	VALID(pLastJob);
	if (m_pCurJob == pLastJob)
	{
		ClearCurJob();

		GFieldNPCSession* pSession = GetSession();
		if (pSession)
			pSession->OnJobFinished(pLastJob);
	}
}

GFieldNPCSession* GBehaviorSession::GetSession()
{
	if (m_uidSession.IsInvalid())
		return NULL;

	GField* pField = m_pOwnerNPC->GetField();
	if (!pField)
	{
		m_uidSession.SetInvalid();
		return NULL;
	}

	return pField->GetSession()->Find(m_uidSession);
}
