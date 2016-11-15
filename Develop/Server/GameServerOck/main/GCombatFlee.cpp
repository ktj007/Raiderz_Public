#include "stdafx.h"
#include "GCombatFlee.h"
#include "GMessageID.h"
#include "GEntityNPC.h"
#include "GJobRunner.h"
#include "GAIProfiler.h"
#include "GJobMgr.h"
#include "GHateTable.h"
#include "GNPCMove.h"
#include "GAIMonitorPort.h"

#define COMBAT_FLEE_TIMEOUT			(20.0f)
#define COMBAT_FLEE_YELL_INTERVAL	(5.0f)

/////////////////////////////////////////////////////////////////////////////////////////

GCombatFlee::GCombatFlee(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM) 
: GCombatState(pOwner, pFSM)
, m_pJobRunner(NULL)
{
	m_pJobRunner = new GJobRunner();
}

GCombatFlee::~GCombatFlee()
{
	SAFE_DELETE(m_pJobRunner);
}

void GCombatFlee::OnRegisterTrainstionRule()
{
	AddTransitionRule(GMSG_AI_EVT_STARTCOMBAT,				AI_COMBAT_PROLOGUE);
}

void GCombatFlee::Enter()
{
	m_YellIntervalRegulator.SetTickTime(COMBAT_FLEE_YELL_INTERVAL);
	m_YellIntervalRegulator.Start();

	GetNPC()->GetNPCMove().SetRetreat(true);
}

void GCombatFlee::Exit(int nNextStateID)
{
	GetNPC()->GetNPCMove().SetRetreat(false);

	if (GConst::AIMON_ENABLE &&
			GAIMonitorPort::Inst().IsSetedNPC(GetNPC()->GetUID()))
	{
		GAIMonitorPort::Inst().OnJobEvent(false, "ClearJob - cause by 'Exit CombatFleeState'");
	}
	GetNPC()->GetJobMgr()->Clear(true);
}

GMessageResult	GCombatFlee::Message(GMessage& msg)
{
	if (__super::Message(msg) == MR_TRUE)		return MR_TRUE;

	switch (msg.m_nID)
	{
	case GMSG_AI_EVT_FLEE:
		{
			EVENT_FLEE_INFO info = *(EVENT_FLEE_INFO*)(msg.m_pData);

			// 만료시간 지정
			m_ExpirationRegulator.SetTickTime(info.fDurationTime);
			m_ExpirationRegulator.Start();

			// 도망치는 타입 지정
			m_nFleeType = info.nFleeType;
		}
		break;
	}

	return MR_FALSE;
}

void GCombatFlee::Flee()
{
	if (!m_pOwner->GetJobMgr()->IsEmpty())
		return; // 수행할 잡이 있음

	switch(m_nFleeType)
	{
	case Flee_FromEnemy:
		if (GetNPC()->GetHateTable().IsEmpty())
		{
			// 적이 누군지 모를 경우
			GetNPC()->ReturnToHomePoint();
		}
		else
		{
			MUID uidTarget = GetNPC()->GetHateTable().GetTarget();
			m_pJobRunner->FleeFromEntity(GetAI(), uidTarget);
		}
		break;

	case Flee_ToHome:
		m_pJobRunner->FleeToHome(GetAI());
		break;
	}
	
}

void GCombatFlee::Update(float fDelta)
{
	AI_PFI_BLOCK(APT_ID_COMBAT_FLEE, L"CombatFlee");

	bool bExpirated			= m_ExpirationRegulator.IsReady(fDelta);

	if (m_YellIntervalRegulator.IsReady(fDelta))
	{
		GetNPC()->doYell();
	}

	if(!bExpirated)
	{
		Flee();
	}
	else
	{
		if (GetNPC()->GetHateTable().IsEmpty())
		{
			SendMsg_EndCombat();
		}
		else
		{
			SendMsg_StartCombat();			
		}

		return;
	}
}

float GCombatFlee::GetDurationTime() 
{
	return m_ExpirationRegulator.GetTickTime();
}
