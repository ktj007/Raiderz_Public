#include "stdafx.h"
#include "XMotion.h"
#include "XModuleMotion.h"
#include "MockAnimationController.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "XUnitTestUtil.h"
#include "XTestWrapper.h"
#include "XMotionLogic.h"

SUITE (Motion)
{
	class FMotion
	{
	public:
		FMotion()
		{

		}
		virtual ~FMotion()
		{

		}

		void AddMotionIdle()
		{
			info.motion->AddMotionForTest(L"	<MOTION name='idle' stance='all'>		\
										  <Default>															\
										  <Sequence ani='idle'/>												\
										  </Default>															\
										  </MOTION>");

		}

		XTestInfoWrapper<XMotionMgr>	m_MotionInfoWrapper;
	};

	TEST(TestMockAnimationController)
	{
		TestAniDataSet		ani_set;
		ani_set.AddAni(L"idle", 1.0, true);
		ani_set.AddAni(L"jump", 2.0, false);

		MockAnimationController		mock_ac(NULL, NULL);
		mock_ac.SetAniDataSet(&ani_set);


		mock_ac.SetAnimation(wstring(L"idle"));
		CHECK_EQUAL(mock_ac.IsAnimationLoop(), true);
		CHECK_EQUAL(mock_ac.GetAniFrame(), 0);
		CHECK_EQUAL(mock_ac.GetAniMaxFrame(), 4800);
		CHECK(mock_ac.GetAnimationName() == wstring(L"idle"));
		CHECK_CLOSE(mock_ac.GetCurFrameToSecond(), 0.0f, 0.01f);
		CHECK_CLOSE(mock_ac.GetMaxFrameToSecond(), 1.0f, 0.01f);

		mock_ac.PlayAnimation();
		mock_ac.Update(0.5f);
		CHECK_EQUAL(mock_ac.GetAniFrame(), 2400);
		CHECK_CLOSE(mock_ac.GetCurFrameToSecond(), 0.5f, 0.01f);

		mock_ac.Update(0.5f);
		mock_ac.Update(0.5f);

		CHECK_EQUAL(mock_ac.GetAniFrame(), 2400);

		// jump로 변경
		mock_ac.SetAnimation(wstring(L"jump"));
		mock_ac.PlayAnimation();


		CHECK_EQUAL(mock_ac.IsAnimationLoop(), false);
		CHECK(mock_ac.GetAnimationName() == wstring(L"jump"));
		CHECK_EQUAL(mock_ac.GetAniFrame(), 0);

		mock_ac.Update(0.5f);
		mock_ac.Update(0.5f);
		mock_ac.Update(0.5f);
		mock_ac.Update(0.5f);
		mock_ac.Update(0.5f);
		mock_ac.Update(0.5f);

		CHECK_EQUAL(mock_ac.GetAniFrame(), 4800*2);
		CHECK_CLOSE(mock_ac.GetCurFrameToSecond(), mock_ac.GetMaxFrameToSecond(), 0.01f);
	}


	TEST_FIXTURE(FMotion, TestMotionAniNameMaker)
	{
		AddMotionIdle();

		XMotion* pIdleMotion = info.motion->GetMotion(L"idle");

		XMotionAniNameMaker maker;
		wstring strAniName;

		maker.MakePlayerAniName(strAniName, pIdleMotion, MT_DEFAULT, 0, WEAPON_NONE, CS_NORMAL);
		CHECK(strAniName == wstring(L"none_idle"));

		maker.MakePlayerAniName(strAniName, pIdleMotion, MT_DEFAULT, 0, WEAPON_TWO_HANDED, CS_NORMAL);
		CHECK(strAniName == wstring(L"none_idle"));

		maker.MakePlayerAniName(strAniName, pIdleMotion, MT_DEFAULT, 0, WEAPON_TWO_HANDED, CS_BATTLE);
		CHECK(strAniName == wstring(L"2hd_idle"));

		maker.MakeNPCAniName(strAniName, pIdleMotion, MT_DEFAULT, 0, 0, NULL);
		CHECK(strAniName == wstring(L"idle"));

		maker.MakeNPCAniName(strAniName, pIdleMotion, MT_DEFAULT, 0, NPC_MODE_1, NULL);
		CHECK(strAniName == wstring(L"mode1_idle"));
	}

	TEST_FIXTURE(FMotion, TestMotionAniNameMaker_NPCPrefixMode)
	{
		AddMotionIdle();

		XMotion* pIdleMotion = info.motion->GetMotion(L"idle");

		XMotionAniNameMaker maker;
		wstring strAniName;

		XNPCInfo	npcInfo;

		// prefix가 없을 때
		maker.MakeNPCAniName(strAniName, pIdleMotion, MT_DEFAULT, 0, NPC_MODE_1, &npcInfo);
		CHECK(strAniName == wstring(L"mode1_idle"));

		// prefix가 있을 때
		npcInfo.strAniPrefix = L"prefix_";

		maker.MakeNPCAniName(strAniName, pIdleMotion, MT_DEFAULT, 0, NPC_MODE_1, &npcInfo);
		CHECK(strAniName == wstring(L"prefix_idle"));
	}

	TEST(TestMotionAniSequence)
	{
		XMotionAniSequence aniSeq1;
		
		aniSeq1.SetAniName(L"Test1", false);
		CHECK(wcscmp(aniSeq1.GetAniName(), L"Test1") == 0);
		CHECK(wcscmp(aniSeq1.GetAniName(WEAPON_NONE), L"Test1") == 0);
		CHECK(wcscmp(aniSeq1.GetAniName(WEAPON_1H_SLASH), L"Test1") == 0);

		XMotionAniSequence aniSeq2;
		aniSeq2.SetAniName(L"Test2", true);
		CHECK(wcscmp(aniSeq2.GetAniName(), L"Test2") == 0);
		CHECK(wcscmp(aniSeq2.GetAniName(WEAPON_NONE), L"none_Test2") == 0);
		CHECK(wcscmp(aniSeq2.GetAniName(WEAPON_1H_SLASH), L"1hs_Test2") == 0);

	}

	TEST(TestMotionTrigger_Valid)
	{
		XMotionTrigger mif;

		CHECK_EQUAL(false, mif.IsValid());

		mif.m_nIfType = MTRIGGER_IF_DEAD;

		CHECK_EQUAL(false, mif.IsValid());

		mif.m_nThenType = MTRIGGER_THEN_HOLD;

		CHECK_EQUAL(true, mif.IsValid());
	}

	class FMotionForParseCameraLook : public FMotion
	{
	public:
		FMotionForParseCameraLook()
		{
			AddMotions();
		}

		void AddMotions()
		{
			info.motion->AddMotionForTest(L"	<MOTION name='motion_default' stance='all' >		\
										  <Default>															\
										  <Sequence ani='idle'/>												\
										  </Default>															\
										  </MOTION>");

			info.motion->AddMotionForTest(L"	<MOTION name='motion_true' stance='all' cameraLook='true'>		\
										  <Default>															\
										  <Sequence ani='idle'/>												\
										  </Default>															\
										  </MOTION>");

			info.motion->AddMotionForTest(L"	<MOTION name='motion_true_normal' stance='all' cameraLook='true' cameraLookStance='normal'>		\
										  <Default>															\
										  <Sequence ani='idle'/>												\
										  </Default>															\
										  </MOTION>");

			info.motion->AddMotionForTest(L"	<MOTION name='motion_true_battle' stance='all' cameraLook='true' cameraLookStance='battle'>		\
										  <Default>															\
										  <Sequence ani='idle'/>												\
										  </Default>															\
										  </MOTION>");

		}
	};

	TEST_FIXTURE(FMotionForParseCameraLook, TestMotion_ParseMotion_CameraLook_Default)
	{
		XMotion* pMotion = info.motion->GetMotion(L"motion_default");
		ASSERT_CHECK(pMotion != NULL);

		CHECK_EQUAL(false, pMotion->IsCameraLook());
		CHECK_EQUAL(false, pMotion->IsCameraLook(XMotion::MS_NORMAL));
		CHECK_EQUAL(false, pMotion->IsCameraLook(XMotion::MS_BATTLE));
		CHECK_EQUAL(false, pMotion->IsCameraLook(XMotion::MS_ALL));
	}

	TEST_FIXTURE(FMotionForParseCameraLook, TestMotion_ParseMotion_CameraLook_True)
	{
		XMotion* pMotion = info.motion->GetMotion(L"motion_true");
		ASSERT_CHECK(pMotion != NULL);

		CHECK_EQUAL(true, pMotion->IsCameraLook());
		CHECK_EQUAL(true, pMotion->IsCameraLook(XMotion::MS_NORMAL));
		CHECK_EQUAL(true, pMotion->IsCameraLook(XMotion::MS_BATTLE));
		CHECK_EQUAL(true, pMotion->IsCameraLook(XMotion::MS_ALL));
	}

	TEST_FIXTURE(FMotionForParseCameraLook, TestMotion_ParseMotion_CameraLook_True_Normal)
	{
		XMotion* pMotion = info.motion->GetMotion(L"motion_true_normal");
		ASSERT_CHECK(pMotion != NULL);

		CHECK_EQUAL(false, pMotion->IsCameraLook());
		CHECK_EQUAL(true, pMotion->IsCameraLook(XMotion::MS_NORMAL));
		CHECK_EQUAL(false, pMotion->IsCameraLook(XMotion::MS_BATTLE));
		CHECK_EQUAL(false, pMotion->IsCameraLook(XMotion::MS_ALL));
	}

	TEST_FIXTURE(FMotionForParseCameraLook, TestMotion_ParseMotion_CameraLook_True_Battle)
	{
		XMotion* pMotion = info.motion->GetMotion(L"motion_true_battle");
		ASSERT_CHECK(pMotion != NULL);

		CHECK_EQUAL(false, pMotion->IsCameraLook());
		CHECK_EQUAL(false, pMotion->IsCameraLook(XMotion::MS_NORMAL));
		CHECK_EQUAL(true, pMotion->IsCameraLook(XMotion::MS_BATTLE));
		CHECK_EQUAL(false, pMotion->IsCameraLook(XMotion::MS_ALL));
	}

}