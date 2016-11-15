#include "stdafx.h"
#include "TestModule.h"
#include "XMyPlayer.h"
#include "XModuleMyControl.h"
#include "XModuleMotion.h"
#include "XUnitTestUtil.h"
#include "XNetwork.h"

SUITE(TestSwim)
{
	TEST(TestMyPlayerSwimInOut)
	{
		TestGame		tg;

		XMyPlayer* pMyPlayer = tg.GetMyPlayer();
		if (pMyPlayer == NULL) return;

		// 수영 애니메이션 셋팅
		TestAniDataSet		ani_set;
		ani_set.AddAni(L"idle", 1.0, true);
		ani_set.AddAni(L"swim_idle", 1.33, true);

		MockAnimationController		mock_ac(NULL, NULL);
		mock_ac.SetAniDataSet(&ani_set);

		// 모션 셋팅
		XTestInfoWrapper<XMotionMgr>	m_MotionInfoWrapper;
		info.motion->AddMotionForTest(L"	<MOTION name='idle' stance='all'>		\
									  <Default>															\
									  <Sequence ani='idle'/>												\
									  </Default>															\
									  </MOTION>");

		info.motion->AddMotionForTest(L"	<MOTION name='swimidle' stance='all'>		\
									  <Default>															\
									  <Sequence ani='swim_idle'/>												\
									  </Default>															\
									  </MOTION>");

		mock_ac.SetAnimation(wstring(L"idle"));

		// 물속으로 들어갈때
		XEvent msg;
		msg.m_nID = XEVTL_SWIM_IN;

		bool bTest = false;
		msg.m_pData = (void*)&bTest;
		pMyPlayer->Event(msg);
		CHECK(pMyPlayer->IsSwim() == true);

		XModuleMotion* pModuleMotion = pMyPlayer->GetModuleMotion();
		if(pModuleMotion == NULL)
			return;

		CHECK(pModuleMotion->GetCurrMotion() == wstring(L"swimidle"));

		// 물밖으로 나올때
		msg.m_nID = XEVTL_SWIM_OUT;
		pMyPlayer->Event(msg);
		CHECK(pMyPlayer->IsSwim() == false);
		CHECK(pModuleMotion->GetCurrMotion() == wstring(L"idle"));
	}

	TEST(TestNetPlayerSwimInOut)
	{
		// 물속으로 들어갈때
		// build context
		TestGame			testGame;
		minet::MTestNetAgent		netAgent(global.net->GetClient());
		TestModuleContext	testContext( &testGame, &netAgent);
		testContext.MakeContextFieldLoadingCompleteIgnore();


		vec3 vFirstPos = vec3(  0.0f, 0.0f, 0.0f);
		vec3 vFirstDir = vec3( -1.0f, 0.0f, 0.0f);
		vec3 vLastDir  = vec3(  1.0f, 0.0f, 0.0f);


		// Create net player
		XNetPlayer* pNetPlayer = testContext.CreateNetPlayerAndEnterToGmae( vFirstPos, vFirstDir, SEX_FEMALE);
		CHECK( pNetPlayer != NULL);

		// 수영 애니메이션 셋팅
		TestAniDataSet		ani_set;
		ani_set.AddAni(L"idle", 1.0, true);
		ani_set.AddAni(L"swim_idle", 1.33, true);

		MockAnimationController		mock_ac(NULL, NULL);
		mock_ac.SetAniDataSet(&ani_set);

		// 모션 셋팅
		XTestInfoWrapper<XMotionMgr>	m_MotionInfoWrapper;
		info.motion->AddMotionForTest(L"	<MOTION name='idle' stance='all'>		\
									  <Default>															\
									  <Sequence ani='idle'/>												\
									  </Default>															\
									  </MOTION>");

		info.motion->AddMotionForTest(L"	<MOTION name='swimidle' stance='all'>		\
									  <Default>															\
									  <Sequence ani='swim_idle'/>												\
									  </Default>															\
									  </MOTION>");

		mock_ac.SetAnimation(wstring(L"idle"));

		// 물속으로 들어갈때
		XEvent msg;
		msg.m_nID = XEVTL_SWIM_IN;

		bool bTest = false;
		msg.m_pData = (void*)&bTest;
		pNetPlayer->Event(msg);
		CHECK(pNetPlayer->IsSwim() == true);

		XModuleMotion* pModuleMotion = pNetPlayer->GetModuleMotion();
		if(pModuleMotion == NULL)
			return;

		CHECK(pModuleMotion->GetCurrMotion() == wstring(L"swimidle"));

		// 물밖으로 나올때
		msg.m_nID = XEVTL_SWIM_OUT;
		pNetPlayer->Event(msg);
		CHECK(pNetPlayer->IsSwim() == false);
		CHECK(pModuleMotion->GetCurrMotion() == wstring(L"idle"));
	}
}

