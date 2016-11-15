#include "stdafx.h"
#include "GTask_Talent.h"
#include "GEntitySync.h"
#include "GMessageID.h"
#include "GCommandTable.h"
#include "GField.h"
#include "GEntityNPC.h"
#include "GModuleCombat.h"
#include "GTalent.h"
#include "GTalentInfoMgr.h"
#include "GGlobal.h"

GTask_Talent::GTask_Talent(GEntityNPC* pOwner): 
GTask(pOwner), m_nTalentID(INVALID_TALENT_ID) 
{
	;
}

GTask_UseTalent::GTask_UseTalent(GEntityNPC* pEntity, GTalentInfo* pTalentInfo, MUID uidTarget, bool bGainStress, bool bSetDirection)
: GTask_Talent(pEntity)
, m_pTalentInfo(pTalentInfo)
, m_uidTarget(uidTarget)
, m_bGainStress(bGainStress)
, m_bUsedTalent(false)
, m_bSetDirection(bSetDirection)
{
	_ASSERT(pTalentInfo);

	if (pTalentInfo)
		m_nTalentID = pTalentInfo->m_nID;
	else m_nTalentID = 0;

}

GTask_UseTalent::~GTask_UseTalent()
{

}

bool GTask_UseTalent::OnStart()
{
	PFI_BLOCK_THISFUNC(509);

	if (!m_pTalentInfo) return false;
	m_bUsedTalent = false;

	if (m_pTalentInfo &&
		m_pTalentInfo->m_bSetDirection &&
		m_bSetDirection)
	{
		m_pOwner->SetDirForwardToActor(m_uidTarget);
	}
	
	UseTalent();

	return true;
}

void GTask_UseTalent::UseTalent()
{
	mmlog("GTask_UseTalent::UseTalent\n");

	if (m_bUsedTalent) return;

	// 스킬 사용
	m_pOwner->doUseTalent(m_pTalentInfo->m_nID, m_uidTarget, false, m_bGainStress);
	m_bUsedTalent = true;
}

GTaskResult GTask_UseTalent::OnRun(float fDelta)
{
	PFI_BLOCK_THISFUNC(1803);

	if (!m_pTalentInfo)		return TR_CANCELED;

	if (!m_bUsedTalent)
	{
		UseTalent();
	}

	if (IsTalentCanceled())
	{
		m_pOwner->GetModuleCombat()->DeleteCurTalent();
		return TR_CANCELED;
	}

	if (IsTalentCompleted())
	{
		m_pOwner->GetModuleCombat()->DeleteCurTalent();
		return TR_COMPLETED;
	}

	return TR_RUNNING;
}

void GTask_UseTalent::OnComplete()
{
}

void GTask_UseTalent::OnCancel()
{
	GTalent* pCurrTalent = m_pOwner->GetModuleCombat()->GetTalent();
	if (!pCurrTalent)	
		return;

	pCurrTalent->CancelForce(true);

}

bool GTask_UseTalent::IsTalentCanceled()
{
	GTalent* pCurrTalent = m_pOwner->GetModuleCombat()->GetTalent();
	if (!pCurrTalent)	return false;
	
	return (pCurrTalent->GetPhase() == TALENT_PHASE_CANCELED);
}

bool GTask_UseTalent::IsTalentCompleted()
{
	GTalent* pCurrTalent = m_pOwner->GetModuleCombat()->GetTalent();
	if ( pCurrTalent == NULL  ||  pCurrTalent->GetPhase() == TALENT_PHASE_FINISH)
		return true;

	return false;
}

//////////////////

GTask_Guard::GTask_Guard(GEntityNPC* pEntity, int nGuardTalentID, float fGuardingTime): 
GTask_Talent(pEntity), m_fGuardingTime(fGuardingTime)
{
	m_fElapsed = 0.0f;
	m_bReleased = false;
	m_nTalentID = nGuardTalentID;

	m_fPostDelay = 1.0f;	// 기본은 1초
	GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(m_nTalentID);
	if (pTalentInfo && pTalentInfo->m_fPostDelay > 0.0f)
	{
		m_fPostDelay = pTalentInfo->m_fPostDelay;
	}
}

GTask_Guard::~GTask_Guard()
{

}

bool GTask_Guard::OnStart()
{
	m_pOwner->doGuard(m_nTalentID);

	return true;
}

GTaskResult GTask_Guard::OnRun(float fDelta)
{
	m_fElapsed += fDelta;

	if (!m_bReleased)
	{
		if (!m_pOwner->IsGuarding()) 
			return TR_CANCELED;
	}

	if (m_fElapsed >= m_fGuardingTime && !m_bReleased) 
	{
		GuardRelease();
	}
	else if (m_fElapsed >= m_fGuardingTime + m_fPostDelay)
	{
		return TR_COMPLETED;
	}

	return TR_RUNNING;
}

void GTask_Guard::OnComplete()
{

}

void GTask_Guard::OnCancel()
{
	
}

void GTask_Guard::GuardRelease()
{
	m_bReleased = true;
	m_pOwner->doGuardReleased();
}
