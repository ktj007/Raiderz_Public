#include "stdafx.h"
#include "GTestForward.h"

SUITE(Swim)
{
	struct FSwim
	{
		FSwim()
		{
			pField = GUTHelper_Field::DefaultMockField();
			pPlayerAttacker = test.player.NewPlayer(pField);
			pPlayerVictim = test.player.NewPlayer(pField);
		}

		~FSwim()
		{
		}

		MockField* pField;
		GEntityPlayer* pPlayerAttacker;
		GEntityPlayer* pPlayerVictim;

		DECLWRAPPER_Field;
	};

	TEST_FIXTURE(FSwim, ReleaseMFWhenBeginSwim)
	{
		GTalentInfo* pTalentinfo = test.talent.MakeDamageTalent(1);
		pTalentinfo->m_MotionFactorGroup.var[MF_UPPERED].nValue = 100;
		pTalentinfo->m_MotionFactorGroup.var[MF_UPPERED].nWeight = 1000;
		pTalentinfo->m_HitFilter.bMotionFactor[MF_UPPERED] = true;
		test.talent.UseTalent(pPlayerAttacker, pTalentinfo);

		CHECK_EQUAL(MF_UPPERED, pPlayerVictim->GetCurrentMotionFactor());
		pPlayerVictim->ChangeActionState(AS_SWIMMING);
		CHECK_EQUAL(MF_NONE, pPlayerVictim->GetCurrentMotionFactor());
	}
}