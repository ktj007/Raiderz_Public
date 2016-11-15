#include "stdafx.h"
#include "GBehaviorPeace.h"
#include "GModuleAI.h"
#include "GEntityNPC.h"
#include "GColtPicker.h"
#include "GJob_Broker.h"
#include "GJob_Talent.h"
#include "GMessageID.h"
#include "GJobRunner.h"
#include "GAIProfiler.h"
#include "GJobMgr.h"
#include "GScriptManager.h"
#include "GScriptGroup_NPC.h"
#include "GColtMgr.h"
#include "GNPCAssist.h"

GBehaviorPeace::GBehaviorPeace(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM)
: GBehaviorState(pOwner, pFSM)
, m_nActType(PAT_STAND)
, m_pJobRunner(new GJobRunner())
{	
	// do nothing
}

GBehaviorPeace::~GBehaviorPeace()
{
	SAFE_DELETE(m_pJobRunner);
}

void GBehaviorPeace::OnRegisterTrainstionRule()
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
	AddTransitionRule(GMSG_AI_EVT_MISSION,			AI_BEHAVIOR_MISSION);
	AddTransitionRule(GMSG_AI_EVT_SESSION,			AI_BEHAVIOR_SESSION);
	AddTransitionRule(GMSG_AI_EVT_SESSION_PUSHJOB,	AI_BEHAVIOR_SESSION);
}

void GBehaviorPeace::Enter()	
{
	m_nActType = ChoiceActType();

	RunAct();
}

void GBehaviorPeace::Exit( int nNextStateID )
{

}

void GBehaviorPeace::Update(float fDelta)	
{	
	PFI_BLOCK_THISFUNC(9059);
	AI_PFI_BLOCK(APT_ID_BEHAVIOR_PEACE, L"BehaviorPeace");

	if (m_pOwnerNPC->HasSession())
	{
		GMessage msg(GMSG_AI_EVT_SESSION);
		m_pOwner->Message(msg);
		return; // 技记 柳青吝
	}

	if (m_pOwnerNPC->HasMissionJob())
	{
		GMessage msg(GMSG_AI_EVT_MISSION);
		m_pOwner->Message(msg);
		return; // 固记 柳青吝
	}

	GBehaviorState::Update(fDelta);	 
	
	GJobMgr* pJobMgr = m_pOwner->GetJobMgr();
	if (!pJobMgr->IsEmpty())
		return;

	RunAct();
}

GMessageResult GBehaviorPeace::Message( GMessage& msg )
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
		}
		break;
	}

	return MR_FALSE;
}

GBehaviorPeace::PeaceActType GBehaviorPeace::ChoiceActType()
{
	const SPAWN_INFO* pSpawnInfo = m_pOwnerNPC->GetSpawnInfo();
	if (pSpawnInfo)
	{
		if (pSpawnInfo->fRoamRadius > 0.0f) return PAT_ROAM;
	}

	return PAT_STAND;
}

void GBehaviorPeace::RunAct()
{
	switch (m_nActType)
	{
	case PAT_ROAM:
		{
			const SPAWN_INFO* pSpawnInfo = m_pOwnerNPC->GetSpawnInfo();
			if (pSpawnInfo)
			{
				m_pJobRunner->Roam(m_pOwner, pSpawnInfo->fRoamRadius);
			}
		}
		break;
	case PAT_STAND:
		{
			m_pOwner->GetColt()->RunColtIdle(m_pOwnerNPC);
		}
		break;
	}
}
