#include "StdAfx.h"
#include "GBehaviorReturn.h"
#include "GEntityNPC.h"
#include "GModuleAI.h"
#include "GJobMgr.h"
#include "GNPCMove.h"
#include "GAIMonitorPort.h"

GBehaviorReturn::GBehaviorReturn(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM)
: GBehaviorState(pOwner, pFSM)
{
}

GBehaviorReturn::~GBehaviorReturn(void)
{
}

void GBehaviorReturn::OnRegisterTrainstionRule()
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
}

void GBehaviorReturn::Enter()
{
	// 귀환중 플래그 켜기
	m_pOwnerNPC->GetNPCMove().SetReturnToMove(true);

	// 잡 초기화
	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(m_pOwnerNPC->GetUID()))
	{
		GAIMonitorPort::Inst().OnJobEvent(false, "ClearJob - cause by 'Enter BehaviorReturnState'");
	}
	m_pOwner->GetJobMgr()->Clear();

	// 홈포인트로 돌아가기
	m_pOwnerNPC->ReturnToHomePoint();
}

void GBehaviorReturn::Exit( int nNextStateID )
{
	// 귀환중 플래그 끄기
	m_pOwnerNPC->GetNPCMove().SetReturnToMove(false);
}

void GBehaviorReturn::Update( float fDelta )
{
	PFI_BLOCK_THISFUNC(9063);

	if (!m_pOwner->GetJobMgr()->IsEmpty())	
		return; // 잡이 진행중

	// 수행할 잡이 없음
	GMessage msg(GMSG_AI_EVT_MISSION);
	m_pOwner->Message(msg);
}



