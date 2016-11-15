#include "stdafx.h"
#include "GBehaviorCombat.h"
#include "GCombatState.h"
#include "GCombatCombat.h"
#include "GCombatFlee.h"
#include "GCombatVictory.h"
#include "GCombatPrologue.h"
#include "GMessageID.h"
#include "GEntityNPC.h"
#include "GNPCPathFinder.h"
#include "GColtMgr.h"
#include "GField.h"
#include "GAIStatistics.h"
#include "GAIProfiler.h"
#include "GJobMgr.h"
#include "GHateTable.h"
#include "GNPCInteraction.h"
#include "GColtPicker.h"
#include "GInteractionSystem.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GPlayerNPCIconSender.h"
#include "GAIMonitorPort.h"

/////////////////////////////////////////////////////////////////////////////////////////
// GBehaviorCombat //////////////////////////////////////////////////////////////////////
GBehaviorCombat::GBehaviorCombat(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM) 
: GBehaviorState(pOwner, pFSM)
{
	m_pCombatStateMachine = new GCombatStateMachine(pOwner);

	GCombatState* pStates[AI_COMBAT_SIZE];
	for (int i = 0; i < AI_COMBAT_SIZE; i++)
		pStates[i] = NULL;

	pStates[AI_COMBAT_COMBAT] = new GCombatCombat(pOwner, m_pCombatStateMachine);
	pStates[AI_COMBAT_FLEE] = new GCombatFlee(pOwner, m_pCombatStateMachine);
	pStates[AI_COMBAT_VICTORY] = new GCombatVictory(pOwner, m_pCombatStateMachine);
	pStates[AI_COMBAT_PROLOGUE] = new GCombatPrologue(pOwner, m_pCombatStateMachine);

	for (int ai_state = 0; ai_state < AI_COMBAT_SIZE; ai_state++)
	{
		if (pStates[ai_state] != NULL) m_pCombatStateMachine->AddState(pStates[ai_state]);
	}
}

GBehaviorCombat::~GBehaviorCombat()
{
	m_pCombatStateMachine->DeleteAll();
	delete m_pCombatStateMachine;
}

void GBehaviorCombat::OnRegisterTrainstionRule()
{
	AddTransitionRule(GMSG_AI_EVT_ENDCOMBAT,				AI_BEHAVIOR_RETURN);
	AddTransitionRule(GMSG_AI_EVT_DEAD,						AI_BEHAVIOR_DEAD);
	AddTransitionRule(GMSG_AI_EVT_MISSION,					AI_BEHAVIOR_MISSION);
	AddTransitionRule(GMSG_AI_EVT_SESSION,					AI_BEHAVIOR_SESSION);
	AddTransitionRule(GMSG_AI_EVT_SESSION_CHANGE_SCENE,		AI_BEHAVIOR_SESSION);
	AddTransitionRule(GMSG_AI_EVT_SESSION_PUSHJOB,			AI_BEHAVIOR_SESSION);
}

void GBehaviorCombat::Enter()
{
	m_pOwner->GetOwnerNPC()->GetPathFinder()->OnEnterToCombat();	

	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(m_pOwnerNPC->GetUID()))
	{
		GAIMonitorPort::Inst().OnJobEvent(false, "ClearJob - cause by 'Enter BehaviorCombatState'");
	}

	GJobMgr* pJobMgr = m_pOwner->GetJobMgr();
	pJobMgr->Clear();

	AI_STAT_BATTLE_INC(m_pOwnerNPC->GetField()->GetID(), m_pOwnerNPC->GetSectorX(), m_pOwnerNPC->GetSectorY());

	GVectorMUID vecPlayerUID;
	m_pOwnerNPC->GetNeighborSectorPlayer(vecPlayerUID.Vector());
	
	for each (const MUID& nPlayerUID in vecPlayerUID.Vector())
	{
		GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(nPlayerUID);
		if (NULL == pPlayer) continue;

		pPlayer->GetNPCIconSender().SendByFindEnemy(m_pOwnerNPC);
	}

	m_pOwnerNPC->GetNPCInteraction().DisableByCombatEnter();

	// 전투 시작시에 비헤비어 모드로 고정
	//m_pOwnerNPC->BehaviorMode();

	m_pOwner->OnCombatBegin();
	m_pOwnerNPC->OnCombatBegin(m_pOwnerNPC);

	m_pCombatStateMachine->ChangeState(AI_COMBAT_PROLOGUE);
}

void GBehaviorCombat::Exit(int nNextStateID)
{
	GState<GModuleAI>* pCurState = 
		m_pCombatStateMachine->GetCurrentState();

	if (pCurState)
		pCurState->Exit(-1);

	m_pCombatStateMachine->Reset();
	
	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(m_pOwnerNPC->GetUID()))
	{
		GAIMonitorPort::Inst().OnJobEvent(false, "ClearJob - cause by 'Exit BehaviorCombatState'");
	}
	GJobMgr* pJobMgr = m_pOwner->GetJobMgr();
	pJobMgr->Clear();

	m_pOwner->SetAssist(false);

	m_pOwnerNPC->GetPathFinder()->OnLeaveFromCombat();
	m_pOwnerNPC->GetNPCInteraction().EnableByCombatExit();
	m_pOwner->GetColt()->GetColtCombat()->Reset();
	
	m_pOwner->OnCombatEnd();
	m_pOwnerNPC->OnCombatEnd(m_pOwnerNPC);
}

GMessageResult GBehaviorCombat::OnDefaultMessage(GMessage& msg)
{
	switch (msg.m_nID)
	{
	case GMSG_AI_EVT_TRY_ATTACKED:
		{
			
		}break;
	case GMSG_AI_EVT_ATTACKED:
		{
			EVENT_ATTACKED_INFO info = *(EVENT_ATTACKED_INFO*)(msg.m_pData);

			GEntityActor* pOpponent = NULL;
			_ASSERT(m_pOwnerNPC->GetNPCInfo());
			if (!m_pOwnerNPC->GetNPCInfo())					return MR_TRUE;
						
			pOpponent = m_pOwnerNPC->FindActor(info.uidAttacker);

			m_pOwner->GetColt()->RunColtReaction(m_pOwnerNPC, pOpponent);
		}				
		break;
	case GMSG_AI_EVT_FIND_TARGET:
		{
		}
		break;
	case GMSG_AI_EVT_STARTCOMBAT:
		{
			EVENT_START_COMBAT info = *(EVENT_START_COMBAT*)(msg.m_pData);

			if (info.uidEnemy.IsValid())
			{
				if (m_pOwnerNPC->GetField())
				{
					GEntityActor* pAttacker = m_pOwnerNPC->GetField()->FindActor(info.uidEnemy);
					if (pAttacker)
						m_pOwnerNPC->GetHateTable().AddPoint_FoundEnemy(pAttacker);
				}
			}
		}
		break;
	}

	return MR_FALSE;
}

GMessageResult GBehaviorCombat::Message(GMessage& msg)
{
	if (__super::Message(msg) == MR_TRUE)					return MR_TRUE;
	if (OnDefaultMessage(msg) == MR_TRUE)					return MR_TRUE;
	if (m_pCombatStateMachine->Message(msg) == MR_TRUE)		return MR_TRUE;
	

	return MR_FALSE;
}

void GBehaviorCombat::Update(float fDelta)
{
	PFI_BLOCK_THISFUNC(9060);
	AI_PFI_BLOCK(APT_ID_BEHAVIOR_COMBAT, L"BehaviorCombat");

	GBehaviorState::Update(fDelta);
	m_pCombatStateMachine->Update(fDelta);
}

void GBehaviorCombat::ChangeCombat(AI_COMBAT_STATE nNextCombat)
{
	m_pCombatStateMachine->ChangeState(nNextCombat);
}

GState<GModuleAI>*	GBehaviorCombat::FindCombatState(int nStateID)
{
	return m_pCombatStateMachine->FindState(nStateID);
}


AI_COMBAT_STATE GBehaviorCombat::GetCurrentStateID()
{
	return (AI_COMBAT_STATE)m_pCombatStateMachine->GetCurrentStateID();
}


AI_COMBAT_STATE GBehaviorCombat::GetPrevStateID()
{
	return (AI_COMBAT_STATE)m_pCombatStateMachine->GetNextStateID();
}

void GBehaviorCombat::SetCombatFSMLog( bool bLog )
{
	m_pCombatStateMachine->SetDebugName(L"Combat");
	m_pCombatStateMachine->SetLog(bLog);
}