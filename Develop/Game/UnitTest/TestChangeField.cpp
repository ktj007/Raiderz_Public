#include "stdafx.h"
#include "XUnitTestHelper.h"
#include "XGamePlayState.h"
#include "XMyPlayer.h"
#include "FTestGame.h"
#include "XUnitTestHelper.h"
#include "XUnitTestUtil.h"
#include "XPost_Local.h"
#include "MockGameApp.h"
#include "CCommandResultTable.h"

SUITE(ChangeField)
{
	class FChangeField : public FTestGame
	{
	public:
		FChangeField() : FTestGame() 
		{
		}
		virtual ~FChangeField() {}
	};

	TEST_FIXTURE(FChangeField, TestNewUIID_WhenChangeChannelAndMoveServer)
	{
		XMyPlayer* pMyPlayer = GetMyPlayer();
		pMyPlayer->GetActor()->SetAnimationUpdated(true);

		global.app->ChangeFrame(PLAY_FRAME_ID);
		Update(0.01f);

		UIID nFirstUIID = XGetMyUIID();
		XObjectManager* pObjectManager = gg.omgr;

		CHECK(pObjectManager->FindActor_UIID(nFirstUIID) != NULL);


		// XGamePlayState::FinalGame_ForChangeField 에서 필요한 부분만 가져옮 - 리팩토링 필요
		// clear world object
		if (pMyPlayer)
		{
			pObjectManager->Erase(pMyPlayer);
		}
		pObjectManager->DestroyAllObject();


		MUID uidNew = XUnitTestUtil::NewMUID();
		UIID nNewUIID = 1001;

		// 서버 이동 응답
		m_pNetAgent->OnRecv(MC_COMM_RESPONSE_MOVE_GAME_SERVER, 3, NEW_INT(CR_SUCCESS), NEW_UID(uidNew), NEW_USHORT(nNewUIID));

		// XGamePlayState::InitGame_ForChangeField 에서 필요한 부분만 가져옮 - 리팩토링 필요
		// create user and init
		pObjectManager->Add(pMyPlayer);

		CHECK(pObjectManager->FindActor_UIID(nNewUIID) != NULL);
	}
}