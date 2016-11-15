#include "stdafx.h"

#include "XGame.h"
#include "XNetwork.h"
#include "TestModule.h"
#include "RSceneManager.h"
#include "XInteractionInfo.h"

SUITE(GameState)
{
	TEST(TestGameStateChange)
	{
		TestGame tGame;

		// 테스트에서 쓰이는 카메라 보관
		RCameraSceneNode* pCameraSceneNode = REngine::GetSceneManager().GetPrimaryCamera();
		
		tGame.GetGame()->OnChangeStart(GPS_CUTSCENE, -1);
		CHECK(tGame.GetGame()->GetCurrentGameState()->GetPlayState() == GPS_CUTSCENE);

		tGame.GetGame()->OnChangeStart(GPS_GAMEPLAY);
		CHECK(tGame.GetGame()->GetCurrentGameState()->GetPlayState() == GPS_GAMEPLAY);

		// 보관된 카메라 다시 셋팅
		REngine::GetSceneManager().SetPrimaryCamera(pCameraSceneNode);
	}

	TEST(TestCutSceneInteraction)
	{
		TestGame tGame;
		minet::MTestNetAgent		netAgent(global.net->GetClient());
		XMyPlayer* pMyPlayer = tGame.GetMyPlayer();
		CHECK(pMyPlayer != NULL);
		if (pMyPlayer == NULL) return;

		TestModuleContext	testContext(&tGame, &netAgent);
		testContext.MakeContextFieldLoadingCompleteIgnore();

		// net player
		XNetPlayer* pNetPlayer = testContext.CreateNetPlayerAndEnterToGmae(pMyPlayer->GetPosition(), vec3(1.f, 0.f, 0.f), SEX_FEMALE);
		CHECK(pNetPlayer != NULL);
		pNetPlayer->GetActor()->SetAnimationUpdated(true);

		testContext.UpdateGame(0.1f);
		CHECK(XGetInteractionInfo().GetInteractionSearchUID() == pNetPlayer->GetUID());

		pNetPlayer->SetCutScene(true);
		testContext.UpdateGame(0.1f);
		CHECK(XGetInteractionInfo().GetInteractionSearchUID() == MUID::ZERO);

		pNetPlayer->SetCutScene(false);
		testContext.UpdateGame(0.1f);
		CHECK(XGetInteractionInfo().GetInteractionSearchUID() == pNetPlayer->GetUID());
	}
}
