#include "stdafx.h"
#include "XGame.h"
#include "TestModule.h"
#include "XMotion.h"
#include "XModuleMotion.h"
#include "XModuleTalent.h"
#include "XCameraManager.h"
#include "XCharacter.h"
#include "RMesh.h"
#include "XNetwork.h"

SUITE(Sync)
{
	struct FSyncFixture
	{
		FSyncFixture()
		{
			m_pNetAgent = new minet::MTestNetAgent(global.net->GetClient());

			InitVars();
			InitContext();

			InitAni();
			InitTalentInfo();
		}

		~FSyncFixture()
		{
			SAFE_DELETE(m_pTestContext);
			SAFE_DELETE(m_pNetAgent);
		}

		void InitContext()
		{
			m_pTestContext = new TestModuleContext(&m_TestGame, m_pNetAgent);
			m_pTestContext->MakeContextFieldLoadingCompleteIgnore();

			m_pController = m_TestGame.GetController();

			m_pMyPlayer = m_pTestContext->GetMyPlayer();
			m_pMyPlayer->SetPosition(m_vStartPos);
			m_pMyPlayer->SetDirection(m_vStartDir);

			m_pNetPlayer = m_pTestContext->CreateNetPlayerAndEnterToGmae(m_vStartPos, m_vStartDir, SEX_FEMALE);

			m_uidMyPlayerUID = m_pMyPlayer->GetUID();
			m_uidNetPlayerUID = m_pNetPlayer->GetUID();

			m_nMyPlayerUIID = m_pMyPlayer->GetUIID();
			m_nNetPlayerUIID = m_pNetPlayer->GetUIID();

			XBaseCameraSceneNode* pCameraNode = global.camera->GetCamera();
			if (pCameraNode)
			{
				pCameraNode->SetDirection(m_vStartDir);
			}

		}

		void InitVars()
		{
			m_nTumbleTalentID = 1;
			m_vStartPos = vec3(1000.0f, 1000.0f, 0.0f);
			m_vStartDir = vec3(0.0f, 1.0f, 0.0f);
		}
		void InitAni()
		{
			TestAniDataSet& aniSet = m_TestGame.GetPlayerAniDataSet();

			m_Helper.SetAniDataSet_Player(&aniSet);

			aniSet.AddAni(L"none_tumble", 1.0, false);		

			TestAniData* pTumbleAni = aniSet.GetAniData(L"none_tumble");
			if (pTumbleAni)
			{
				// 이동 애니 0.1초마다 50cm씩 앞으로 이동한다.
				for (int i = 0; i < 10; i++)
				{
					pTumbleAni->AddDummyLocPer100ms(vec3(0.0f, i*(-50.0f), 0.0f), vec3(0.0f, -1.0f, 0.0f));
				}
			}

			info.motion->AddMotionForTest(L"	<MOTION name='idle' stance='all' cameraLook='true'>		\
										  <Default>									\
										  <Sequence ani='idle'/>					\
										  </Default>								\
										  </MOTION>");
			info.motion->AddMotionForTest(L"	<MOTION name='PS_Tumble' stance='all' cameraLook='true'>		\
										  <Default>									\
										  <Sequence ani='tumble'/>					\
										  </Default>								\
										  </MOTION>");

			info.motion->AddMotionForTest(
				L"<MOTION name='run' stance='all' loop='true' cancelEnabled='true' cameraLook='true'>	\
				<Default>	<Sequence ani='run_f'/> </Default>	\
				<Forward>	<Sequence ani='run_f'/> </Forward>	\
				<Backward>	<Sequence ani='run_b'/> </Backward>	\
				<Left>		<Sequence ani='run_l'/> </Left>		\
				<Right>		<Sequence ani='run_r'/> </Right>	\
				</MOTION>");

		}
		void InitTalentInfo()
		{
			m_Helper.CreateTalentInfo(m_nTumbleTalentID, 
				L"<TALENT name=\"Tumble\" TalentType=\"skill\" Category=\"melee\" SkillType=\"melee\" Stance='all' \
				EffectTiming=\"hit_this_talent\" HitTestType=\"area\" HitMotionDelay=\"0\" \
				HPCost=\"0\" APCost=\"0\" ENCost=\"0\" STACost=\"0\" HPTickCost=\"0\" ENTickCost=\"0\" STATickCost=\"0\" CoolTime=\"0\" \
				UseAni=\"PS_Tumble\" AttackDirection=\"front\" IsMoving=\"True\" IsSetDirection=\"True\" MF_BEATEN=\"0\" MF_WEIGHT_BEATEN=\"0\" \
				MF_KNOCKBACK=\"0\" MF_WEIGHT_KNOCKBACK=\"0\" MF_STUN=\"0\" MF_WEIGHT_STUN=\"0\" MF_DEFLECT=\"0\" MF_WEIGHT_DEFLECT=\"0\" \
				MF_THROWUP=\"200\" MF_WEIGHT_THROWUP=\"1000\" MF_KNOCKDOWN=\"0\" MF_WEIGHT_KNOCKDOWN=\"0\" \
				MF_UPPERED=\"0\" MF_WEIGHT_UPPERED=\"0\" DamageAttrib=\"blunt\" Guardable=\"True\" Cancelable=\"False\" />"
											);

		}
		void UpdateGame(float fDelta)
		{
			m_pTestContext->UpdateGame(fDelta);
			m_pTestContext->UpdateGame(0.0001f);
		}

		minet::MTestNetAgent*		m_pNetAgent;
		TestGame			m_TestGame;
		TestModuleContext*	m_pTestContext;
		XMyPlayer*			m_pMyPlayer;
		XNetPlayer*			m_pNetPlayer;
		XUnitTestHelper		m_Helper;
		MockController*		m_pController;

		vec3				m_vStartPos;
		vec3				m_vStartDir;
		MUID				m_uidMyPlayerUID;
		MUID				m_uidNetPlayerUID;
		
		UIID				m_nMyPlayerUIID;
		UIID				m_nNetPlayerUIID;
		int					m_nTumbleTalentID;

		XTestInfoWrapper<XMotionMgr>							m_MotionInfoWrapper;
		XTestInfoWrapper2<XTalentInfoMgr, MockTalentInfoMgr>	m_TalentInfoWrapper;
	};


	TEST_FIXTURE(FSyncFixture, TestFixtureValid)
	{
		CHECK(m_uidMyPlayerUID.IsValid());
		CHECK(m_uidNetPlayerUID.IsValid());

		CHECK(m_pMyPlayer != NULL);
		CHECK(m_pNetPlayer != NULL);

		CHECK_CLOSE(0.0f, m_pMyPlayer->GetPosition().DistanceTo(m_vStartPos), 0.001f);
		CHECK_CLOSE(m_vStartDir.x, m_pMyPlayer->GetDirection().x, 0.001f);
		CHECK_CLOSE(m_vStartDir.y, m_pMyPlayer->GetDirection().y, 0.001f);
		CHECK_CLOSE(m_vStartDir.z, m_pMyPlayer->GetDirection().z, 0.001f);
	}

	TEST_FIXTURE(FSyncFixture, TestMovingAnimation)
	{
		m_pMyPlayer->GetModuleTalent()->Start(m_nTumbleTalentID);

		UpdateGame(0.1f);
		CHECK_CLOSE(50.0f, m_pMyPlayer->GetPosition().DistanceTo(m_vStartPos), 0.001f);

		UpdateGame(0.1f);
		CHECK_CLOSE(100.0f, m_pMyPlayer->GetPosition().DistanceTo(m_vStartPos), 0.001f);

	}

	TEST_FIXTURE(FSyncFixture, TestMyPlayerPosSync_Send_WhenMovingTalent)
	{
		// 좌우로 왔다갔다 하다가
		m_pController->TestKeyPress(VKEY_LEFT);
		UpdateGame(0.1f);
		UpdateGame(0.1f);

		m_pController->TestKeyRelease(VKEY_LEFT);
		m_pController->TestKeyPress(VKEY_RIGHT);
		UpdateGame(0.1f);
		UpdateGame(0.1f);
		m_pController->TestKeyRelease(VKEY_RIGHT);


		m_pController->TestKeyPress(VKEY_LEFT);
		UpdateGame(0.1f);
		UpdateGame(0.1f);

		m_pController->TestKeyRelease(VKEY_LEFT);
		m_pController->TestKeyPress(VKEY_RIGHT);
		UpdateGame(0.1f);
		UpdateGame(0.1f);
		m_pController->TestKeyRelease(VKEY_RIGHT);

		XModuleMyControl* pModuleMyControl = m_pMyPlayer->GetModuleMyControl();
		pModuleMyControl->DoActionUseTalent(m_nTumbleTalentID);
		UpdateGame(0.2f);
		UpdateGame(0.2f);
		UpdateGame(0.2f);
		UpdateGame(0.2f);
		UpdateGame(0.2f);


		
	}

	TEST_FIXTURE(FSyncFixture, TestNetPlayerPosSync_WhenMovingTalentAndRotation)
	{
		m_pNetAgent->OnRecv( MC_ACTION_USE_TALENT, 4,

			NEW_USHORT( m_nNetPlayerUIID ),
			NEW_INT(m_nTumbleTalentID),
			NEW_SVEC(m_vStartDir),
			NEW_VEC(m_vStartPos)
			);

		UpdateGame(0.1f);
		CHECK_CLOSE(50.0f, m_pNetPlayer->GetPosition().DistanceTo(m_vStartPos), 0.001f);

	}
}

// MyPlayer 생성[passed]
// NetPlayer 생성[passed]
// 애니메이션 셋 생성[passed]
// 이동 탤런트 생성[passed]

// NetPlayer가 탤런트 사용
// 회전
// 위치 체크
