#include "stdafx.h"
#include "GDeadMotionFactorImpactor.h"
#include "GEntityNPC.h"

bool GDeadMotionFactorImpactor::CheckUp( GMFApplyArgs& outArgs, GEntityActor* pTarget, GTalentInfo* pTalentInfo, int nDamage )
{
	if (pTarget->GetHP() > nDamage) return false;
	if (pTarget->IsNPC() == false) return false;
	if (pTalentInfo->m_nForceMF != MF_NONE) return false;
	if (pTalentInfo->m_FinishEffect.m_nFinishType == TALENT_FINISH_NONE) return false;

	if (Dice() == false) return false;

	GEntityNPC* pNPC = ToEntityNPC(pTarget);
	if (CheckTargetMFModifier(pNPC) == false) return false;

	outArgs.nForceMF = MakeFinishEffectMF(pTalentInfo);
	outArgs.nForceMFWeight = MakeFinishEffectMFWeight(pNPC);

	return true;
}

bool GDeadMotionFactorImpactor::Dice()
{
	return true;
	return (GMath::RandomNumber(0, 9) < 7);
}

bool GDeadMotionFactorImpactor::CheckTargetMFModifier( GEntityNPC* pTarget )
{
	if (pTarget->GetNPCInfo()->MFModifier.Value[MF_KNOCKBACK].fPer < 0.1f) return false;
	if (pTarget->GetNPCInfo()->MFModifier.Value[MF_THROWUP].fPer < 0.1f) return false;

	return true;
}

MF_STATE GDeadMotionFactorImpactor::MakeFinishEffectMF( GTalentInfo* pTalentInfo )
{
	switch (pTalentInfo->m_FinishEffect.m_nFinishType)
	{
	case TALENT_FINISH_THROW_UP: return MF_THROWUP;
	case TALENT_FINISH_KNOCK_BACK: return MF_KNOCKBACK;
	case TALENT_FINISH_RANDOM: 
		{
			if (GMath::RandomNumber(0, 9) < 5)
			{
				return MF_KNOCKBACK;
			}
			else
			{
				return MF_THROWUP;
			}
		}
		break;
	}
	
	return MF_KNOCKBACK;
}

int GDeadMotionFactorImpactor::MakeFinishEffectMFWeight(GEntityNPC* pTarget)
{
	float fColRadius = pTarget->GetColRadius();

	if (fColRadius < 50)
	{
		return GMath::RandomNumber(DEAD_IMPACT_MOTION_FACTOR_KB_WEIGHT_MIN, DEAD_IMPACT_MOTION_FACTOR_KB_WEIGHT_MAX_3);
	}
	else if (fColRadius < 200)
	{
		return GMath::RandomNumber(DEAD_IMPACT_MOTION_FACTOR_KB_WEIGHT_MIN, DEAD_IMPACT_MOTION_FACTOR_KB_WEIGHT_MAX_2);
	}

	return GMath::RandomNumber(DEAD_IMPACT_MOTION_FACTOR_KB_WEIGHT_MIN, DEAD_IMPACT_MOTION_FACTOR_KB_WEIGHT_MAX_1);
}