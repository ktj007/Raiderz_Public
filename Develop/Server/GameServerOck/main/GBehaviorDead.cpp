#include "StdAfx.h"
#include "GBehaviorDead.h"
#include "GMessageID.h"
#include "GAIProfiler.h"
#include "GEntityNPC.h"
#include "GField.h"
#include "GSpawnManager.h"
#include "GJobMgr.h"
#include "GModuleAI.h"
#include "GGlueNPC.h"
#include "GNPCDecay.h"
#include "GColtMgr.h"

GBehaviorDead::GBehaviorDead(GModuleAI* pOwner, GStateMachine<GModuleAI>* pFSM)
: GBehaviorState(pOwner, pFSM)
{
}

GBehaviorDead::~GBehaviorDead()
{
}

void GBehaviorDead::OnRegisterTrainstionRule()
{
	
}

void GBehaviorDead::Enter()
{	
	GJobMgr* pJobMgr = m_pOwner->GetJobMgr();
	pJobMgr->Clear();

	if (!m_pOwner->GetColt()->HasEpilogueColt())
	{
		OnDie();
		return;
	}

	m_pOwner->GetColt()->RunColtEpilogue(m_pOwnerNPC);
}

void GBehaviorDead::Exit(int nNextStateID)
{
}

void GBehaviorDead::Update(float fDelta)	
{
	PFI_BLOCK_THISFUNC(9061);
	AI_PFI_BLOCK(APT_ID_BEHAVIOR_DEAD, L"BehaviorDead");

	GBehaviorState::Update(fDelta);

	if (m_pOwnerNPC->IsDead())
		return; // ÀÌ¹Ì Á×¾úÀ½

	GJobMgr* pJobMgr = m_pOwner->GetJobMgr();
	if (!pJobMgr->IsEmpty())
		return;

	if (m_pOwnerNPC->IsNowUsingTalent())
		return;

	OnDie();
}

GMessageResult GBehaviorDead::Message( GMessage& msg )
{
	GBehaviorState::Message(msg);	

	return MR_FALSE;
}

void GBehaviorDead::OnDie()
{
	VALID(m_pOwnerNPC);
	m_pOwnerNPC->doDieForced();
}
