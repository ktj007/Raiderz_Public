#include "stdafx.h"
#include "XMotionTrigger_Checker.h"
#include "XMotionTrigger_Execute.h"
#include "XMotion.h"
#include "MockPlayer.h"
#include "XTestWrapper.h"

SUITE(MotionTrigger)
{
	struct FMotionTriggerFixture
	{
		FMotionTriggerFixture()
		{
			info.motion->AddMotionForTest(L"	<MOTION name='idle' stance='all'>		\
										  <Default>									\
										  <Sequence ani='idle1'>					\
										  <PostTrigger if='Dead' then='Hold' param1='1'/>		\
										  </Sequence>								\
										  <Sequence ani='idle2'/>					\
										  </Default>								\
										  </MOTION>");
		}
		~FMotionTriggerFixture()
		{

		}
		XTestInfoWrapper<XMotionMgr>	m_MotionInfoWrapper;
	};

	TEST_FIXTURE(FMotionTriggerFixture, TestMotionTrigger_Exist)
	{
		XMotion* pMotion = info.motion->GetMotion(L"idle");
		CHECK_EQUAL(true, pMotion->IsExistTrigger());
	}

	TEST_FIXTURE(FMotionTriggerFixture, TestMotionTrigger_Parsing)
	{
		XMotion* pMotion = info.motion->GetMotion(L"idle");
		CHECK(pMotion != NULL);

		XMotionAniSequence* pSeq1 = pMotion->GetAniSequence(MT_DEFAULT, 0);
		CHECK(pSeq1 != NULL);

		CHECK_EQUAL(true, pSeq1->GetPostTrigger()->IsValid());
		CHECK_EQUAL(MTRIGGER_IF_DEAD, pSeq1->GetPostTrigger()->m_nIfType);
		CHECK_EQUAL(MTRIGGER_THEN_HOLD, pSeq1->GetPostTrigger()->m_nThenType);
		CHECK(wstring(L"1") == pSeq1->GetPostTrigger()->m_strThenParam1);
	}

	TEST(TestMotionTrigger_Checker)
	{
		XMotionTrigger_Checker checker;

		CHECK_EQUAL(false, checker.Check(MTRIGGER_IF_NONE, NULL));

		CHECK_EQUAL(true, checker.Check(MTRIGGER_IF_TRUE, NULL));
	}

	TEST(TestMotionTrigger_IfDead)
	{
		MockPlayer player(MUID::ZERO, SEX_FEMALE);
		XMotionTrigger_Checker checker;

		CHECK_EQUAL(false, checker.Check(MTRIGGER_IF_DEAD, &player));

		player.SetDead(true);
		CHECK_EQUAL(true, checker.Check(MTRIGGER_IF_DEAD, &player));
	}
}