#include "StdAfx.h"
#include "GTalentStress.h"
#include "GTalent.h"
#include "GEntityNPC.h"
#include "GConst.h"
#include "GNPCStress.h"

//////////////////////////////////////////////////////////////////////////
//
//	GTalentStress
//
//////////////////////////////////////////////////////////////////////////

GTalentStress::GTalentStress( bool bGainStress )
: m_bGainStress(bGainStress)
{
}

void GTalentStress::OnEnterPhaseCanceled( GTalent* pTalent )
{
	PFI_B(3310, "GTalentStress::OnEnterPhaseCanceled");
	GainInterruptStress(pTalent->GetOwner());
	PFI_E(3310);
}

void GTalentStress::OnLeavePhaseFinish( GTalent* pTalent )
{
	PFI_B(3320, "GTalentStress::OnLeavePhaseFinish");

	if (IsAvoidThisTalent(pTalent))
	{
		GainAvoidStress(pTalent->GetOwner());
	}

	PFI_E(3320);
}

void GTalentStress::GainAvoidStress(GEntityActor* pOwner)
{
	if (!m_bGainStress)		return;
	VALID(pOwner);
	if (!pOwner->IsNPC())	return;

	GEntityNPC* pOwnerNPC = ToEntityNPC(pOwner);
	pOwnerNPC->GetNPCStress().AddStress(GConst::STRESS_ADD_AVOID_MELEE_ATTACK);
}

void GTalentStress::GainInterruptStress(GEntityActor* pOwner)
{
	VALID(pOwner);
	if (!pOwner->IsNPC())	return;

	GEntityNPC* pOwnerNPC = ToEntityNPC(pOwner);
	pOwnerNPC->GetNPCStress().AddStress(GConst::STRESS_ADD_INTERRUPT_DURING_ATTACKING);
}

bool GTalentStress::IsAvoidThisTalent(GTalent* pTalent)
{
	VALID_RET(pTalent, false);
	GTalentInfo* pTalentInfo = pTalent->GetInfo();
	VALID_RET(pTalentInfo, false);
	if (pTalentInfo->m_nSkillType != ST_MELEE)		return false;	// 근접공격만 회피했다고 가정한다.
	if (!pTalentInfo->isAttackable())				return false;
	if (IsHitThisTalent(pTalent))					return false;	// 때렸으면 스트레스를 안받는다.

	return true;
}

bool GTalentStress::IsHitThisTalent(GTalent* pTalent)
{
	VALID_RET(pTalent, false);
	return pTalent->HasVictim();
}