#include "stdafx.h"
#include "GCombatState.h"
#include "GField.h"
#include "GMessageID.h"
#include "GEntityNPC.h"
#include "GNPCDebugger.h"
#include "GColtMgr.h"
#include "GJobMgr.h"
#include "GHateTable.h"

/////////////////////////////////////////////////////////////////////////////////////////
// GCombatState ////////////////////////////////////////////////////////////////////////

GCombatState::GCombatState( GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM ) 
: GState<GModuleAI>(pOwner, pFSM)
, m_pOwnerNPC(pOwner->GetOwnerNPC())
{

}

bool GCombatState::IsExistEnemy()
{
	return (GetNPC()->GetHateTable().GetTarget().IsValid())?true:false;
}

bool GCombatState::IsObtainVictory()
{
	return (!IsExistEnemy())?true:false;
}

void GCombatState::SendMsg_StartCombat(MUID uidEnemy)
{
	EVENT_START_COMBAT param;
	param.uidEnemy = uidEnemy;

	GMessage msg(GMSG_AI_EVT_STARTCOMBAT, &param);
	GetAI()->Message(msg);
}

void GCombatState::SendMsg_EndCombat()
{
	GMessage msg(GMSG_AI_EVT_ENDCOMBAT);
	GetAI()->Message(msg);
}


void GCombatState::SendMsg_StartVictory()
{
	GMessage msg(GMSG_AI_EVT_START_VICTORY);
	GetAI()->Message(msg);
}

void GCombatState::SendMsg_EndPrologue()
{
	GMessage msg(GMSG_AI_EVT_ENDPROLOGUE);
	GetAI()->Message(msg);
}

void GCombatState::Logf( const wchar_t* szText, ... )
{
	if (!m_pOwnerNPC)		
	{
		_ASSERT(m_pOwnerNPC);
		return;
	}
	wchar_t szBuff[1024];

	va_list args;
	va_start(args,szText);
	vswprintf_s(szBuff,szText,args);
	va_end(args);

	m_pOwnerNPC->GetDebugger()->Log(szBuff);
}

bool GCombatState::IsJobRunning()
{
	GJobMgr* pJobMgr = m_pOwner->GetJobMgr();
	
	if (!pJobMgr->IsEmpty())
	{
		return	true;
	}

	return false;
}

void GCombatStateMachine::OnPreChangeState( GState<GModuleAI>* pNextState )
{
	__super::OnPreChangeState( pNextState );

	m_nPrevStateID = GetCurrentStateID();
}

#include "GAIMonitorPort.h"

void GCombatStateMachine::OnChangedState( GState<GModuleAI>* pNextState )
{
	__super::OnChangedState(pNextState);

	VALID(pNextState);
	if (GConst::AIMON_ENABLE &&
		GAIMonitorPort::Inst().IsSetedNPC(m_pOwner->GetOwnerNPC()->GetUID()))
	{
		GAIMonitorPort::Inst().SendEventMessageToViewer("BehaviorStateChanged: %s", MLocale::ConvUTF16ToAnsi(pNextState->DebugString()).c_str());
	}
}
