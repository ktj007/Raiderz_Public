#include "stdafx.h"
#include "GBehaviorStateMachine.h"
#include "GAIState.h"
#include "GBehaviorState.h"
#include "GBehaviorCombat.h"
#include "GBehaviorPeace.h"
#include "GBehaviorMission.h"
#include "GBehaviorSession.h"
#include "GBehaviorDead.h"
#include "GBehaviorReturn.h"
#include "GDef.h"
#include "GAIMonitorPort.h"

GBehaviorStateMachine::GBehaviorStateMachine(GModuleAI* pOwner)
: GStateMachine<GModuleAI>(pOwner)
, m_nPrevStateID(-1)
{
	// do nothing
}

GBehaviorStateMachine::~GBehaviorStateMachine()
{
	DeleteAll();
}

void GBehaviorStateMachine::Init(void)
{
	GBehaviorState* pStates[AI_BEHAVIOR_SIZE];

	pStates[AI_BEHAVIOR_PEACE] = new GBehaviorPeace(m_pOwner, this);
	pStates[AI_BEHAVIOR_COMBAT] = new GBehaviorCombat(m_pOwner, this);
	pStates[AI_BEHAVIOR_MISSION] = new GBehaviorMission(m_pOwner, this);
	pStates[AI_BEHAVIOR_DEAD] = new GBehaviorDead(m_pOwner, this);
	pStates[AI_BEHAVIOR_RETURN] = new GBehaviorReturn(m_pOwner, this);
	pStates[AI_BEHAVIOR_SESSION] = new GBehaviorSession(m_pOwner, this);

	AddState(pStates[AI_BEHAVIOR_PEACE]);
	AddState(pStates[AI_BEHAVIOR_COMBAT]);
	AddState(pStates[AI_BEHAVIOR_MISSION]);
	AddState(pStates[AI_BEHAVIOR_DEAD]);
	AddState(pStates[AI_BEHAVIOR_RETURN]);
	AddState(pStates[AI_BEHAVIOR_SESSION]);

	ChangeState(AI_BEHAVIOR_MISSION);
}

void GBehaviorStateMachine::Reset()
{
	m_nPrevStateID = -1;
	SetCurrentState(NULL);
}

void GBehaviorStateMachine::PushMissionJob(GJob* pJob)
{
	GBehaviorMission* pState = dynamic_cast<GBehaviorMission*>(FindState(AI_BEHAVIOR_MISSION));
	VALID(pState);
	pState->PushMissionJob(pJob);
}

int GBehaviorStateMachine::GetMissionJobCounter()
{
	GBehaviorMission* pState = dynamic_cast<GBehaviorMission*>(FindState(AI_BEHAVIOR_MISSION));
	VALID_RET(pState, 0);
	return pState->GetMissionJobCount();
}

deque<GJob*> GBehaviorStateMachine::GetMissionJobs()
{
	GBehaviorMission* pState = dynamic_cast<GBehaviorMission*>(FindState(AI_BEHAVIOR_MISSION));
	VALID_RET(pState, deque<GJob*>());
	return pState->GetMissionJobs();
}

GJob* GBehaviorStateMachine::GetCurSessionJob()
{
	GBehaviorSession* pState = dynamic_cast<GBehaviorSession*>(FindState(AI_BEHAVIOR_SESSION));
	VALID_RET(pState, NULL);
	return pState->GetCurJob();
}

GFieldNPCSession* GBehaviorStateMachine::GetNPCSession()
{
	GBehaviorSession* pState = dynamic_cast<GBehaviorSession*>(FindState(AI_BEHAVIOR_SESSION));
	VALID_RET(pState, NULL);
	return pState->GetSession();
}

void GBehaviorStateMachine::ClearMissionJob()
{
	GBehaviorMission* pState = dynamic_cast<GBehaviorMission*>(FindState(AI_BEHAVIOR_MISSION));
	VALID(pState);
	pState->ClearMissionJob();
}

int GBehaviorStateMachine::GetCurrentCombatStateID()
{
	if (GetCurrentStateID() != AI_BEHAVIOR_COMBAT) return AI_COMBAT_NONE;

	return ((GBehaviorCombat*)FindState(AI_BEHAVIOR_COMBAT))->GetCurrentStateID();
}

int GBehaviorStateMachine::GetPrevCombatStateID()
{
	if (GetCurrentStateID() != AI_BEHAVIOR_COMBAT) return AI_COMBAT_NONE;

	return ((GBehaviorCombat*)FindState(AI_BEHAVIOR_COMBAT))->GetPrevStateID();
}

void GBehaviorStateMachine::SetFSMLog(bool bLog)
{
	SetDebugName(L"Behavior");
	SetLog(bLog);

	GBehaviorCombat* pBehaviorCombat = ((GBehaviorCombat*)FindState(AI_BEHAVIOR_COMBAT));
	pBehaviorCombat->SetCombatFSMLog(bLog);
}

bool GBehaviorStateMachine::IsNowCombat()
{
	return GetCurrentStateID() == AI_BEHAVIOR_COMBAT;
}

int GBehaviorStateMachine::GetPrevStateID()
{
	return m_nPrevStateID; 
}

void GBehaviorStateMachine::OnPreChangeState( GState<GModuleAI>* pNextState )
{
	__super::OnPreChangeState( pNextState );

	m_nPrevStateID = GetCurrentStateID();

	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(m_pOwner->GetOwnerUID()))
	{
		GAIMonitorPort::Inst().SendEventMessageToViewer("BehaviorStateChanged: %s", MLocale::ConvUTF16ToAnsi(pNextState->DebugString()).c_str());
	}
}



