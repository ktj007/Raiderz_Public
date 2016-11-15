#include "stdafx.h"
#include "XActionDie.h"
#include "MockPlayer.h"
#include "MockAnimationController.h"
#include "MockModuleMotion.h"
#include "MockModuleAction.h"
#include "XMotionLogic.h"
#include "XUpperMotionLogic.h"
#include "XTestWrapper.h"
#include "XMotionTimeModifier.h"
#include "XModuleAction.h"

SUITE(ActionDie)
{
	struct FActionDieMotionFixture
	{
		FActionDieMotionFixture()
		{
			AniDataSet.AddAni(L"none_idle", 1.0, true);
			AniDataSet.AddAni(L"none_die", 1.0, true);
			AniDataSet.AddAni(L"none_MF_BT1", 1.0, true);
			AniDataSet.AddAni(L"none_MF_BT2", 1.0, true);
			AniDataSet.AddAni(L"none_MF_KB1", 1.0, true);
			AniDataSet.AddAni(L"none_MF_KB2", 1.0, true);
			AniDataSet.AddAni(L"none_MF_KB3", 1.0, true);
			AniDataSet.AddAni(L"none_MF_TU1", 1.0, true);
			AniDataSet.AddAni(L"none_MF_TU2", 1.0, true);
			AniDataSet.AddAni(L"none_MF_KD1", 1.0, true);
			AniDataSet.AddAni(L"none_MF_KD2", 1.0, true);
			AniDataSet.AddAni(L"none_MF_KD3", 1.0, true);

			m_pAnimationController = new MockAnimationController(NULL, NULL);
			m_pAnimationController->SetAniDataSet(&AniDataSet);

			m_pMockPlayer = new MockPlayer(MUID::ZERO, SEX_FEMALE);

			m_pMotionLogic = new XMotionLogic(m_pMockPlayer, m_pAnimationController, new XMotionTimeModifier(NULL, NULL), NULL);
			m_pUpperMotionLogic = new XUpperMotionLogic(m_pMockPlayer, m_pAnimationController, new XMotionTimeModifier(NULL, NULL), NULL);

			m_pMockModuleMotion = new MockModuleMotion(m_pMockPlayer);
			m_pMockModuleMotion->Init(m_pMotionLogic, m_pUpperMotionLogic);

			m_pMockModuleAction = new MockModuleAction(m_pMockPlayer);
			m_pMockModuleAction->Init(m_pMockModuleMotion);

			m_pMockPlayer->SetMockModuleMotion(m_pMockModuleMotion, true);
			m_pMockPlayer->SetMockModuleAction(m_pMockModuleAction);
			m_pMockPlayer->SetMockModuleCollision(NULL);				// 충돌은 쓰지 않겠습니다.

			InitMotionMgr();
		}
		virtual ~FActionDieMotionFixture()
		{
			SAFE_DELETE(m_pMockPlayer);
			//SAFE_DELETE(m_pMotionLogic);
			//SAFE_DELETE(m_pAnimationController);
			//SAFE_DELETE(m_pMockModuleMotion);
			//SAFE_DELETE(m_pMockModuleAction);
		}
		void InitMotionMgr()
		{
			info.motion->AddMotionForTest(L"	<MOTION name='idle' stance='all' loop='true'>		\
										   <Default>									\
										   <Sequence ani='idle'/>					\
										   </Default>								\
										   </MOTION>");

			info.motion->AddMotionForTest(L"	<MOTION name='die' type='once' cancelEnabled='false' cameraLook='false'>		\
										   <Default>									\
										   <Sequence ani='die'/>					\
										   </Default>								\
										   </MOTION>");

			info.motion->AddMotionForTest(L"	<MOTION name='mf_beaten' stance='all' loop='false' cancelEnabled='true' cameraLook='true'>		\
										   <Default>									\
										   <Sequence ani='MF_BT1'/>					\
										   </Default>								\
										   </MOTION>");

			info.motion->AddMotionForTest(L"	<MOTION name='mf_knockback' stance='all' loop='false' cancelEnabled='true' cameraLook='false'>		\
										   <Default>									\
										   <Sequence ani='MF_KB1'/>						\
										   <Sequence ani='MF_KB2'' event='mf_kb_event'>	\
												<PostTrigger if='Dead'' then='ChangeMotion'' param1='die'' />	\
										   </Sequence>									\
										   <Sequence ani='MF_KB3'/>						\
										   </Default>								\
										   </MOTION>");

			info.motion->AddMotionForTest(L"	<MOTION name='mf_throwup' stance='all' loop='false' cancelEnabled='true' cameraLook='false'>		\
										   <Default>									\
										   <Sequence ani='MF_TU1' event='mf_kb_event' />\
										   <Sequence ani='MF_TU2'>						\
										   <PostTrigger if='Dead' then='Hold'/>			\
										   </Sequence>									\
										   <Sequence ani='MF_KD3'/>						\
										   </Default>								\
										   </MOTION>");

			info.motion->AddMotionForTest(L"	<MOTION name='mf_knockdown' stance='all' loop='false' cancelEnabled='true' cameraLook='false'>		\
										   <Default>									\
										   <Sequence ani='MF_KD1'/>						\
										   <Sequence ani='MF_KD2' event='mf_kd_event'>	\
										   <PostTrigger if='Dead' then='Hold'/>			\
										   </Sequence>									\
										   <Sequence ani='MF_KD3'/>						\
										   </Default>								\
										   </MOTION>");

			info.motion->AddMotionForTest(L"	<MOTION name='mf_grappled_collision' stance='all' loop='false' cancelEnabled='true' cameraLook='false'>		\
										   <Default>									\
										   <Sequence ani='MF_TU2'>						\
										   <PostTrigger if='Dead' then='Hold'/>			\
										   </Sequence>									\
										   </Default>								\
										   </MOTION>");

			info.motion->AddMotionForTest(L"	<MOTION name='mf_uppered_short' stance='all' loop='false' cancelEnabled='true' cameraLook='false'>		\
										   <Default>									\
										   <Sequence ani='MF_BT2' speed='1.5'	event='event_landed' 	>\
										   <PostTrigger if='Dead' then='ChangeMotion' param1='die' />\
										   </Sequence>\
										   <Sequence ani='jump_land'/>\
										   </Default>								\
										   </MOTION>");
		}
		void Update(float fDelta)
		{
			m_pAnimationController->Update(fDelta);
			m_pMotionLogic->Update(fDelta);
		}

		void ChangeMotion(const wchar_t* szMotionName)
		{
			XModuleMotion* pModuleMotion = m_pMockPlayer->GetModuleMotion();
			if(pModuleMotion)
			{
				pModuleMotion->ChangeMotion(szMotionName);
			}

		}

		MockPlayer* m_pMockPlayer;
		MockAnimationController* m_pAnimationController;
		MockModuleMotion* m_pMockModuleMotion;
		MockModuleAction* m_pMockModuleAction;

		XMotionLogic* m_pMotionLogic;
		XUpperMotionLogic* m_pUpperMotionLogic;

		TestAniDataSet		AniDataSet;

		XTestInfoWrapper<XMotionMgr>	m_MotionInfoWrapper;
	};

	//////////////////////////////////////////////////////////////////////////
	// 이테스트는 패킷 순서가 아니라 처리 순서에 의한 것입니다.

	//////////////////////////////////////////////////////////////////////////
	// 1. 일반 피격중에 다이가 왔다.
	TEST_FIXTURE(FActionDieMotionFixture, TestActionDie_normal)
	{
		ChangeMotion(L"mf_beaten");
		
		XModuleAction* pModuleAction = m_pMockPlayer->GetModuleAction();
		if(pModuleAction)
		{
			pModuleAction->Die();
		}

		CHECK(wstring(L"die") == m_pMotionLogic->GetCurrMotion());

	}

	// 2. 모션팩터 피격중에 다이가 왔다.
	TEST_FIXTURE(FActionDieMotionFixture, TestActionDie_MF)
	{
		ChangeMotion(L"mf_knockdown");

		XModuleAction* pModuleAction = m_pMockPlayer->GetModuleAction();
		if(pModuleAction)
		{
			pModuleAction->Die();
		}

		CHECK(wstring(L"mf_knockdown") == m_pMotionLogic->GetCurrMotion());
	}


	// 3. 모션 팩터 애니메이션이 없는 경우
	TEST_FIXTURE(FActionDieMotionFixture, TestActionDie_no_MF_animation)
	{
		ChangeMotion(L"idle");

		XModuleAction* pModuleAction = m_pMockPlayer->GetModuleAction();
		if(pModuleAction)
		{
			pModuleAction->Die(MF_THROWUP, false);
		}

		CHECK(wstring(L"die") == m_pMotionLogic->GetCurrMotion());

	}

	// 4. Upper 모션 팩터 이후에 죽는 애니메이션이 나온가?
	TEST_FIXTURE(FActionDieMotionFixture, TestActionDie_upper)
	{
		ChangeMotion(L"mf_uppered_short");

		m_pMockPlayer->SetDead(true);

		m_pMockPlayer->Update(1.1f);


		// 이벤트 보내기
		XEvent msg;
		msg.m_nID = XEVTL_ON_MOTION_EVENT; 
		msg.m_pData = MOTION_EVENT_LANDED;
		m_pMockPlayer->Event(msg);

		CHECK(wstring(L"die") == m_pMotionLogic->GetCurrMotion());
	}

	// 5. 넉다운 모션 팩터 이후에 죽는 애니메이션이 아니라 Hold가 되는가?
	TEST_FIXTURE(FActionDieMotionFixture, TestActionDie_knockdown)
	{
		ChangeMotion(L"mf_knockdown");

		m_pMockPlayer->SetDead(true);

		m_pMockPlayer->Update(2.1f);

		// 이벤트 보내기
		XEvent msg;
		msg.m_nID = XEVTL_ON_MOTION_EVENT; 
		msg.m_pData = MOTION_EVENT_MF_KNOCKDOWN_EVENT;
		m_pMockPlayer->Event(msg);

		CHECK(wstring(L"mf_knockdown") == m_pMotionLogic->GetCurrMotion());
	}

}