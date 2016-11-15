#include "StdAfx.h"
#include "FBaseCombat.h"
#include "GTestForward.h"

FBaseCombat::FBaseCombat(void)
{
}

FBaseCombat::~FBaseCombat(void)
{
}

void FBaseCombat::Attack( GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pAttackTalent )
{
	if (!pAttackTalent)
	{
		pAttackTalent = test.talent.MakeDamageTalent(1);
	}
	
	test.talent.UseTalent(pAttacker, pAttackTalent, pVictim->GetUID());
}
