#include "stdafx.h"
#include "XTalentHitParam.h"
#include "XUnitTestUtil.h"

SUITE(TalentHitParam)
{
	class FTalentHitParam
	{
	public:
		FTalentHitParam()
		{

		}
		~FTalentHitParam()
		{

		}

		void CHECK_TalentHitParam(XTalentHitParam* pTalentHitParam, TD_TALENT_HIT* td_talent_hit, TD_TALENT_HIT_EX_KNOCKBACK* td_ex_knockback = NULL)
		{
			CHECK_EQUAL(td_talent_hit->nAttackerUIID,		pTalentHitParam->nAttackerUIID);
			CHECK_EQUAL(td_talent_hit->nTalentID,			pTalentHitParam->nTalentID);
			CHECK_EQUAL((MF_STATE)td_talent_hit->nMFState,	pTalentHitParam->nMFState);
			CHECK_EQUAL(td_talent_hit->nMFWeight,			pTalentHitParam->nMFWeight);
			CHECK_EQUAL(td_talent_hit->nDamage,				pTalentHitParam->nDamage);
			CHECK_EQUAL(td_talent_hit->nFlags,				pTalentHitParam->nFlags);
			CHECK_EQUAL(td_talent_hit->nHitTime,			pTalentHitParam->GetHitTime());

			if (td_ex_knockback)
			{
				CHECK_CLOSE(td_ex_knockback->vKnockbackDir.X(),			pTalentHitParam->vServerKnockbackDir.x, 0.001f);
				CHECK_CLOSE(td_ex_knockback->vKnockbackDir.Y(),			pTalentHitParam->vServerKnockbackDir.y, 0.001f);
				CHECK_EQUAL(td_ex_knockback->vKnockbackVictimPos,		pTalentHitParam->vServerKnockbackVictimPos);
			}
		}
	};

	TEST(TestMakeDelayTime)
	{
		XTalentHitParam param;
		
		TD_TALENT_HIT td_hit;
		td_hit.nHitTime = 100;		// 탤런트 시작후 100ms 후에 판정함

		param.Build(&td_hit);

		uint32 delay = param.MakeDelayTime(0.0f, 0);

		CHECK_EQUAL(100, delay);

		// 탤런트 Act 시작한지 50ms가 지났다.
		delay = param.MakeDelayTime(0.05f, 0);
		CHECK_EQUAL(50, delay);
		

		// 탤런트 Act 시작한지 1000ms가 지났다.
		delay = param.MakeDelayTime(1.0f, 0);
		CHECK_EQUAL(0, delay);
	}

	TEST_FIXTURE(FTalentHitParam, TestBuild)
	{
		TD_TALENT_HIT td_hit;
		td_hit.nAttackerUIID = XUnitTestUtil::NewUIID();
		td_hit.nTalentID = 1551;
		td_hit.nMFState = MF_BEATEN;
		td_hit.nMFWeight = 500;
		td_hit.nDamage = 540;
		td_hit.nFlags = 2;
		td_hit.nHitTime = 100;

		XTalentHitParam param;
		param.Build(&td_hit);

		CHECK_TalentHitParam(&param, &td_hit);
	}


	TEST_FIXTURE(FTalentHitParam, TestBuild_KnockBack)
	{
		TD_TALENT_HIT td_hit;
		td_hit.nAttackerUIID = XUnitTestUtil::NewUIID();
		td_hit.nTalentID = 1;
		td_hit.nMFState = MF_KNOCKBACK;
		td_hit.nMFWeight = 1000;
		td_hit.nDamage = 10;
		td_hit.nFlags = 0;
		td_hit.nHitTime = 0;

		TD_TALENT_HIT_EX_KNOCKBACK td_ex_knockback;
		td_ex_knockback.vKnockbackDir = vec3(0.5f, 0.2f, 0.0f);
		td_ex_knockback.vKnockbackVictimPos = vec3(1000.0f, 0.0f, 0.0f);

		XTalentHitParam param;
		param.Build_KnockBack(&td_hit, &td_ex_knockback);

		CHECK_TalentHitParam(&param, &td_hit, &td_ex_knockback);
	}


	TEST_FIXTURE(FTalentHitParam, TestBuild_NoMotionFactor)
	{
		UIID nAttackerUIID = 1;
		UIID nVictimUIID = 2;
		int nTalentID = 10;

		XTalentHitParam param;
		param.Build_NoMotionFactor(nAttackerUIID, nVictimUIID, nTalentID);

		CHECK_EQUAL(nAttackerUIID,		param.nAttackerUIID);
		CHECK_EQUAL(nVictimUIID,		param.nVictimUIID);
		CHECK_EQUAL(nTalentID,			param.nTalentID);

		CHECK_EQUAL(MF_NONE,			param.nMFState);
		CHECK_EQUAL(0,					param.nMFWeight);
		CHECK_EQUAL(0,					param.nDamage);
		CHECK_EQUAL(0,					param.nFlags);
		CHECK_EQUAL(0,					param.GetHitTime());
	}

	TEST_FIXTURE(FTalentHitParam, TestBuild_BuffNoMotionFactor)
	{
		UIID nVictimUIID = 1;
		int nTalentID = 10;

		XTalentHitParam param;
		param.Build_BuffNoMotionFactor(nVictimUIID, nTalentID);

		CHECK_EQUAL(UIID_INVALID,		param.nAttackerUIID);
		CHECK_EQUAL(nVictimUIID,		param.nVictimUIID);
		CHECK_EQUAL(nTalentID,			param.nTalentID);

		CHECK_EQUAL(MF_NONE,			param.nMFState);
		CHECK_EQUAL(0,					param.nMFWeight);
		CHECK_EQUAL(0,					param.nDamage);
		CHECK_EQUAL(0,					param.nFlags);
		CHECK_EQUAL(0,					param.GetHitTime());
	}


	TEST_FIXTURE(FTalentHitParam, TestFlag_Miss)
	{
		XTalentHitParam param;

		param.nFlags = 0;
		CHECK_EQUAL(false, param.IsFlagMiss());

		SetBitSet(param.nFlags, CTR_MISS);
		CHECK_EQUAL(true, param.IsFlagMiss());
	}

	TEST_FIXTURE(FTalentHitParam, TestFlag_Riposte)
	{
		XTalentHitParam param;

		param.nFlags = 0;
		CHECK_EQUAL(false, param.IsFlagRiposte());

		SetBitSet(param.nFlags, CTR_RIPOSTE);
		CHECK_EQUAL(true, param.IsFlagRiposte());
	}

	TEST_FIXTURE(FTalentHitParam, TestFlag_ResistPerfect)
	{
		XTalentHitParam param;

		param.nFlags = 0;
		CHECK_EQUAL(false, param.IsFlagResistPerfect());

		SetBitSet(param.nFlags, CTR_RESIST_PERFECT);
		CHECK_EQUAL(true, param.IsFlagResistPerfect());
	}

	TEST_FIXTURE(FTalentHitParam, TestFlag_ResistPartial)
	{
		XTalentHitParam param;

		param.nFlags = 0;
		CHECK_EQUAL(false, param.IsFlagResistPartial());

		SetBitSet(param.nFlags, CTR_RESIST_PARTIAL);
		CHECK_EQUAL(true, param.IsFlagResistPartial());
	}

	TEST_FIXTURE(FTalentHitParam, TestFlag_Critical)
	{
		XTalentHitParam param;

		param.nFlags = 0;
		CHECK_EQUAL(false, param.IsFlagCritical());

		SetBitSet(param.nFlags, CTR_CRITICAL);
		CHECK_EQUAL(true, param.IsFlagCritical());
	}

	TEST_FIXTURE(FTalentHitParam, TestFlag_Drain)
	{
		XTalentHitParam param;

		param.nFlags = 0;
		CHECK_EQUAL(false, param.IsFlagDrain());

		SetBitSet(param.nFlags, CTR_DRAIN);
		CHECK_EQUAL(true, param.IsFlagDrain());
	}

	TEST_FIXTURE(FTalentHitParam, TestFlag_ZeroTalentDamage)
	{
		XTalentHitParam param;

		param.nFlags = 0;
		CHECK_EQUAL(false, param.IsFlagZeroDamage());

		SetBitSet(param.nFlags, CTR_ZERO_TALENT_DAMAGE);
		CHECK_EQUAL(true, param.IsFlagZeroDamage());
	}

	TEST_FIXTURE(FTalentHitParam, TestFlag_Immune)
	{
		XTalentHitParam param;

		param.nFlags = 0;
		CHECK_EQUAL(false, param.IsFlagImmune());

		SetBitSet(param.nFlags, CTR_IMMUNE);
		CHECK_EQUAL(true, param.IsFlagImmune());
	}

	TEST_FIXTURE(FTalentHitParam, TestFlag_Avoid)
	{
		XTalentHitParam param;

		param.nFlags = 0;
		CHECK_EQUAL(false, param.IsFlagAvoid());

		SetBitSet(param.nFlags, CTR_AVOID);
		CHECK_EQUAL(true, param.IsFlagAvoid());
	}

	TEST_FIXTURE(FTalentHitParam, TestFlag_Falling)
	{
		XTalentHitParam param;

		param.nFlags = 0;
		CHECK_EQUAL(false, param.IsFlagFalling());

		SetBitSet(param.nFlags, CTR_FALLING);
		CHECK_EQUAL(true, param.IsFlagFalling());
	}

	TEST_FIXTURE(FTalentHitParam, TestFlag_Dead)
	{
		XTalentHitParam param;

		param.nFlags = 0;
		CHECK_EQUAL(false, param.IsFlagDead());

		SetBitSet(param.nFlags, CTR_DEAD);
		CHECK_EQUAL(true, param.IsFlagDead());
	}

	TEST_FIXTURE(FTalentHitParam, TestFlag_HitBPart)
	{
		XTalentHitParam param;

		param.nFlags = 0;
		CHECK_EQUAL(false, param.IsFlagBPart1());
		CHECK_EQUAL(false, param.IsFlagBPart2());
		CHECK_EQUAL(false, param.IsFlagBPart3());
		CHECK_EQUAL(false, param.IsFlagBPart4());

		SetBitSet(param.nFlags, CTR_HIT_BPART1);
		SetBitSet(param.nFlags, CTR_HIT_BPART2);
		SetBitSet(param.nFlags, CTR_HIT_BPART3);
		SetBitSet(param.nFlags, CTR_HIT_BPART4);

		CHECK_EQUAL(true, param.IsFlagBPart1());
		CHECK_EQUAL(true, param.IsFlagBPart2());
		CHECK_EQUAL(true, param.IsFlagBPart3());
		CHECK_EQUAL(true, param.IsFlagBPart4());
	}

}