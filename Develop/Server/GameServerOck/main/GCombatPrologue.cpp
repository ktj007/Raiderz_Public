#include "StdAfx.h"
#include "GCombatPrologue.h"
#include "GMessageID.h"
#include "GColtMgr.h"
#include "GJobMgr.h"
#include "GEntityNPC.h"

GCombatPrologue::GCombatPrologue(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM)
: GCombatState(pOwner, pFSM)
, m_bAlreadyUsed(false)
{
}

GCombatPrologue::~GCombatPrologue(void)
{
}

void GCombatPrologue::OnRegisterTrainstionRule()
{
	AddTransitionRule(GMSG_AI_EVT_ENDPROLOGUE,				AI_COMBAT_COMBAT);
}

void GCombatPrologue::Enter()
{
	if (m_bAlreadyUsed ||
		!m_pOwner->GetColt()->HasPrologueColt())
	{
		// 프롤로그 콜트 없음
		SendMsg_EndPrologue();	
		return;
	}

	m_pOwner->GetColt()->RunColtPrologue(GetNPC());
	m_bAlreadyUsed = true;
}

void GCombatPrologue::Exit( int nNextStateID )
{
}

void GCombatPrologue::Update( float fDelta )
{
	GJobMgr* pJobMgr = m_pOwner->GetJobMgr();
	if (!pJobMgr->IsEmpty())
		return;

	if (GetNPC()->IsNowUsingTalent())
		return;

	// 프롤로그 콜트 완료
	SendMsg_EndPrologue();
}
