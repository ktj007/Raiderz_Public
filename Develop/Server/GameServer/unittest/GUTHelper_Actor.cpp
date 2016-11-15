#include "StdAfx.h"
#include "GUTHelper_Actor.h"
#include "GEntityActor.h"
#include "SUnitTestUtil.h"
#include "GTestForward.h"

CHR_INFO* GUTHelper_Actor::DefaultActorCharInfo()
{
	static CHR_INFO info;
	info.nMaxHP = 10000;
	info.nMaxEN = 10000;
	info.nMaxSTA = 10000;
	info.fSpeed = 100;
	return &info;
}

void GUTHelper_Actor::Hit( GEntityActor* pAttacker, GEntityActor* pVictim )
{
	VALID(pAttacker);
	VALID(pVictim);

	GTalentInfo* pAttackInfo = test.talent.MakeDamageTalent(10);
	test.talent.UseTalent(pAttacker, pAttackInfo, pVictim->GetUID());
}

