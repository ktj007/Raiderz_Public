#include "stdafx.h"
#include "GMotionFactorHelper.h"

SUITE(MotionFactorHelper)
{
	TEST(CSMotionFactorHelper_IsFakeMotionFactor)
	{
		CHECK_EQUAL(true, CSMotionFactorHelper::IsFakeMotionFactor(MF_FAKE_BEATEN));
		CHECK_EQUAL(true, CSMotionFactorHelper::IsFakeMotionFactor(MF_FAKE_KNOCKBACK));

		CHECK_EQUAL(false, CSMotionFactorHelper::IsFakeMotionFactor(MF_BEATEN));
		CHECK_EQUAL(false, CSMotionFactorHelper::IsFakeMotionFactor(MF_KNOCKBACK));
		CHECK_EQUAL(false, CSMotionFactorHelper::IsFakeMotionFactor(MF_STUN));
		CHECK_EQUAL(false, CSMotionFactorHelper::IsFakeMotionFactor(MF_DEFLECT));
		CHECK_EQUAL(false, CSMotionFactorHelper::IsFakeMotionFactor(MF_THROWUP));
		CHECK_EQUAL(false, CSMotionFactorHelper::IsFakeMotionFactor(MF_KNOCKDOWN));
		CHECK_EQUAL(false, CSMotionFactorHelper::IsFakeMotionFactor(MF_UPPERED));
		CHECK_EQUAL(false, CSMotionFactorHelper::IsFakeMotionFactor(MF_GRAPPLED));
		CHECK_EQUAL(false, CSMotionFactorHelper::IsFakeMotionFactor(MF_SWALLOWED));
		CHECK_EQUAL(false, CSMotionFactorHelper::IsFakeMotionFactor(MF_DRAG));
	}

	TEST(GMotionFactorHelper_CalcReleaseTime_Beatens)
	{
		int nWeight = 1000;
		CHECK_CLOSE2((nWeight / 1000.0f), GMotionFactorHelper::CalcReleaseTime(MF_BEATEN, nWeight, 1.0f, ETID_PLAYER));
		CHECK_CLOSE2((nWeight / 1000.0f)+FORCED_RELEASE_TIME_DELAY, GMotionFactorHelper::CalcReleaseTime(MF_BEATEN, nWeight, 1.0f, ETID_NPC));

		CHECK_CLOSE2((nWeight / 1000.0f)+FORCED_RELEASE_TIME_DELAY, GMotionFactorHelper::CalcReleaseTime(MF_STUN, nWeight, 1.0f, ETID_NPC));
		CHECK_CLOSE2((nWeight / 1000.0f)+FORCED_RELEASE_TIME_DELAY, GMotionFactorHelper::CalcReleaseTime(MF_DEFLECT, nWeight, 1.0f, ETID_NPC));
		CHECK_CLOSE2((nWeight / 1000.0f)+FORCED_RELEASE_TIME_DELAY, GMotionFactorHelper::CalcReleaseTime(MF_GRAPPLED, nWeight, 1.0f, ETID_NPC));
		CHECK_CLOSE2((nWeight / 1000.0f)+FORCED_RELEASE_TIME_DELAY, GMotionFactorHelper::CalcReleaseTime(MF_SWALLOWED, nWeight, 1.0f, ETID_NPC));
	}

	TEST(GMotionFactorHelper_CalcReleaseTime_Knockdowns)
	{
		int nWeight = 2000;
		float fStandupTime = 1.8f;	// 일어나는 시간

		CHECK_CLOSE2(fStandupTime + (nWeight / 1000.0f), GMotionFactorHelper::CalcReleaseTime(MF_KNOCKDOWN, nWeight, fStandupTime, ETID_PLAYER));
		CHECK_CLOSE2(fStandupTime + (nWeight / 1000.0f)+FORCED_RELEASE_TIME_DELAY, GMotionFactorHelper::CalcReleaseTime(MF_KNOCKDOWN, nWeight, fStandupTime, ETID_NPC));

		CHECK_CLOSE2(fStandupTime + (nWeight / 1000.0f), GMotionFactorHelper::CalcReleaseTime(MF_THROWUP, nWeight, fStandupTime, ETID_PLAYER));
		CHECK_CLOSE2(fStandupTime + (nWeight / 1000.0f)+FORCED_RELEASE_TIME_DELAY, GMotionFactorHelper::CalcReleaseTime(MF_THROWUP, nWeight, fStandupTime, ETID_NPC));
	}

	TEST(GMotionFactorHelper_CalcReleaseTime_Uppereds)
	{
		int nWeight = 1500;

		CHECK_CLOSE2(KNOCKBACK_TIME * (nWeight / 100.0f), GMotionFactorHelper::CalcReleaseTime(MF_UPPERED, nWeight, 1.0f, ETID_PLAYER));
		CHECK_CLOSE2(KNOCKBACK_TIME * (nWeight / 100.0f)+FORCED_RELEASE_TIME_DELAY, GMotionFactorHelper::CalcReleaseTime(MF_UPPERED, nWeight, 1.0f, ETID_NPC));

		CHECK_CLOSE2(KNOCKBACK_TIME * (nWeight / 100.0f), GMotionFactorHelper::CalcReleaseTime(MF_KNOCKBACK, nWeight, 1.0f, ETID_PLAYER));
		CHECK_CLOSE2(KNOCKBACK_TIME * (nWeight / 100.0f)+FORCED_RELEASE_TIME_DELAY, GMotionFactorHelper::CalcReleaseTime(MF_KNOCKBACK, nWeight, 1.0f, ETID_NPC));
	}

	TEST(GMotionFactorHelper_CalcReleaseTime_Drag)
	{
		int nWeight = 800;

		CHECK_CLOSE2(DRAG_TIME * (nWeight / 100.0f)+1.0f, GMotionFactorHelper::CalcReleaseTime(MF_DRAG, nWeight, 1.0f, ETID_PLAYER));
		CHECK_CLOSE2(DRAG_TIME * (nWeight / 100.0f)+1.0f+FORCED_RELEASE_TIME_DELAY, GMotionFactorHelper::CalcReleaseTime(MF_DRAG, nWeight, 1.0f, ETID_NPC));
	}

}